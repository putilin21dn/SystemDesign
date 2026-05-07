#include "auth_middleware.hpp"

#include <string>

namespace middleware {
namespace {
constexpr std::string_view kAuthPrefix = "Bearer ";
}

AuthMiddleware::AuthMiddleware(
    std::shared_ptr<services::AuthService> auth_service)
    : auth_service_(std::move(auth_service)) {}

bool AuthMiddleware::Check(const userver::server::http::HttpRequest& request,
                           int64_t& user_id,
                           std::string& error_message) const {
  const auto auth_header = request.GetHeader("Authorization");
  if (auth_header.empty()) {
    error_message = "Missing Authorization header";
    return false;
  }

  if (auth_header.rfind(kAuthPrefix.data(), 0) != 0) {
    error_message = "Authorization must use Bearer token";
    return false;
  }

  const auto token = auth_header.substr(kAuthPrefix.size());
  if (token.empty()) {
    error_message = "Empty Bearer token";
    return false;
  }

  const auto token_user_id = auth_service_->ValidateBearerToken(token);
  if (!token_user_id) {
    error_message = "Invalid or expired token";
    return false;
  }

  user_id = *token_user_id;
  return true;
}

}  // namespace middleware
