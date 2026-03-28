#include "user_workouts_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/workout.hpp"
#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

UserWorkoutsHandler::UserWorkoutsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  const auto cluster = pg_cluster.GetCluster();
  user_service_ = std::make_unique<services::UserService>(cluster);
  workout_service_ = std::make_unique<services::WorkoutService>(cluster);
  auth_service_ = std::make_shared<services::AuthService>(cluster);
  auth_middleware_ = std::make_unique<middleware::AuthMiddleware>(auth_service_);
}

std::string UserWorkoutsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
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
    const auto user_id = std::stoll(request.GetPathArg("id"));
    if (user_id != auth_user_id) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kForbidden,
                                      "You can view only your own workouts");
    }

    const auto user = user_service_->GetUserById(user_id);
    if (!user) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kNotFound,
                                      "User not found");
    }

    const auto workouts = workout_service_->GetUserWorkouts(user_id);
    json::ValueBuilder response;
    json::ValueBuilder workouts_array(json::Type::kArray);

    for (const auto& workout : workouts) {
      workouts_array.PushBack(models::Serialize(
          workout,
          userver::formats::serialize::To<userver::formats::json::Value>{}));
    }

    response["workouts"] = workouts_array.ExtractValue();
    utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
    return json::ToString(response.ExtractValue());
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid user id");
  }
}

}  // namespace handlers
