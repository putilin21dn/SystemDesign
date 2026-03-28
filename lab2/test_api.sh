#!/usr/bin/env bash

set -euo pipefail

BASE_URL="${BASE_URL:-http://localhost:8080}"
SUFFIX="$(date +%s)"
LOGIN="user_${SUFFIX}"
EMAIL="${LOGIN}@mail.test"
PASSWORD="secret123"

print_step() {
  printf '\n=== %s ===\n' "$1"
}

pretty() {
  python3 -m json.tool 2>/dev/null || cat
}

http_code() {
  local url="$1"
  shift
  curl -sS -o /tmp/fitness_resp.json -w "%{http_code}" "$@" "$url"
}

extract_json_field() {
  local field="$1"
  python3 - "$field" <<'PY'
import json
import sys

field = sys.argv[1]
with open('/tmp/fitness_resp.json', 'r', encoding='utf-8') as f:
    data = json.load(f)
print(data.get(field, ''))
PY
}

print_step "Register"
CODE=$(http_code "$BASE_URL/auth/register" \
  -X POST \
  -H "Content-Type: application/json" \
  -d "{\"login\":\"$LOGIN\",\"name\":\"Ivan\",\"surname\":\"Petrov\",\"email\":\"$EMAIL\",\"password\":\"$PASSWORD\"}")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Login"
CODE=$(http_code "$BASE_URL/auth/login" \
  -X POST \
  -H "Content-Type: application/json" \
  -d "{\"login\":\"$LOGIN\",\"password\":\"$PASSWORD\"}")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty
TOKEN=$(extract_json_field token)

if [[ -z "$TOKEN" ]]; then
  echo "Token is empty, stopping tests"
  exit 1
fi

print_step "Unauthorized create workout (expect 401)"
CODE=$(http_code "$BASE_URL/workouts" \
  -X POST \
  -H "Content-Type: application/json" \
  -d '{"user_id":1,"date":"2026-03-27"}')
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Create exercise with token"
CODE=$(http_code "$BASE_URL/exercises" \
  -X POST \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"name":"Squat","description":"Leg exercise","muscle_group":"legs"}')
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty
EXERCISE_ID=$(extract_json_field id)

print_step "Get exercises"
CODE=$(http_code "$BASE_URL/exercises")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Get user id by login"
CODE=$(http_code "$BASE_URL/users/$LOGIN")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty
USER_ID=$(extract_json_field id)

print_step "Create workout with token"
CODE=$(http_code "$BASE_URL/workouts" \
  -X POST \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"user_id\":$USER_ID,\"date\":\"2026-03-27\"}")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty
WORKOUT_ID=$(extract_json_field id)

print_step "Add exercise to workout"
CODE=$(http_code "$BASE_URL/workouts/$WORKOUT_ID/exercises" \
  -X POST \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"exercise_id\":$EXERCISE_ID,\"sets\":4,\"reps\":10,\"weight\":80}")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Get user workouts (protected)"
CODE=$(http_code "$BASE_URL/users/$USER_ID/workouts" \
  -H "Authorization: Bearer $TOKEN")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Get user stats (protected)"
CODE=$(http_code "$BASE_URL/users/$USER_ID/stats?from=2026-01-01&to=2026-12-31" \
  -H "Authorization: Bearer $TOKEN")
echo "HTTP $CODE"
cat /tmp/fitness_resp.json | pretty

print_step "Done"
echo "All tests finished"
