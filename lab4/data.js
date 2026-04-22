use("fitness_tracker");

db.users.deleteMany({});
db.exercises.deleteMany({});
db.workouts.deleteMany({});
db.auth_sessions.deleteMany({});

db.users.insertMany([
  {
    _id: "user_001",
    login: "runner_anna",
    name: "Anna",
    surname: "Smirnova",
    email: "anna@mail.test",
    password_hash: "hash_runner_anna",
    profile: { gender: "female", birth_year: 1997, height_cm: 168, weight_kg: 61.5 },
    preferences: { goals: ["strength", "endurance"], units: "metric" },
    created_at: ISODate("2026-01-05T10:00:00Z")
  },
  {
    _id: "user_002",
    login: "max_power",
    name: "Maxim",
    surname: "Ivanov",
    email: "max@mail.test",
    password_hash: "hash_max_power",
    profile: { gender: "male", birth_year: 1995, height_cm: 181, weight_kg: 84.2 },
    preferences: { goals: ["hypertrophy"], units: "metric" },
    created_at: ISODate("2026-01-06T10:00:00Z")
  },
  {
    _id: "user_003",
    login: "maria_fit",
    name: "Maria",
    surname: "Petrova",
    email: "maria@mail.test",
    password_hash: "hash_maria_fit",
    profile: { gender: "female", birth_year: 2001, height_cm: 164, weight_kg: 57.8 },
    preferences: { goals: ["mobility", "endurance"], units: "metric" },
    created_at: ISODate("2026-01-07T10:00:00Z")
  },
  {
    _id: "user_004",
    login: "oleg_run",
    name: "Oleg",
    surname: "Sokolov",
    email: "oleg@mail.test",
    password_hash: "hash_oleg_run",
    profile: { gender: "male", birth_year: 1992, height_cm: 176, weight_kg: 73.4 },
    preferences: { goals: ["endurance"], units: "metric" },
    created_at: ISODate("2026-01-08T10:00:00Z")
  },
  {
    _id: "user_005",
    login: "kate_lift",
    name: "Ekaterina",
    surname: "Volkova",
    email: "kate@mail.test",
    password_hash: "hash_kate_lift",
    profile: { gender: "female", birth_year: 1999, height_cm: 170, weight_kg: 66.1 },
    preferences: { goals: ["strength"], units: "metric" },
    created_at: ISODate("2026-01-09T10:00:00Z")
  },
  {
    _id: "user_006",
    login: "dima_box",
    name: "Dmitry",
    surname: "Kuznetsov",
    email: "dima@mail.test",
    password_hash: "hash_dima_box",
    profile: { gender: "male", birth_year: 1998, height_cm: 179, weight_kg: 78.9 },
    preferences: { goals: ["conditioning"], units: "metric" },
    created_at: ISODate("2026-01-10T10:00:00Z")
  },
  {
    _id: "user_007",
    login: "irina_yoga",
    name: "Irina",
    surname: "Morozova",
    email: "irina@mail.test",
    password_hash: "hash_irina_yoga",
    profile: { gender: "female", birth_year: 1990, height_cm: 166, weight_kg: 59.3 },
    preferences: { goals: ["mobility"], units: "metric" },
    created_at: ISODate("2026-01-11T10:00:00Z")
  },
  {
    _id: "user_008",
    login: "sergey_tri",
    name: "Sergey",
    surname: "Lebedev",
    email: "sergey@mail.test",
    password_hash: "hash_sergey_tri",
    profile: { gender: "male", birth_year: 1988, height_cm: 183, weight_kg: 80.5 },
    preferences: { goals: ["endurance", "strength"], units: "metric" },
    created_at: ISODate("2026-01-12T10:00:00Z")
  },
  {
    _id: "user_009",
    login: "alisa_core",
    name: "Alisa",
    surname: "Fedorova",
    email: "alisa@mail.test",
    password_hash: "hash_alisa_core",
    profile: { gender: "female", birth_year: 2000, height_cm: 162, weight_kg: 54.4 },
    preferences: { goals: ["core", "mobility"], units: "metric" },
    created_at: ISODate("2026-01-13T10:00:00Z")
  },
  {
    _id: "user_010",
    login: "nikita_swim",
    name: "Nikita",
    surname: "Orlov",
    email: "nikita@mail.test",
    password_hash: "hash_nikita_swim",
    profile: { gender: "male", birth_year: 1996, height_cm: 185, weight_kg: 86.7 },
    preferences: { goals: ["endurance", "recovery"], units: "metric" },
    created_at: ISODate("2026-01-14T10:00:00Z")
  }
]);

