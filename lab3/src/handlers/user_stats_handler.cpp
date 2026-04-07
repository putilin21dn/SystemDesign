#include "user_stats_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

UserStatsHandler::UserStatsHandler(
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

std::string UserStatsHandler::HandleRequestThrow(
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
    const auto from_date = request.GetArg("from");
    const auto to_date = request.GetArg("to");

    if (from_date.empty() || to_date.empty()) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kBadRequest,
                                      "Missing from or to parameters");
    }

    if (user_id != auth_user_id) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kForbidden,
                                      "You can view only your own statistics");
    }

    const auto user = user_service_->GetUserById(user_id);
    if (!user) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kNotFound,
                                      "User not found");
    }

    const auto stats = workout_service_->GetUserStats(user_id, from_date, to_date);

    json::ValueBuilder response;
    response["total_workouts"] = stats.total_workouts;
    response["total_weight"] = stats.total_weight;
    response["total_duration"] = stats.total_duration;

    utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
    return json::ToString(response.ExtractValue());
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid user id");
  }
}

}  // namespace handlers
