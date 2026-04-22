# Lab4: MongoDB 

Проект для варианта «Фитнес-трекер». За основу взята папка `lab3`: прежняя PostgreSQL-схема, SQL-данные и C++ API на userver сохранены. Для `lab4` дополнительно добавлена MongoDB-документная модель, MongoDB-скрипты и MongoDB endpoints в том же C++ API.

## Что включено

- `schema.sql`, `data.sql`, `queries.sql`, `optimization.md` — PostgreSQL-часть из `lab3`
- `schema_design.md` — проектирование MongoDB-документной модели
- `validation.js` — `$jsonSchema`-валидация MongoDB и индексы
- `data.js` — тестовые MongoDB-данные
- `queries.js` — MongoDB CRUD-запросы и aggregation pipeline
- `src/` — C++ userver API с подключением к PostgreSQL и MongoDB
- `Dockerfile`, `docker-compose.yaml` — запуск API, PostgreSQL и MongoDB

## Быстрый запуск

```bash
cd SystemDesign/lab4
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

## PostgreSQL API из lab3

Эти endpoints остались на PostgreSQL:

```bash
curl -i -X POST http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"login":"user_new","name":"Ivan","surname":"Petrov","email":"user_new@mail.test"}'

curl -i http://localhost:8080/users/user_new
curl -i "http://localhost:8080/users/search?name=Iv&surname=Pet"
curl -i http://localhost:8080/exercises
```

## MongoDB API

Получить тренировки из MongoDB:

```bash
curl -i http://localhost:8080/mongo/workouts
curl -i "http://localhost:8080/mongo/workouts?user_id=user_001"
```

Создать тренировку в MongoDB:

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
    "notes": "Created via C++ userver MongoDB endpoint"
  }'
```

Aggregation pipeline через API:

```bash
curl -i http://localhost:8080/mongo/stats/users
```

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
