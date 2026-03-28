#include "exercise_service.hpp"

#include <exception>
#include <spdlog/spdlog.h>

namespace services {

ExerciseService::ExerciseService(
    std::shared_ptr<userver::storages::postgres::Cluster> cluster)
    : cluster_(cluster) {}

models::Exercise ExerciseService::CreateExercise(
    const std::string& name,
    const std::string& description,
    const std::string& muscle_group) {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO exercises (name, description, muscle_group) "
      "VALUES ($1, $2, $3) "
      "RETURNING id, name, description, muscle_group, created_at::text AS created_at",
      name, description, muscle_group);

  auto row = result[0];
  return models::Exercise{
      row["id"].As<int64_t>(),
      row["name"].As<std::string>(),
      row["description"].As<std::string>(),
      row["muscle_group"].As<std::string>(),
      row["created_at"].As<std::string>()};
}

std::vector<models::Exercise> ExerciseService::GetAllExercises() {
  std::vector<models::Exercise> result;
  try {
    auto res = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, muscle_group, created_at::text AS created_at "
        "FROM exercises ORDER BY id");

    for (auto row : res) {
      result.push_back(models::Exercise{
          row["id"].As<int64_t>(),
          row["name"].As<std::string>(),
          row["description"].As<std::string>(),
          row["muscle_group"].As<std::string>(),
          row["created_at"].As<std::string>()});
    }
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
  }
  return result;
}

std::optional<models::Exercise> ExerciseService::GetExerciseById(
    int64_t exercise_id) {
  try {
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, name, description, muscle_group, created_at::text AS created_at "
        "FROM exercises WHERE id = $1",
        exercise_id);

    if (result.IsEmpty()) {
      return std::nullopt;
    }

    auto row = result[0];
    return models::Exercise{
        row["id"].As<int64_t>(),
        row["name"].As<std::string>(),
        row["description"].As<std::string>(),
        row["muscle_group"].As<std::string>(),
        row["created_at"].As<std::string>()};
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
    return std::nullopt;
  }
}

}  // namespace services
