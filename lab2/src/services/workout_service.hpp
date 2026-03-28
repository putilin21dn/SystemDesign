#pragma once

#include <string>
#include <optional>
#include <vector>
#include <userver/storages/postgres/cluster.hpp>

#include "models/workout.hpp"
#include "models/workout_exercise.hpp"

namespace services {

struct WorkoutStats {
  int64_t total_workouts;
  double total_weight;
  int64_t total_duration;  // in minutes
};

class WorkoutService {
 public:
  explicit WorkoutService(
      std::shared_ptr<userver::storages::postgres::Cluster> cluster);

  models::Workout CreateWorkout(int64_t user_id, const std::string& date);

  std::optional<models::Workout> GetWorkoutById(int64_t workout_id);

  std::vector<models::Workout> GetUserWorkouts(int64_t user_id);

  models::WorkoutExercise AddExerciseToWorkout(int64_t workout_id,
                                               int64_t exercise_id,
                                               int64_t sets,
                                               int64_t reps,
                                               double weight);

  WorkoutStats GetUserStats(int64_t user_id,
                            const std::string& from_date,
                            const std::string& to_date);

 private:
  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
};

}  // namespace services
