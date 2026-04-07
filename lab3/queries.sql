-- Lab3: SQL queries for all operations

-- 1) Create user
INSERT INTO users (login, name, surname, email, password_hash)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, login, name, surname, email, created_at;

-- 2) Get user by login
SELECT id, login, name, surname, email, created_at
FROM users
WHERE login = $1;

-- 3) Search user by name/surname mask
SELECT id, login, name, surname, email, created_at
FROM users
WHERE name ILIKE $1 AND surname ILIKE $2
ORDER BY id;

-- 4) Create exercise
INSERT INTO exercises (name, description, muscle_group)
VALUES ($1, $2, $3)
RETURNING id, name, description, muscle_group, created_at;

-- 5) Get exercises list
SELECT id, name, description, muscle_group, created_at
FROM exercises
ORDER BY id;

-- 6) Create workout
INSERT INTO workouts (user_id, date, duration)
VALUES ($1, $2::date, COALESCE($3, 0))
RETURNING id, user_id, date, duration, created_at;

-- 7) Add exercise to workout
INSERT INTO workout_exercises (workout_id, exercise_id, sets, reps, weight)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, workout_id, exercise_id, sets, reps, weight, created_at;

-- 8) User workouts history
SELECT id, user_id, date, duration, created_at
FROM workouts
WHERE user_id = $1
ORDER BY date DESC, id DESC;

-- 9) Workout statistics for period
SELECT
  COUNT(w.id)::bigint AS total_workouts,
  COALESCE(SUM(we.weight * we.sets * we.reps), 0)::double precision AS total_weight,
  COALESCE(SUM(w.duration), 0)::bigint AS total_duration
FROM workouts w
LEFT JOIN workout_exercises we ON w.id = we.workout_id
WHERE w.user_id = $1
  AND w.date BETWEEN $2::date AND $3::date;

-- 10) Auth register
INSERT INTO users (login, name, surname, email, password_hash)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, login, name, surname, email, created_at;

-- 11) Auth login (credentials check)
SELECT id, password_hash
FROM users
WHERE login = $1;

-- 12) Create session
INSERT INTO auth_sessions (token, user_id, expires_at)
VALUES ($1, $2, NOW() + INTERVAL '24 hours')
RETURNING token, user_id, expires_at;

-- 13) Validate session token
SELECT user_id
FROM auth_sessions
WHERE token = $1 AND expires_at > NOW();
