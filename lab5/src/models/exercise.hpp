#pragma once

#include <string>

#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>

namespace models {

struct Exercise {
  int64_t id;
  std::string name;
  std::string description;
  std::string muscle_group;
  std::string created_at;
};

inline userver::formats::json::Value Serialize(
    const Exercise& exercise,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = exercise.id;
  builder["name"] = exercise.name;
  builder["description"] = exercise.description;
  builder["muscle_group"] = exercise.muscle_group;
  builder["created_at"] = exercise.created_at;
  return builder.ExtractValue();
}

inline Exercise Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<Exercise>) {
  return Exercise{
      json["id"].As<int64_t>(),
      json["name"].As<std::string>(),
      json["description"].As<std::string>(),
      json["muscle_group"].As<std::string>(),
      json["created_at"].As<std::string>()};
}

}  // namespace models
