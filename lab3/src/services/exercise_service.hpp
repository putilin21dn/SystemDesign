#pragma once

#include <string>
#include <optional>
#include <vector>
#include <userver/storages/postgres/cluster.hpp>

#include "models/exercise.hpp"

namespace services {

class ExerciseService {
 public:
  explicit ExerciseService(
      std::shared_ptr<userver::storages::postgres::Cluster> cluster);

  models::Exercise CreateExercise(const std::string& name,
                                  const std::string& description,
                                  const std::string& muscle_group);

  std::vector<models::Exercise> GetAllExercises();

  std::optional<models::Exercise> GetExerciseById(int64_t exercise_id);

 private:
  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
};

}  // namespace services
