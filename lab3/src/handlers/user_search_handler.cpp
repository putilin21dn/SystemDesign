#include "user_search_handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/user.hpp"
#include "utils/http_response.hpp"

namespace handlers {
namespace json = userver::formats::json;

UserSearchHandler::UserSearchHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  user_service_ = std::make_unique<services::UserService>(pg_cluster.GetCluster());
}

std::string UserSearchHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kMethodNotAllowed,
                                    "Method not allowed");
  }

  const auto name = request.GetArg("name");
  const auto surname = request.GetArg("surname");
  if (name.empty() && surname.empty()) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kBadRequest,
                                    "At least one query parameter is required: name or surname");
  }

  const auto users = user_service_->SearchUsersByName(name, surname);

  json::ValueBuilder response;
  json::ValueBuilder users_array(json::Type::kArray);
  for (const auto& user : users) {
    users_array.PushBack(models::Serialize(
        user,
        userver::formats::serialize::To<userver::formats::json::Value>{}));
  }

  response["users"] = users_array.ExtractValue();
  utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
  return json::ToString(response.ExtractValue());
}

}  // namespace handlers
