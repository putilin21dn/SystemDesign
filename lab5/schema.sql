-- Lab3 schema: fitness tracker

CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL PRIMARY KEY,
    login VARCHAR(64) NOT NULL UNIQUE,
    name VARCHAR(128) NOT NULL,
    surname VARCHAR(128) NOT NULL,
    email VARCHAR(255) NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT users_login_not_blank CHECK (length(trim(login)) > 0),
    CONSTRAINT users_name_not_blank CHECK (length(trim(name)) > 0),
    CONSTRAINT users_surname_not_blank CHECK (length(trim(surname)) > 0),
    CONSTRAINT users_email_not_blank CHECK (length(trim(email)) > 0)
);

CREATE TABLE IF NOT EXISTS exercises (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(128) NOT NULL,
    description TEXT NOT NULL DEFAULT '',
    muscle_group VARCHAR(64) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT exercises_name_not_blank CHECK (length(trim(name)) > 0),
    CONSTRAINT exercises_muscle_group_not_blank CHECK (length(trim(muscle_group)) > 0),
    CONSTRAINT exercises_unique_name_group UNIQUE (name, muscle_group)
);

CREATE TABLE IF NOT EXISTS workouts (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    date DATE NOT NULL,
    duration BIGINT NOT NULL DEFAULT 0,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT workouts_duration_non_negative CHECK (duration >= 0)
);

CREATE TABLE IF NOT EXISTS workout_exercises (
    id BIGSERIAL PRIMARY KEY,
    workout_id BIGINT NOT NULL REFERENCES workouts(id) ON DELETE CASCADE,
    exercise_id BIGINT NOT NULL REFERENCES exercises(id) ON DELETE RESTRICT,
    sets BIGINT NOT NULL,
    reps BIGINT NOT NULL,
    weight DOUBLE PRECISION NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT workout_exercises_sets_positive CHECK (sets > 0),
    CONSTRAINT workout_exercises_reps_positive CHECK (reps > 0),
    CONSTRAINT workout_exercises_weight_non_negative CHECK (weight >= 0)
);

CREATE TABLE IF NOT EXISTS auth_sessions (
    token TEXT PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT auth_sessions_valid_lifetime CHECK (expires_at > created_at)
);

-- Indexes for frequent filters/joins
CREATE INDEX IF NOT EXISTS idx_users_name ON users(name);
CREATE INDEX IF NOT EXISTS idx_users_surname ON users(surname);
CREATE INDEX IF NOT EXISTS idx_users_name_surname ON users(name, surname);

CREATE INDEX IF NOT EXISTS idx_exercises_muscle_group ON exercises(muscle_group);

CREATE INDEX IF NOT EXISTS idx_workouts_user_id ON workouts(user_id);
CREATE INDEX IF NOT EXISTS idx_workouts_date ON workouts(date);
CREATE INDEX IF NOT EXISTS idx_workouts_user_id_date ON workouts(user_id, date);

CREATE INDEX IF NOT EXISTS idx_workout_exercises_workout_id ON workout_exercises(workout_id);
CREATE INDEX IF NOT EXISTS idx_workout_exercises_exercise_id ON workout_exercises(exercise_id);
CREATE INDEX IF NOT EXISTS idx_workout_exercises_workout_id_exercise_id ON workout_exercises(workout_id, exercise_id);

CREATE INDEX IF NOT EXISTS idx_auth_sessions_user_id ON auth_sessions(user_id);
CREATE INDEX IF NOT EXISTS idx_auth_sessions_expires_at ON auth_sessions(expires_at);
