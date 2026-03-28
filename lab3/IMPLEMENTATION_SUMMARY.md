# Yandex Userver REST API Сервис - Фитнес Трекер

## Обзор

Реализован REST API сервис на **Yandex userver** для управления пользователями, упражнениями и тренировками.

## Реализованный функционал

### API Endpoints

1. `POST /auth/register` - регистрация пользователя
2. `POST /auth/login` - логин и получение Bearer токена
3. `POST /users` - создание пользователя (без auth)
4. `GET /users/{login}` - получение пользователя по логину
5. `GET /users/search?name=...` - поиск пользователей по имени
6. `GET /exercises` - список упражнений
7. `POST /exercises` - создание упражнения (auth)
8. `POST /workouts` - создание тренировки (auth)
9. `POST /workouts/{workout_id}/exercises` - добавление упражнения в тренировку (auth)
10. `GET /users/{id}/workouts` - история тренировок пользователя (auth)
11. `GET /users/{id}/stats?from=...&to=...` - статистика за период (auth)

### Аутентификация

- Session-based authentication через таблицу `auth_sessions`
- Формат: `Authorization: Bearer <token>`
- Защищены минимум 2 endpoint (фактически 5)
- Добавлен middleware для проверки токена (`src/middleware/auth_middleware.*`)

### Компоненты проекта

#### Модели данных (`src/models/`)
- `user.hpp`
- `exercise.hpp`
- `workout.hpp`
- `workout_exercise.hpp`

#### HTTP handlers (`src/handlers/`)
- `auth_register_handler.*`
- `auth_login_handler.*`
- `users_handler.*`
- `user_by_login_handler.*`
- `user_search_handler.*`
- `exercises_handler.*` (GET)
- `exercises_create_handler.*` (POST)
- `workouts_handler.*`
- `workout_exercises_handler.*`
- `user_workouts_handler.*`
- `user_stats_handler.*`

#### Бизнес-логика (`src/services/`)
- `auth_service.*`
- `user_service.*`
- `exercise_service.*`
- `workout_service.*`

#### Middleware / Utils
- `src/middleware/auth_middleware.*`
- `src/utils/http_response.hpp`

### База данных (PostgreSQL)

Схема в `schema.sql` включает:
- `users`
- `exercises`
- `workouts`
- `workout_exercises`
- `auth_sessions`

Все SQL запросы параметризованы.

## Документация API

- OpenAPI спецификация: `docs/openapi.yaml`
- Swagger UI можно открыть через:
  - `https://editor.swagger.io/` (вставить `openapi.yaml`)
  - или локально через контейнер `swaggerapi/swagger-ui`

## Тестирование

### Автотест

```bash
cd SystemDesign/api
./test_api.sh
```

Скрипт проверяет:
- успешные сценарии (register/login/create/list/stats)
- ошибки (например, `401` без токена)

### Запуск проекта

```bash
cd SystemDesign/api
docker compose up --build -d
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

## Технологии

| Компонент | Технология |
|---|---|
| Фреймворк | Yandex userver (C++17) |
| БД | PostgreSQL 15 |
| Сборка | CMake |
| JSON | userver::formats::json |
| Логирование | spdlog |
| Контейнеризация | Docker / Docker Compose |

## Итог

- Требования по REST API выполнены
- Требования по auth выполнены
- OpenAPI спецификация добавлена
- Базовые тесты (успех + ошибки) добавлены

**Статус:** Готово к использованию  
**Последнее обновление:** 28 марта 2026
