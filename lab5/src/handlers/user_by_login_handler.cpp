#include "user_by_login_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/user.hpp"
#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

UserByLoginHandler::UserByLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  user_service_ = std::make_unique<services::UserService>(pg_cluster.GetCluster());
}

std::string UserByLoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kMethodNotAllowed,
                                    "Method not allowed");
  }

  const auto login = request.GetPathArg("login");
  const auto user = user_service_->GetUserByLogin(login);
  if (!user) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kNotFound,
                                    "User not found");
  }

  utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
  return json::ToString(models::Serialize(
      *user,
      userver::formats::serialize::To<userver::formats::json::Value>{}));
}

}  // namespace handlers
