#include "auth_register_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/user.hpp"
#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

AuthRegisterHandler::AuthRegisterHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  auth_service_ =
      std::make_shared<services::AuthService>(pg_cluster.GetCluster());
}

std::string AuthRegisterHandler::HandleRequestThrow(
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
    const auto name = body["name"].As<std::string>();
    const auto surname = body["surname"].As<std::string>();
    const auto email = body["email"].As<std::string>();
    const auto password = body["password"].As<std::string>();

    if (login.empty() || name.empty() || surname.empty() || email.empty() ||
        password.empty()) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kBadRequest,
                                      "All fields are required");
    }

    const auto user =
        auth_service_->RegisterUser(login, name, surname, email, password);

    utils::SetStatus(request, userver::server::http::HttpStatus::kCreated);
    return json::ToString(models::Serialize(
        user,
        userver::formats::serialize::To<userver::formats::json::Value>{}));
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kConflict,
                                    "User with this login/email already exists");
  }
}

}  // namespace handlers
