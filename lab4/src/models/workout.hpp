#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/serialize.hpp>

namespace models {

struct Workout {
  int64_t id;
  int64_t user_id;
  std::string date;
  int64_t duration;  // in minutes
  std::string created_at;
};

inline userver::formats::json::Value Serialize(
    const Workout& workout,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = workout.id;
  builder["user_id"] = workout.user_id;
  builder["date"] = workout.date;
  builder["duration"] = workout.duration;
  builder["created_at"] = workout.created_at;
  return builder.ExtractValue();
}

inline Workout Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<Workout>) {
  return Workout{
      json["id"].As<int64_t>(),
      json["user_id"].As<int64_t>(),
      json["date"].As<std::string>(),
      json["duration"].As<int64_t>(),
      json["created_at"].As<std::string>()};
}

}  // namespace models
