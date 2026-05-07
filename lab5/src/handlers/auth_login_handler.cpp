#include "auth_login_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

AuthLoginHandler::AuthLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  auth_service_ =
      std::make_shared<services::AuthService>(pg_cluster.GetCluster());
}

std::string AuthLoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kPost) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kMethodNotAllowed,
                                    "Method not allowed");
  }

  try {
    const auto body = json::FromString(request.RequestBody());
    const auto login = body["login"].As<std::string>();
    const auto password = body["password"].As<std::string>();

    if (login.empty() || password.empty()) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kBadRequest,
                                      "Login and password are required");
    }

    const auto login_result = auth_service_->Login(login, password);
    if (!login_result) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kUnauthorized,
                                      "Invalid credentials");
    }

    json::ValueBuilder response;
    response["token"] = login_result->token;
    response["token_type"] = "Bearer";
    response["expires_at"] = login_result->expires_at;
    response["user_id"] = login_result->user_id;

    utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
    return json::ToString(response.ExtractValue());
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid JSON body");
  }
}

}  // namespace handlers
