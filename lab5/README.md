# Lab5: caching и rate limiting

Проект для варианта «Фитнес-трекер». За основу взят REST API из `lab4`: C++/userver, PostgreSQL для основной модели и MongoDB для документной модели тренировок. В рамках задания добавлены кеширование часто читаемых данных, инвалидация кеша и rate limiting для дорогого endpoint.

## Что реализовано

- In-memory cache с TTL для `GET /exercises`, `GET /mongo/workouts` и `GET /mongo/stats/users`.
- Инвалидация кеша при изменении данных:
  - `POST /exercises` сбрасывает кеш списка упражнений.
  - `POST /mongo/workouts` сбрасывает кеш MongoDB-тренировок и агрегированной статистики.
- Token Bucket rate limiting для `GET|POST /mongo/workouts`.
- HTTP-заголовки:
  - `X-Cache: HIT|MISS`
  - `X-RateLimit-Limit`
  - `X-RateLimit-Remaining`
  - `X-RateLimit-Reset`
- `429 Too Many Requests` при превышении лимита.
- `performance_design.md` с анализом hot paths, TTL, инвалидации, лимитов и метрик мониторинга.

## Быстрый запуск

```bash
cd SystemDesign/lab5
docker compose up --build -d
```

Поднимаются три сервиса:

- `postgres` на `localhost:5432`
- `mongo` на `localhost:27017`
- `api` на `localhost:8080`

Проверка:

```bash
docker compose ps
curl -i http://localhost:8080/exercises
curl -i http://localhost:8080/mongo/workouts
curl -i http://localhost:8080/mongo/stats/users
```

## API для проверки оптимизаций

Кеш списка упражнений:

```bash
curl -i http://localhost:8080/exercises
curl -i http://localhost:8080/exercises
```

Во втором ответе ожидается `X-Cache: HIT`.

Кеш MongoDB-тренировок:

```bash
curl -i http://localhost:8080/mongo/workouts
curl -i "http://localhost:8080/mongo/workouts?user_id=user_001"
```

Кеш агрегированной статистики:

```bash
curl -i http://localhost:8080/mongo/stats/users
curl -i http://localhost:8080/mongo/stats/users
```

Инвалидация MongoDB-кеша:

```bash
curl -i -X POST http://localhost:8080/mongo/workouts \
  -H "Content-Type: application/json" \
  -d '{
    "user_id": "user_001",
    "date": "2026-04-22T18:00:00Z",
    "duration": 55,
    "exercises": [
      {
        "exercise_id": "exercise_001",
        "name": "Barbell Squat",
        "sets": 4,
        "reps": 8,
        "weight": 80
      }
    ],
    "tags": ["strength", "legs"],
    "notes": "Created via lab5 endpoint"
  }'
```

Rate limiting:

```bash
for i in $(seq 1 105); do
  curl -s -o /dev/null -w "%{http_code} %{header_json}\n" \
    http://localhost:8080/mongo/workouts
done
```

Для анонимных запросов лимит `100` запросов в минуту. Для запросов с `Authorization` лимит `1000` запросов в минуту.

## Полная проверка API

```bash
./test_api.sh
```

Скрипт регистрирует пользователя, логинится, создает упражнение и тренировку, проверяет защищенные endpoints.

## MongoDB-скрипты

Повторно выполнить MongoDB-запросы:

```bash
docker compose exec -T mongo mongosh fitness_tracker < queries.js
```

Проверить коллекции:

```bash
docker compose exec -T mongo mongosh fitness_tracker --eval "db.users.countDocuments()"
docker compose exec -T mongo mongosh fitness_tracker --eval "db.exercises.countDocuments()"
docker compose exec -T mongo mongosh fitness_tracker --eval "db.workouts.countDocuments()"
docker compose exec -T mongo mongosh fitness_tracker --eval "db.auth_sessions.countDocuments()"
```

Полный сброс:

```bash
docker compose down -v
docker compose up --build -d
```
