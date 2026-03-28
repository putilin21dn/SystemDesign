# Lab2: Fitness Tracker API 

REST API сервис на Yandex userver для фитнес-трекера.

## Функционал

Сущности:
- Пользователь
- Упражнение
- Тренировка

Реализованные операции:
- Создание пользователя
- Поиск пользователя по логину
- Поиск пользователя по маске имени и фамилии
- Создание упражнения
- Получение списка упражнений
- Создание тренировки
- Добавление упражнения в тренировку
- Получение истории тренировок пользователя
- Получение статистики тренировок за период

Дополнительно:
- Регистрация и логин: `/auth/register`, `/auth/login`
- Защищенные endpoint через `Authorization: Bearer <token>`
- OpenAPI спецификация: `docs/openapi.yaml`
- Smoke-тесты: `test_api.sh`

## Структура

```text
lab2/
├── CMakeLists.txt
├── Dockerfile
├── docker-compose.yaml
├── schema.sql
├── src/
├── configs/
├── docs/
│   └── openapi.yaml
└── test_api.sh
```

## Запуск

```bash
cd lab2
docker compose up --build -d
```

Проверка:

```bash
docker compose ps
docker compose logs -f api
```

Остановка:

```bash
docker compose down
```

Полный сброс БД:

```bash
docker compose down -v
docker compose up --build -d
```

API:
- `http://localhost:8080`

## Swagger / OpenAPI

Вариант 1:
- открыть `docs/openapi.yaml` в https://editor.swagger.io/

Вариант 2 (локальный Swagger UI):

```bash
cd /Users/dmitry/Desktop/project/SystemDesign/lab2
docker run --rm -p 8081:8080 \
  -e SWAGGER_JSON=/openapi.yaml \
  -v "$PWD/docs/openapi.yaml:/openapi.yaml" \
  swaggerapi/swagger-ui
```

Открыть:
- `http://localhost:8081`

## Автотест

```bash
cd /Users/dmitry/Desktop/project/SystemDesign/lab2
./test_api.sh
```

## Примеры API

### 1. Регистрация

```bash
curl -i -X POST http://localhost:8080/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "login":"demo_user",
    "name":"Ivan",
    "surname":"Petrov",
    "email":"demo_user@mail.test",
    "password":"secret123"
  }'
```

### 2. Логин

```bash
curl -s -X POST http://localhost:8080/auth/login \
  -H "Content-Type: application/json" \
  -d '{"login":"demo_user","password":"secret123"}'
```

Сохранить токен:

```bash
TOKEN="<вставь token из ответа>"
```

### 3. Создать упражнение (auth)

```bash
curl -i -X POST http://localhost:8080/exercises \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "name":"Bench Press",
    "description":"Chest exercise",
    "muscle_group":"chest"
  }'
```

### 4. Получить список упражнений

```bash
curl -i http://localhost:8080/exercises
```

### 5. Поиск пользователя по логину

```bash
curl -i http://localhost:8080/users/demo_user
```

### 6. Поиск по маске имени и фамилии

```bash
curl -i "http://localhost:8080/users/search?name=Iv&surname=Pet"
```

### 7. Создать тренировку (auth)

```bash
curl -i -X POST http://localhost:8080/workouts \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"user_id":1,"date":"2026-03-28"}'
```

### 8. Добавить упражнение в тренировку (auth)

```bash
curl -i -X POST http://localhost:8080/workouts/1/exercises \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"exercise_id":1,"sets":4,"reps":10,"weight":80}'
```

### 9. История тренировок пользователя (auth)

```bash
curl -i http://localhost:8080/users/1/workouts \
  -H "Authorization: Bearer $TOKEN"
```

### 10. Статистика за период (auth)

```bash
curl -i "http://localhost:8080/users/1/stats?from=2026-01-01&to=2026-12-31" \
  -H "Authorization: Bearer $TOKEN"
```

## Негативные проверки

Без токена (ожидается `401`):

```bash
curl -i -X POST http://localhost:8080/workouts \
  -H "Content-Type: application/json" \
  -d '{"user_id":1,"date":"2026-03-28"}'
```

Пустые параметры поиска (ожидается `400`):

```bash
curl -i "http://localhost:8080/users/search"
```