db.exercises.insertMany([
  { _id: "exercise_001", name: "Barbell Squat", description: "Basic lower-body strength movement", muscle_group: "legs", equipment: ["barbell", "rack"], difficulty: 4, created_at: ISODate("2026-01-01T09:00:00Z") },
  { _id: "exercise_002", name: "Bench Press", description: "Horizontal press for chest and triceps", muscle_group: "chest", equipment: ["barbell", "bench"], difficulty: 4, created_at: ISODate("2026-01-01T09:05:00Z") },
  { _id: "exercise_003", name: "Deadlift", description: "Hip hinge full-body strength movement", muscle_group: "back", equipment: ["barbell"], difficulty: 5, created_at: ISODate("2026-01-01T09:10:00Z") },
  { _id: "exercise_004", name: "Pull Up", description: "Vertical pulling movement", muscle_group: "back", equipment: ["pull-up bar"], difficulty: 4, created_at: ISODate("2026-01-01T09:15:00Z") },
  { _id: "exercise_005", name: "Overhead Press", description: "Vertical press for shoulders", muscle_group: "shoulders", equipment: ["barbell"], difficulty: 4, created_at: ISODate("2026-01-01T09:20:00Z") },
  { _id: "exercise_006", name: "Plank", description: "Static core exercise", muscle_group: "core", equipment: [], difficulty: 2, created_at: ISODate("2026-01-01T09:25:00Z") },
  { _id: "exercise_007", name: "Lunge", description: "Single-leg lower-body exercise", muscle_group: "legs", equipment: ["dumbbell"], difficulty: 3, created_at: ISODate("2026-01-01T09:30:00Z") },
  { _id: "exercise_008", name: "Row", description: "Horizontal pulling movement", muscle_group: "back", equipment: ["cable"], difficulty: 3, created_at: ISODate("2026-01-01T09:35:00Z") },
  { _id: "exercise_009", name: "Burpee", description: "Conditioning full-body movement", muscle_group: "full_body", equipment: [], difficulty: 3, created_at: ISODate("2026-01-01T09:40:00Z") },
  { _id: "exercise_010", name: "Romanian Deadlift", description: "Posterior-chain hinge movement", muscle_group: "legs", equipment: ["barbell"], difficulty: 4, created_at: ISODate("2026-01-01T09:45:00Z") }
]);

