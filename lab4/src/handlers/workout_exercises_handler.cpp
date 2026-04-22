#include "workout_exercises_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/workout_exercise.hpp"
#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

WorkoutExercisesHandler::WorkoutExercisesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  const auto cluster = pg_cluster.GetCluster();
  workout_service_ = std::make_unique<services::WorkoutService>(cluster);
  auth_service_ = std::make_shared<services::AuthService>(cluster);
  auth_middleware_ = std::make_unique<middleware::AuthMiddleware>(auth_service_);
}

std::string WorkoutExercisesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kPost) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kMethodNotAllowed,
                                    "Method not allowed");
  }

  int64_t auth_user_id = 0;
  std::string auth_error;
  if (!auth_middleware_->Check(request, auth_user_id, auth_error)) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kUnauthorized,
                                    auth_error);
  }

  try {
    const auto body = json::FromString(request.RequestBody());
    const auto workout_id = std::stoll(request.GetPathArg("workout_id"));
    const auto exercise_id = body["exercise_id"].As<int64_t>();
    const auto sets = body["sets"].As<int64_t>();
    const auto reps = body["reps"].As<int64_t>();
    const auto weight = body["weight"].As<double>();

    const auto workout = workout_service_->GetWorkoutById(workout_id);
    if (!workout) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kNotFound,
                                      "Workout not found");
    }

    if (workout->user_id != auth_user_id) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kForbidden,
                                      "You can update only your own workouts");
    }

    const auto workout_exercise = workout_service_->AddExerciseToWorkout(
        workout_id, exercise_id, sets, reps, weight);

    utils::SetStatus(request, userver::server::http::HttpStatus::kCreated);
    return json::ToString(models::Serialize(
        workout_exercise,
        userver::formats::serialize::To<userver::formats::json::Value>{}));
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid request body");
  }
}

}  // namespace handlers
