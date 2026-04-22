#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <userver/server/http/http_request.hpp>

#include "services/auth_service.hpp"

namespace middleware {

class AuthMiddleware {
 public:
  explicit AuthMiddleware(std::shared_ptr<services::AuthService> auth_service);

  bool Check(const userver::server::http::HttpRequest& request,
             int64_t& user_id,
             std::string& error_message) const;

 private:
  std::shared_ptr<services::AuthService> auth_service_;
};

}  // namespace middleware
