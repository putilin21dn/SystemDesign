# Проектирование документной модели MongoDB

## Предметная область

Система хранит данные фитнес-трекера:

- пользователи регистрируются в системе;
- упражнения имеют название, описание и мышечную группу;
- тренировки принадлежат пользователю и включают выполненные упражнения;
- сессии авторизации связывают токен с пользователем.

## Коллекции

### `users`

Хранит учетные записи пользователей.

```js
{
  _id: "user_001",
  login: "runner_anna",
  name: "Anna",
  surname: "Smirnova",
  email: "anna@mail.test",
  password_hash: "hash_runner_anna",
  profile: {
    gender: "female",
    birth_year: 1997,
    height_cm: 168,
    weight_kg: 61.5
  },
  preferences: {
    goals: ["strength", "endurance"],
    units: "metric"
  },
  created_at: ISODate("2026-01-05T10:00:00Z")
}
```

`profile` и `preferences` встроены в пользователя, потому что эти данные почти всегда читаются вместе с профилем и не являются самостоятельными сущностями.

### `exercises`

Справочник упражнений.

```js
{
  _id: "exercise_001",
  name: "Barbell Squat",
  description: "Basic lower-body strength movement",
  muscle_group: "legs",
  equipment: ["barbell", "rack"],
  difficulty: 4,
  created_at: ISODate("2026-01-01T09:00:00Z")
}
```

Упражнения вынесены в отдельную коллекцию, потому что они переиспользуются в разных тренировках и могут обновляться независимо.

### `workouts`

Хранит тренировки. Связь с пользователем сделана через reference `user_id`. Список выполненных упражнений встроен массивом `exercises`.

```js
{
  _id: "workout_001",
  user_id: "user_001",
  date: ISODate("2026-03-01T07:30:00Z"),
  duration: 62,
  exercises: [
    {
      exercise_id: "exercise_001",
      name: "Barbell Squat",
      sets: 4,
      reps: 8,
      weight: 70
    }
  ],
  tags: ["strength", "legs"],
  notes: "Felt strong",
  created_at: ISODate("2026-03-01T08:40:00Z")
}
```

Выбор:

- `user_id` как reference: пользователь может иметь много тренировок, поэтому встраивание тренировок в `users` привело бы к постоянному росту документа пользователя;
- `exercises` внутри `workouts` как embedded documents: состав конкретной тренировки обычно читается вместе с тренировкой, а исторические значения `sets`, `reps`, `weight` относятся именно к факту выполнения;
- `exercise_id` внутри элемента массива оставлен как reference на справочник упражнений, чтобы можно было выполнять аналитику по упражнениям и обновлять справочник без изменения истории тренировок;
- `name` продублирован в элементе тренировки как денормализованное поле для удобного чтения истории.

### `auth_sessions`

Хранит активные сессии.

```js
{
  _id: "session_001",
  token: "token_001",
  user_id: "user_001",
  expires_at: ISODate("2026-05-01T00:00:00Z"),
  created_at: ISODate("2026-04-01T00:00:00Z")
}
```

Сессии вынесены отдельно, потому что они часто создаются и удаляются, имеют собственный жизненный цикл и не должны раздувать документ пользователя.

## Индексы

- `users.login` и `users.email` — уникальные индексы для регистрации и поиска;
- `users.name, users.surname` — поиск пользователей по ФИО;
- `exercises.name, exercises.muscle_group` — уникальность упражнения в группе;
- `exercises.muscle_group` — фильтрация справочника;
- `workouts.user_id, workouts.date` — список тренировок пользователя по датам;
- `workouts.exercises.exercise_id` — аналитика по упражнению;
- `auth_sessions.token` — поиск сессии по bearer token;
- `auth_sessions.expires_at` — удаление истекших сессий.

## Валидация

Валидация задана в `validation.js` через `$jsonSchema`: обязательные поля, типы, минимальные/максимальные значения, формат email через `pattern`, ограничения для массивов и вложенных объектов.
