#include "workout_service.hpp"

#include <exception>
#include <spdlog/spdlog.h>

namespace services {

WorkoutService::WorkoutService(
    std::shared_ptr<userver::storages::postgres::Cluster> cluster)
    : cluster_(cluster) {}

models::Workout WorkoutService::CreateWorkout(int64_t user_id,
                                               const std::string& date) {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO workouts (user_id, date, duration) "
      "VALUES ($1, $2::date, $3) "
      "RETURNING id, user_id, date::text AS date, duration, created_at::text AS created_at",
      user_id, date, 0);

  auto row = result[0];
  return models::Workout{
      row["id"].As<int64_t>(),
      row["user_id"].As<int64_t>(),
      row["date"].As<std::string>(),
      row["duration"].As<int64_t>(),
      row["created_at"].As<std::string>()};
}

std::optional<models::Workout> WorkoutService::GetWorkoutById(
    int64_t workout_id) {
  try {
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, user_id, date::text AS date, duration, created_at::text AS created_at "
        "FROM workouts WHERE id = $1",
        workout_id);

    if (result.IsEmpty()) {
      return std::nullopt;
    }

    auto row = result[0];
    return models::Workout{
        row["id"].As<int64_t>(),
        row["user_id"].As<int64_t>(),
        row["date"].As<std::string>(),
        row["duration"].As<int64_t>(),
        row["created_at"].As<std::string>()};
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
    return std::nullopt;
  }
}

std::vector<models::Workout> WorkoutService::GetUserWorkouts(
    int64_t user_id) {
  std::vector<models::Workout> result;
  try {
    auto res = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, user_id, date::text AS date, duration, created_at::text AS created_at "
        "FROM workouts WHERE user_id = $1 ORDER BY date DESC",
        user_id);

    for (auto row : res) {
      result.push_back(models::Workout{
          row["id"].As<int64_t>(),
          row["user_id"].As<int64_t>(),
          row["date"].As<std::string>(),
          row["duration"].As<int64_t>(),
          row["created_at"].As<std::string>()});
    }
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
  }
  return result;
}

models::WorkoutExercise WorkoutService::AddExerciseToWorkout(
    int64_t workout_id,
    int64_t exercise_id,
    int64_t sets,
    int64_t reps,
    double weight) {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO workout_exercises (workout_id, exercise_id, sets, reps, weight) "
      "VALUES ($1, $2, $3, $4, $5) "
      "RETURNING id, workout_id, exercise_id, sets, reps, weight, created_at::text AS created_at",
      workout_id, exercise_id, sets, reps, weight);

  auto row = result[0];
  return models::WorkoutExercise{
      row["id"].As<int64_t>(),
      row["workout_id"].As<int64_t>(),
      row["exercise_id"].As<int64_t>(),
      row["sets"].As<int64_t>(),
      row["reps"].As<int64_t>(),
      row["weight"].As<double>(),
      row["created_at"].As<std::string>()};
}

WorkoutStats WorkoutService::GetUserStats(int64_t user_id,
                                          const std::string& from_date,
                                          const std::string& to_date) {
  try {
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT COUNT(w.id)::bigint AS total_workouts, "
        "COALESCE(SUM(we.weight * we.sets * we.reps), 0)::double precision AS total_weight, "
        "COALESCE(SUM(w.duration), 0)::bigint AS total_duration "
        "FROM workouts w "
        "LEFT JOIN workout_exercises we ON w.id = we.workout_id "
        "WHERE w.user_id = $1 AND w.date BETWEEN $2::date AND $3::date",
        user_id, from_date, to_date);

    if (result.IsEmpty()) {
      return WorkoutStats{0, 0, 0};
    }

    auto row = result[0];
    return WorkoutStats{
        row["total_workouts"].As<int64_t>(),
        row["total_weight"].As<double>(),
        row["total_duration"].As<int64_t>()};
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
    return WorkoutStats{0, 0, 0};
  }
}

}  // namespace services
