# Lab3: Проектирование и оптимизация реляционной БД

Проект для варианта «Фитнес-трекер» (пользователь, тренировка, упражнение) на PostgreSQL с подключенным API из lab2.

## Что включено

- `schema.sql` — схема БД (`CREATE TABLE`, ограничения, индексы)
- `data.sql` — тестовые данные (минимум 10 записей в каждой таблице)
- `queries.sql` — SQL-запросы для всех операций варианта
- `optimization.md` — описание оптимизаций и EXPLAIN
- `Dockerfile`, `docker-compose.yaml` — запуск API и PostgreSQL
- `src/` — API сервис (userver), подключенный к этой БД

## Быстрый запуск

```bash
cd lab3
docker compose up --build -d
```

Проверить:

```bash
docker compose ps
docker compose logs -f postgres
docker compose logs -f api
```

Остановить:

```bash
docker compose down
```

Полный сброс БД и повторная инициализация схемы + данных:

```bash
docker compose down -v
docker compose up --build -d
```

## Проверка SQL файлов в БД

```bash
cd lab3
docker compose exec -T postgres psql -U postgres -d fitness_tracker -c "\dt"
docker compose exec -T postgres psql -U postgres -d fitness_tracker -c "SELECT COUNT(*) FROM users;"
docker compose exec -T postgres psql -U postgres -d fitness_tracker -c "SELECT COUNT(*) FROM exercises;"
docker compose exec -T postgres psql -U postgres -d fitness_tracker -c "SELECT COUNT(*) FROM workouts;"
docker compose exec -T postgres psql -U postgres -d fitness_tracker -c "SELECT COUNT(*) FROM workout_exercises;"
```

## Примеры использования API

Создание пользователя:

```bash
curl -i -X POST http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"login":"user_new","name":"Ivan","surname":"Petrov","email":"user_new@mail.test"}'
```

Поиск пользователя по логину:

```bash
curl -i http://localhost:8080/users/user_new
```

Поиск по маске имени/фамилии:

```bash
curl -i "http://localhost:8080/users/search?name=Iv&surname=Pet"
```

Получение списка упражнений:

```bash
curl -i http://localhost:8080/exercises
```

## EXPLAIN 

Команды для анализа планов есть в `optimization.md`.

Пример запуска EXPLAIN вручную:

```bash
cd lab3
docker compose exec -T postgres psql -U postgres -d fitness_tracker
```

Далее вставить SQL-команды EXPLAIN из `optimization.md`.
