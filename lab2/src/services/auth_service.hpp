#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <userver/storages/postgres/cluster.hpp>

#include "models/user.hpp"

namespace services {

struct LoginResult {
  std::string token;
  int64_t user_id;
  std::string expires_at;
};

class AuthService {
 public:
  explicit AuthService(
      std::shared_ptr<userver::storages::postgres::Cluster> cluster);

  models::User RegisterUser(const std::string& login,
                            const std::string& name,
                            const std::string& surname,
                            const std::string& email,
                            const std::string& password);

  std::optional<LoginResult> Login(const std::string& login,
                                   const std::string& password);

  std::optional<int64_t> ValidateBearerToken(const std::string& token);

 private:
  static std::string HashPassword(const std::string& password);
  static std::string GenerateToken();

  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
};

}  // namespace services
