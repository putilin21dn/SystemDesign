#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/serialize.hpp>

namespace models {

struct WorkoutExercise {
  int64_t id;
  int64_t workout_id;
  int64_t exercise_id;
  int64_t sets;
  int64_t reps;
  double weight;  // in kg
  std::string created_at;
};

inline userver::formats::json::Value Serialize(
    const WorkoutExercise& workout_exercise,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = workout_exercise.id;
  builder["workout_id"] = workout_exercise.workout_id;
  builder["exercise_id"] = workout_exercise.exercise_id;
  builder["sets"] = workout_exercise.sets;
  builder["reps"] = workout_exercise.reps;
  builder["weight"] = workout_exercise.weight;
  builder["created_at"] = workout_exercise.created_at;
  return builder.ExtractValue();
}

inline WorkoutExercise Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<WorkoutExercise>) {
  return WorkoutExercise{
      json["id"].As<int64_t>(),
      json["workout_id"].As<int64_t>(),
      json["exercise_id"].As<int64_t>(),
      json["sets"].As<int64_t>(),
      json["reps"].As<int64_t>(),
      json["weight"].As<double>(),
      json["created_at"].As<std::string>()};
}

}  // namespace models
