#include "exercises_create_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/exercise.hpp"
#include "utils/http_response.hpp"
#include "utils/response_cache.hpp"

namespace handlers {
namespace json = userver::formats::json;

ExercisesCreateHandler::ExercisesCreateHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  const auto cluster = pg_cluster.GetCluster();
  exercise_service_ = std::make_unique<services::ExerciseService>(cluster);
  auth_service_ = std::make_shared<services::AuthService>(cluster);
  auth_middleware_ = std::make_unique<middleware::AuthMiddleware>(auth_service_);
}

std::string ExercisesCreateHandler::HandleRequestThrow(
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
    const auto name = body["name"].As<std::string>();
    const auto description = body["description"].As<std::string>();
    const auto muscle_group = body["muscle_group"].As<std::string>();

    if (name.empty() || muscle_group.empty()) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kBadRequest,
                                      "name and muscle_group are required");
    }

    const auto exercise =
        exercise_service_->CreateExercise(name, description, muscle_group);
    utils::GlobalResponseCache().Invalidate("exercises:list");

    utils::SetStatus(request, userver::server::http::HttpStatus::kCreated);
    return json::ToString(models::Serialize(
        exercise,
        userver::formats::serialize::To<userver::formats::json::Value>{}));
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid request body");
  }
}

}  // namespace handlers
