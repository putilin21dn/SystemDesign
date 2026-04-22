# Оптимизация запросов

## Частые запросы
1. Поиск пользователя по логину.
2. Поиск пользователей по маске имени и фамилии.
3. История тренировок пользователя за период.
4. Статистика пользователя за период.
5. JOIN `workouts` + `workout_exercises` для аналитики.

## Добавленные индексы
- `users(login)` и `users(email)` через `UNIQUE` constraints (создаются автоматически).
- `idx_users_name`, `idx_users_surname`, `idx_users_name_surname` для фильтрации по имени/фамилии.
- `idx_exercises_muscle_group` для фильтрации упражнений по группе мышц.
- `idx_workouts_user_id`, `idx_workouts_date`, `idx_workouts_user_id_date` для истории и выборок по периоду.
- `idx_workout_exercises_workout_id`, `idx_workout_exercises_exercise_id`, `idx_workout_exercises_workout_id_exercise_id` для JOIN и фильтров по FK.
- `idx_auth_sessions_user_id`, `idx_auth_sessions_expires_at` для проверки и очистки сессий.

## Методика сравнения планов

- `До оптимизации`: индексные планы отключены на уровне планировщика:
  - `SET enable_indexscan = off;`
  - `SET enable_bitmapscan = off;`
  - `SET enable_indexonlyscan = off;`
- `После оптимизации`: настройки возвращены в дефолт (`RESET ...`).

Такой подход позволяет честно сравнить один и тот же запрос в режиме без индексных сканов и с их использованием.

## EXPLAIN: поиск по логину
Запрос:
```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, login, name, surname
FROM users
WHERE login = 'testuser5';
```

Факт:
- `Index Scan using users_login_key on users`
- `Execution Time: 0.134 ms`

Вывод: точечный поиск по логину использует уникальный индекс и выполняется через `Index Scan`.

## EXPLAIN: маска по имени и фамилии
Запрос:
```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, login, name, surname
FROM users
WHERE name ILIKE '%iv%' AND surname ILIKE '%pet%';
```

Факт:
- `Seq Scan on users`
- `Execution Time: 0.609 ms`

Вывод: для шаблона с ведущим `%` обычные B-Tree индексы, как правило, не используются. Для ускорения нужен `pg_trgm` + GIN/GiST.

## EXPLAIN: история тренировок пользователя за период
Запрос:
```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, user_id, date, duration
FROM workouts
WHERE user_id = 5
  AND date BETWEEN '2026-01-01'::date AND '2026-12-31'::date
ORDER BY date DESC, id DESC;
```

До (индексы отключены в планировщике):
- `Seq Scan on workouts`
- `Sort`
- `Execution Time: 4.468 ms`

После (дефолтный планировщик):
- `Index Scan Backward using idx_workouts_user_id_date on workouts`
- `Execution Time: 0.610 ms`

Вывод: составной индекс `(user_id, date)` напрямую используется и заметно уменьшает время выполнения.

## EXPLAIN: статистика пользователя (JOIN)
Запрос:
```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT
  COUNT(w.id)::bigint AS total_workouts,
  COALESCE(SUM(we.weight * we.sets * we.reps), 0)::double precision AS total_weight
FROM workouts w
LEFT JOIN workout_exercises we ON w.id = we.workout_id
WHERE w.user_id = 5
  AND w.date BETWEEN '2026-01-01'::date AND '2026-12-31'::date;
```

До (индексы отключены в планировщике):
- `Hash Right Join`
- `Seq Scan on workouts`
- `Seq Scan on workout_exercises`
- `Execution Time: 1.545 ms`

После (дефолтный планировщик):
- `Nested Loop Left Join`
- `Index Scan using idx_workouts_user_id_date on workouts`
- `Bitmap Index Scan on idx_workout_exercises_workout_id_exercise_id`
- `Execution Time: 0.582 ms`

Вывод: индексы по `workouts(user_id, date)` и `workout_exercises(workout_id, exercise_id)` уменьшают стоимость JOIN и ускоряют агрегирование.

## Итог
- Для операций истории/статистики индексы реально применяются (подтверждено `EXPLAIN ANALYZE`).
- Самый сильный эффект даёт составной индекс `idx_workouts_user_id_date`.
- Для `%mask%` поиска по строкам рекомендуется триграммная индексация (`pg_trgm`), если это частый сценарий.
