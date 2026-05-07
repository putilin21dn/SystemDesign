#pragma once

#include <memory>

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

#include "middleware/auth_middleware.hpp"
#include "services/auth_service.hpp"
#include "services/exercise_service.hpp"

namespace handlers {

class ExercisesCreateHandler final
    : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-exercises-create";

  ExercisesCreateHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  std::unique_ptr<services::ExerciseService> exercise_service_;
  std::shared_ptr<services::AuthService> auth_service_;
  std::unique_ptr<middleware::AuthMiddleware> auth_middleware_;
};

}  // namespace handlers
