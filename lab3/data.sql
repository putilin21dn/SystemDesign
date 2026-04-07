-- Lab3 test data (at least 10 records per table)

INSERT INTO users (id, login, name, surname, email, password_hash, created_at) VALUES
(1, 'ivan01', 'Ivan', 'Petrov', 'ivan01@example.com', 'hash_ivan01', NOW() - INTERVAL '20 days'),
(2, 'olga02', 'Olga', 'Smirnova', 'olga02@example.com', 'hash_olga02', NOW() - INTERVAL '19 days'),
(3, 'max03', 'Maxim', 'Ivanov', 'max03@example.com', 'hash_max03', NOW() - INTERVAL '18 days'),
(4, 'anna04', 'Anna', 'Kuznetsova', 'anna04@example.com', 'hash_anna04', NOW() - INTERVAL '17 days'),
(5, 'nina05', 'Nina', 'Volkova', 'nina05@example.com', 'hash_nina05', NOW() - INTERVAL '16 days'),
(6, 'igor06', 'Igor', 'Sokolov', 'igor06@example.com', 'hash_igor06', NOW() - INTERVAL '15 days'),
(7, 'maria07', 'Maria', 'Lebedeva', 'maria07@example.com', 'hash_maria07', NOW() - INTERVAL '14 days'),
(8, 'pavel08', 'Pavel', 'Morozov', 'pavel08@example.com', 'hash_pavel08', NOW() - INTERVAL '13 days'),
(9, 'daria09', 'Daria', 'Novikova', 'daria09@example.com', 'hash_daria09', NOW() - INTERVAL '12 days'),
(10, 'alex10', 'Alexey', 'Mikhailov', 'alex10@example.com', 'hash_alex10', NOW() - INTERVAL '11 days')
ON CONFLICT (id) DO NOTHING;

INSERT INTO exercises (id, name, description, muscle_group, created_at) VALUES
(1, 'Bench Press', 'Chest press with barbell', 'chest', NOW() - INTERVAL '10 days'),
(2, 'Squat', 'Back squat', 'legs', NOW() - INTERVAL '10 days'),
(3, 'Deadlift', 'Conventional deadlift', 'back', NOW() - INTERVAL '10 days'),
(4, 'Overhead Press', 'Shoulder press', 'shoulders', NOW() - INTERVAL '9 days'),
(5, 'Pull Up', 'Bodyweight pull up', 'back', NOW() - INTERVAL '9 days'),
(6, 'Barbell Row', 'Bent-over row', 'back', NOW() - INTERVAL '8 days'),
(7, 'Biceps Curl', 'Dumbbell curl', 'arms', NOW() - INTERVAL '8 days'),
(8, 'Triceps Pushdown', 'Cable pushdown', 'arms', NOW() - INTERVAL '7 days'),
(9, 'Lunges', 'Walking lunges', 'legs', NOW() - INTERVAL '7 days'),
(10, 'Plank', 'Core static hold', 'core', NOW() - INTERVAL '6 days')
ON CONFLICT (id) DO NOTHING;

INSERT INTO workouts (id, user_id, date, duration, created_at) VALUES
(1, 1, CURRENT_DATE - 10, 60, NOW() - INTERVAL '10 days'),
(2, 2, CURRENT_DATE - 9, 55, NOW() - INTERVAL '9 days'),
(3, 3, CURRENT_DATE - 8, 70, NOW() - INTERVAL '8 days'),
(4, 4, CURRENT_DATE - 7, 50, NOW() - INTERVAL '7 days'),
(5, 5, CURRENT_DATE - 6, 65, NOW() - INTERVAL '6 days'),
(6, 6, CURRENT_DATE - 5, 45, NOW() - INTERVAL '5 days'),
(7, 7, CURRENT_DATE - 4, 75, NOW() - INTERVAL '4 days'),
(8, 8, CURRENT_DATE - 3, 60, NOW() - INTERVAL '3 days'),
(9, 9, CURRENT_DATE - 2, 52, NOW() - INTERVAL '2 days'),
(10, 10, CURRENT_DATE - 1, 68, NOW() - INTERVAL '1 day')
ON CONFLICT (id) DO NOTHING;

INSERT INTO workout_exercises (id, workout_id, exercise_id, sets, reps, weight, created_at) VALUES
(1, 1, 1, 4, 8, 80, NOW() - INTERVAL '10 days'),
(2, 2, 2, 5, 6, 100, NOW() - INTERVAL '9 days'),
(3, 3, 3, 4, 5, 120, NOW() - INTERVAL '8 days'),
(4, 4, 4, 4, 8, 45, NOW() - INTERVAL '7 days'),
(5, 5, 5, 4, 10, 0, NOW() - INTERVAL '6 days'),
(6, 6, 6, 4, 8, 70, NOW() - INTERVAL '5 days'),
(7, 7, 7, 3, 12, 16, NOW() - INTERVAL '4 days'),
(8, 8, 8, 3, 12, 25, NOW() - INTERVAL '3 days'),
(9, 9, 9, 4, 10, 22, NOW() - INTERVAL '2 days'),
(10, 10, 10, 3, 60, 0, NOW() - INTERVAL '1 day')
ON CONFLICT (id) DO NOTHING;

INSERT INTO auth_sessions (token, user_id, expires_at, created_at) VALUES
('session_token_1', 1, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_2', 2, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_3', 3, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_4', 4, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_5', 5, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_6', 6, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_7', 7, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_8', 8, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_9', 9, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour'),
('session_token_10', 10, NOW() + INTERVAL '1 day', NOW() - INTERVAL '1 hour')
ON CONFLICT (token) DO NOTHING;

SELECT setval('users_id_seq', COALESCE((SELECT MAX(id) FROM users), 1), true);
SELECT setval('exercises_id_seq', COALESCE((SELECT MAX(id) FROM exercises), 1), true);
SELECT setval('workouts_id_seq', COALESCE((SELECT MAX(id) FROM workouts), 1), true);
SELECT setval('workout_exercises_id_seq', COALESCE((SELECT MAX(id) FROM workout_exercises), 1), true);
