#pragma once

#include <memory>

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <spdlog/spdlog.h>

#include "services/user_service.hpp"

namespace handlers {

class UsersHandler final 
    : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-users";

  UsersHandler(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
  std::unique_ptr<services::UserService> user_service_;
};

}  // namespace handlers
