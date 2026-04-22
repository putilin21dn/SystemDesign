#pragma once

#include <string>
#include <optional>
#include <vector>
#include <userver/storages/postgres/cluster.hpp>

#include "models/user.hpp"

namespace services {

class UserService {
 public:
  explicit UserService(
      std::shared_ptr<userver::storages::postgres::Cluster> cluster);

  models::User CreateUser(const std::string& login,
                          const std::string& name,
                          const std::string& surname,
                          const std::string& email);

  std::optional<models::User> GetUserByLogin(const std::string& login);

  std::vector<models::User> SearchUsersByName(const std::string& name,
                                              const std::string& surname = "");

  std::optional<models::User> GetUserById(int64_t user_id);

 private:
  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
};

}  // namespace services
