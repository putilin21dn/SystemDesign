#include "mongo_workouts_handler.hpp"

#include <chrono>
#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/bson/value_builder.hpp>
#include <userver/formats/common/type.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/options.hpp>
#include <userver/utils/datetime.hpp>

#include "utils/http_response.hpp"

namespace handlers {
namespace bson = userver::formats::bson;
namespace json = userver::formats::json;
namespace mongo_options = userver::storages::mongo::options;

namespace {

std::string MakeGeneratedWorkoutId() {
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  const auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  return "workout_" + std::to_string(millis);
}

json::ValueBuilder WorkoutToJson(const bson::Value& doc) {
  json::ValueBuilder item;
  item["_id"] = doc["_id"].As<std::string>();
  item["user_id"] = doc["user_id"].As<std::string>();
  item["duration"] = doc["duration"].As<int64_t>();
  if (doc.HasMember("notes")) {
    item["notes"] = doc["notes"].As<std::string>();
  }

  auto exercises = item["exercises"];
  for (const auto& exercise : doc["exercises"]) {
    json::ValueBuilder exercise_json;
    exercise_json["exercise_id"] = exercise["exercise_id"].As<std::string>();
    exercise_json["name"] = exercise["name"].As<std::string>();
    exercise_json["sets"] = exercise["sets"].As<int64_t>();
    exercise_json["reps"] = exercise["reps"].As<int64_t>();
    exercise_json["weight"] = exercise["weight"].As<double>();
    exercises.PushBack(exercise_json.ExtractValue());
  }

  auto tags = item["tags"];
  for (const auto& tag : doc["tags"]) {
    tags.PushBack(tag.As<std::string>());
  }

  return item;
}

}  // namespace

MongoWorkoutsHandler::MongoWorkoutsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pool_(context.FindComponent<userver::components::Mongo>("mongo-db")
                .GetPool()) {}

std::string MongoWorkoutsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() == userver::server::http::HttpMethod::kGet) {
    return ListWorkouts(request);
  }
  if (request.GetMethod() == userver::server::http::HttpMethod::kPost) {
    return CreateWorkout(request);
  }

  return utils::MakeErrorResponse(
      request, userver::server::http::HttpStatus::kMethodNotAllowed,
      "Method not allowed");
}

std::string MongoWorkoutsHandler::ListWorkouts(
    const userver::server::http::HttpRequest& request) const {
  auto collection = pool_->GetCollection("workouts");

  auto filter = bson::MakeDoc();
  if (request.HasArg("user_id")) {
    filter = bson::MakeDoc("user_id", request.GetArg("user_id"));
  }

  auto cursor = collection.Find(
      filter, mongo_options::Sort{std::make_pair("date",
                                                 mongo_options::Sort::kDescending)});

  json::ValueBuilder response;
  auto workouts = response["workouts"];
  if (cursor) {
    for (const auto& doc : cursor) {
      workouts.PushBack(WorkoutToJson(doc).ExtractValue());
    }
  }

  return json::ToString(response.ExtractValue());
}

std::string MongoWorkoutsHandler::CreateWorkout(
    const userver::server::http::HttpRequest& request) const {
  try {
    const auto body = json::FromString(request.RequestBody());
    const auto user_id = body["user_id"].As<std::string>();
    const auto duration = body["duration"].As<int64_t>();
    const auto date = userver::utils::datetime::Stringtime(
        body["date"].As<std::string>());

    if (user_id.empty() || duration <= 0 || body["exercises"].IsEmpty()) {
      return utils::MakeErrorResponse(
          request, userver::server::http::HttpStatus::kBadRequest,
          "user_id, positive duration and exercises are required");
    }

    bson::ValueBuilder exercises_builder{
        userver::formats::common::Type::kArray};
    for (const auto& exercise : body["exercises"]) {
      exercises_builder.PushBack(bson::ValueBuilder(bson::MakeDoc(
          "exercise_id", exercise["exercise_id"].As<std::string>(), "name",
          exercise["name"].As<std::string>("Custom Exercise"), "sets",
          exercise["sets"].As<int64_t>(), "reps", exercise["reps"].As<int64_t>(),
          "weight", exercise["weight"].As<double>())));
    }

    bson::ValueBuilder tags_builder{userver::formats::common::Type::kArray};
    if (body.HasMember("tags")) {
      for (const auto& tag : body["tags"]) {
        tags_builder.PushBack(bson::ValueBuilder(tag.As<std::string>()));
      }
    }

    const auto workout_id = MakeGeneratedWorkoutId();
    auto collection = pool_->GetCollection("workouts");
    collection.InsertOne(bson::MakeDoc(
        "_id", workout_id, "user_id", user_id, "date", date, "duration",
        duration, "exercises", exercises_builder.ExtractValue(), "tags",
        tags_builder.ExtractValue(), "notes", body["notes"].As<std::string>(""),
        "created_at",
        std::chrono::system_clock::now()));

    json::ValueBuilder response;
    response["_id"] = workout_id;
    response["user_id"] = user_id;
    response["duration"] = duration;
    utils::SetStatus(request, userver::server::http::HttpStatus::kCreated);
    return json::ToString(response.ExtractValue());
  } catch (const std::exception& e) {
    return utils::MakeErrorResponse(
        request, userver::server::http::HttpStatus::kBadRequest, e.what());
  }
}

}  // namespace handlers
