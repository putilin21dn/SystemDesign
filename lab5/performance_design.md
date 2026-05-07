# Performance design: caching и rate limiting

## 1. Анализ производительности

Система фитнес-трекера содержит несколько hot paths:

- `GET /exercises` часто вызывается клиентом при создании тренировки и заполнении справочников. Данные редко меняются, поэтому endpoint хорошо подходит для кеширования.
- `GET /mongo/workouts` часто используется для ленты тренировок и фильтра по пользователю. Endpoint читает MongoDB и сортирует документы по дате.
- `GET /mongo/stats/users` выполняет aggregation pipeline по коллекции `workouts`, поэтому может стать медленным при росте объема данных.
- `POST /mongo/workouts` меняет данные, от которых зависят списки тренировок и агрегированная статистика.

Медленные операции:

- обращения к PostgreSQL и MongoDB;
- MongoDB aggregation по тренировкам;
- сортировка тренировок по `date`;
- сериализация больших списков тренировок в JSON.

Целевые требования:

- p95 latency для кешируемых GET endpoints: до 50 ms при cache hit;
- p95 latency для чтения из БД: до 300 ms на учебном объеме данных;
- пропускная способность для публичного Mongo endpoint: не более 100 анонимных запросов в минуту на клиента;
- корректный отказ при перегрузке: `429 Too Many Requests`.

## 2. Стратегия кеширования

Используется Cache-Aside (Lazy Loading) поверх in-memory кеша процесса API.

Алгоритм:

1. Handler строит cache key из endpoint и параметров запроса.
2. Если значение найдено и TTL не истек, возвращается готовый JSON и заголовок `X-Cache: HIT`.
3. Если значения нет, handler читает БД, сериализует ответ, кладет JSON в кеш и возвращает `X-Cache: MISS`.
4. При изменении данных handler явно инвалидирует связанные cache keys.

Кешируемые данные:

| Endpoint | Cache key | TTL | Причина |
| --- | --- | --- | --- |
| `GET /exercises` | `exercises:list` | 60 s | справочник часто читается и редко меняется |
| `GET /mongo/workouts` | `mongo:workouts:all`, `mongo:workouts:user:{id}` | 45 s | лента тренировок часто читается, но меняется чаще справочника |
| `GET /mongo/stats/users` | `mongo:stats:users` | 120 s | aggregation дорогая, статистика допускает небольшую задержку |

Инвалидация:

- `POST /exercises` удаляет `exercises:list`;
- `POST /mongo/workouts` удаляет все ключи с префиксом `mongo:workouts:` и ключ `mongo:stats:users`.

Для учебного задания выбран in-memory кеш без Redis, потому что приложение запускается одним API-контейнером. В production при нескольких replicas кеш нужно вынести в Redis, чтобы все инстансы видели одинаковые ключи и инвалидацию.

## 3. Rate limiting

Rate limiting применяется к `GET|POST /mongo/workouts`, потому что endpoint публичный, читает или меняет MongoDB и может быть использован для перегрузки сервиса.

Выбран алгоритм Token Bucket:

- хорошо переносит короткие bursts;
- проще Fixed Window Counter с точки зрения пользовательского опыта;
- дешевле Sliding Window Log по памяти;
- возвращает понятные остатки лимита.

Лимиты:

- анонимный клиент: `100` запросов в минуту;
- клиент с `Authorization`: `1000` запросов в минуту.

Идентификатор клиента:

- если есть `Authorization`, используется значение этого заголовка;
- иначе используется `X-Forwarded-For`;
- если прокси-заголовка нет, используется общий bucket `anonymous`.

Ответы содержат:

- `X-RateLimit-Limit` — размер bucket;
- `X-RateLimit-Remaining` — сколько токенов осталось;
- `X-RateLimit-Reset` — через сколько секунд bucket восстановится;
- `429 Too Many Requests` при отсутствии доступных токенов.

## 4. Влияние на производительность

Кеширование уменьшает:

- число запросов к PostgreSQL для справочника упражнений;
- число запросов к MongoDB для списка тренировок;
- число запусков aggregation pipeline для статистики пользователей;
- CPU-затраты на повторную сериализацию одинаковых ответов.

Rate limiting защищает:

- MongoDB от всплесков чтения и записи;
- API worker threads от очередей запросов;
- пользователей от деградации latency из-за агрессивных клиентов.

Ожидаемые эффекты:

- cache hit latency становится близкой к стоимости поиска в `unordered_map` и отправки готового JSON;
- cache hit rate для `/exercises` должен быть высоким, потому что справочник редко изменяется;
- aggregation endpoint получает наибольший выигрыш при повторных запросах dashboards.

## 5. Метрики мониторинга

Для оценки эффективности нужны следующие метрики:

- latency по endpoint: avg, p95, p99;
- RPS по endpoint и HTTP-статусам;
- cache hit rate: `hits / (hits + misses)`;
- количество cache invalidations;
- размер кеша и число активных keys;
- количество `429 Too Many Requests`;
- загрузка PostgreSQL и MongoDB: CPU, slow queries, connections;
- время выполнения MongoDB aggregation.

Эффективность кеша измеряется так:

```text
hit_rate = cache_hits / (cache_hits + cache_misses)
db_saved_requests = cache_hits
latency_gain = p95_without_cache - p95_with_cache
```

Хороший результат для `/exercises`: hit rate выше 80% после прогрева. Для `/mongo/stats/users` hit rate зависит от dashboard-нагрузки, но даже 30-50% снижает число дорогих aggregation-запросов.