db.workouts.insertMany([
  { _id: "workout_001", user_id: "user_001", date: ISODate("2026-03-01T07:30:00Z"), duration: 62, exercises: [{ exercise_id: "exercise_001", name: "Barbell Squat", sets: 4, reps: 8, weight: 70 }, { exercise_id: "exercise_006", name: "Plank", sets: 3, reps: 60, weight: 0 }], tags: ["strength", "legs"], notes: "Felt strong", created_at: ISODate("2026-03-01T08:40:00Z") },
  { _id: "workout_002", user_id: "user_002", date: ISODate("2026-03-02T18:00:00Z"), duration: 75, exercises: [{ exercise_id: "exercise_002", name: "Bench Press", sets: 5, reps: 5, weight: 95 }, { exercise_id: "exercise_005", name: "Overhead Press", sets: 3, reps: 8, weight: 50 }], tags: ["strength", "push"], notes: "Heavy day", created_at: ISODate("2026-03-02T19:20:00Z") },
  { _id: "workout_003", user_id: "user_003", date: ISODate("2026-03-03T08:00:00Z"), duration: 45, exercises: [{ exercise_id: "exercise_007", name: "Lunge", sets: 3, reps: 12, weight: 16 }, { exercise_id: "exercise_006", name: "Plank", sets: 4, reps: 45, weight: 0 }], tags: ["mobility", "core"], notes: "Easy pace", created_at: ISODate("2026-03-03T08:55:00Z") },
  { _id: "workout_004", user_id: "user_004", date: ISODate("2026-03-04T06:30:00Z"), duration: 38, exercises: [{ exercise_id: "exercise_009", name: "Burpee", sets: 5, reps: 15, weight: 0 }], tags: ["conditioning"], notes: "Morning conditioning", created_at: ISODate("2026-03-04T07:10:00Z") },
  { _id: "workout_005", user_id: "user_005", date: ISODate("2026-03-05T17:40:00Z"), duration: 70, exercises: [{ exercise_id: "exercise_003", name: "Deadlift", sets: 5, reps: 3, weight: 120 }, { exercise_id: "exercise_008", name: "Row", sets: 4, reps: 10, weight: 55 }], tags: ["strength", "pull"], notes: "Good technique", created_at: ISODate("2026-03-05T19:00:00Z") },
  { _id: "workout_006", user_id: "user_006", date: ISODate("2026-03-06T19:10:00Z"), duration: 52, exercises: [{ exercise_id: "exercise_009", name: "Burpee", sets: 6, reps: 10, weight: 0 }, { exercise_id: "exercise_004", name: "Pull Up", sets: 4, reps: 6, weight: 0 }], tags: ["conditioning", "back"], notes: "High intensity", created_at: ISODate("2026-03-06T20:10:00Z") },
  { _id: "workout_007", user_id: "user_007", date: ISODate("2026-03-07T09:30:00Z"), duration: 40, exercises: [{ exercise_id: "exercise_006", name: "Plank", sets: 3, reps: 75, weight: 0 }, { exercise_id: "exercise_007", name: "Lunge", sets: 3, reps: 10, weight: 10 }], tags: ["mobility", "core"], notes: "Recovery session", created_at: ISODate("2026-03-07T10:15:00Z") },
  { _id: "workout_008", user_id: "user_008", date: ISODate("2026-03-08T11:00:00Z"), duration: 80, exercises: [{ exercise_id: "exercise_001", name: "Barbell Squat", sets: 5, reps: 5, weight: 100 }, { exercise_id: "exercise_003", name: "Deadlift", sets: 3, reps: 5, weight: 130 }], tags: ["strength"], notes: "Competition prep", created_at: ISODate("2026-03-08T12:25:00Z") },
  { _id: "workout_009", user_id: "user_009", date: ISODate("2026-03-09T18:20:00Z"), duration: 35, exercises: [{ exercise_id: "exercise_006", name: "Plank", sets: 5, reps: 50, weight: 0 }, { exercise_id: "exercise_009", name: "Burpee", sets: 4, reps: 12, weight: 0 }], tags: ["core", "conditioning"], notes: "Short workout", created_at: ISODate("2026-03-09T19:00:00Z") },
  { _id: "workout_010", user_id: "user_010", date: ISODate("2026-03-10T07:00:00Z"), duration: 58, exercises: [{ exercise_id: "exercise_010", name: "Romanian Deadlift", sets: 4, reps: 8, weight: 90 }, { exercise_id: "exercise_008", name: "Row", sets: 4, reps: 12, weight: 45 }], tags: ["pull", "legs"], notes: "Moderate load", created_at: ISODate("2026-03-10T08:05:00Z") }
]);

db.auth_sessions.insertMany([
  { _id: "session_001", token: "token_001", user_id: "user_001", expires_at: ISODate("2026-05-01T00:00:00Z"), created_at: ISODate("2026-04-01T00:00:00Z") },
  { _id: "session_002", token: "token_002", user_id: "user_002", expires_at: ISODate("2026-05-02T00:00:00Z"), created_at: ISODate("2026-04-02T00:00:00Z") },
  { _id: "session_003", token: "token_003", user_id: "user_003", expires_at: ISODate("2026-05-03T00:00:00Z"), created_at: ISODate("2026-04-03T00:00:00Z") },
  { _id: "session_004", token: "token_004", user_id: "user_004", expires_at: ISODate("2026-05-04T00:00:00Z"), created_at: ISODate("2026-04-04T00:00:00Z") },
  { _id: "session_005", token: "token_005", user_id: "user_005", expires_at: ISODate("2026-05-05T00:00:00Z"), created_at: ISODate("2026-04-05T00:00:00Z") },
  { _id: "session_006", token: "token_006", user_id: "user_006", expires_at: ISODate("2026-05-06T00:00:00Z"), created_at: ISODate("2026-04-06T00:00:00Z") },
  { _id: "session_007", token: "token_007", user_id: "user_007", expires_at: ISODate("2026-05-07T00:00:00Z"), created_at: ISODate("2026-04-07T00:00:00Z") },
  { _id: "session_008", token: "token_008", user_id: "user_008", expires_at: ISODate("2026-05-08T00:00:00Z"), created_at: ISODate("2026-04-08T00:00:00Z") },
  { _id: "session_009", token: "token_009", user_id: "user_009", expires_at: ISODate("2026-05-09T00:00:00Z"), created_at: ISODate("2026-04-09T00:00:00Z") },
  { _id: "session_010", token: "token_010", user_id: "user_010", expires_at: ISODate("2026-05-10T00:00:00Z"), created_at: ISODate("2026-04-10T00:00:00Z") }
]);

printjson({
  users: db.users.countDocuments(),
  exercises: db.exercises.countDocuments(),
  workouts: db.workouts.countDocuments(),
  auth_sessions: db.auth_sessions.countDocuments()
});
