#include "users_handler.hpp"

#include <exception>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

UsersHandler::UsersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  user_service_ = std::make_unique<services::UserService>(pg_cluster.GetCluster());
}

std::string UsersHandler::HandleRequestThrow(
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

    if (login.empty() || name.empty() || surname.empty() || email.empty()) {
      return utils::MakeErrorResponse(request,
                                      userver::server::http::HttpStatus::kBadRequest,
                                      "All fields are required");
    }

    const auto user = user_service_->CreateUser(login, name, surname, email);

    json::ValueBuilder response;
    response["id"] = user.id;
    response["login"] = user.login;
    response["name"] = user.name;
    response["surname"] = user.surname;
    response["email"] = user.email;

    utils::SetStatus(request, userver::server::http::HttpStatus::kCreated);
    return json::ToString(response.ExtractValue());
  } catch (const std::exception&) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "Invalid request body");
  }
}

}  // namespace handlers
