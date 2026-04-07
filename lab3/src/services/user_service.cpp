#include "user_service.hpp"

#include <exception>
#include <userver/storages/postgres/transaction.hpp>
#include <spdlog/spdlog.h>

namespace services {

UserService::UserService(
    std::shared_ptr<userver::storages::postgres::Cluster> cluster)
    : cluster_(cluster) {}

models::User UserService::CreateUser(const std::string& login,
                                      const std::string& name,
                                      const std::string& surname,
                                      const std::string& email) {
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO users (login, name, surname, email) "
      "VALUES ($1, $2, $3, $4) "
      "RETURNING id, login, name, surname, email, created_at::text AS created_at",
      login, name, surname, email);

  auto row = result[0];
  return models::User{
      row["id"].As<int64_t>(),
      row["login"].As<std::string>(),
      row["name"].As<std::string>(),
      row["surname"].As<std::string>(),
      row["email"].As<std::string>(),
      row["created_at"].As<std::string>()};
}

std::optional<models::User> UserService::GetUserByLogin(
    const std::string& login) {
  try {
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, name, surname, email, created_at::text AS created_at "
        "FROM users WHERE login = $1",
        login);

    if (result.IsEmpty()) {
      return std::nullopt;
    }

    auto row = result[0];
    return models::User{
        row["id"].As<int64_t>(),
        row["login"].As<std::string>(),
        row["name"].As<std::string>(),
        row["surname"].As<std::string>(),
        row["email"].As<std::string>(),
        row["created_at"].As<std::string>()};
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
    return std::nullopt;
  }
}

std::vector<models::User> UserService::SearchUsersByName(
    const std::string& name,
    const std::string& surname) {
  std::vector<models::User> result;
  try {
    const auto name_mask = "%" + name + "%";
    const auto surname_mask = "%" + surname + "%";

    auto res = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, name, surname, email, created_at::text AS created_at "
        "FROM users WHERE name ILIKE $1 AND surname ILIKE $2",
        name_mask, surname_mask);

    for (auto row : res) {
      result.push_back(models::User{
          row["id"].As<int64_t>(),
          row["login"].As<std::string>(),
          row["name"].As<std::string>(),
          row["surname"].As<std::string>(),
          row["email"].As<std::string>(),
          row["created_at"].As<std::string>()});
    }
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
  }
  return result;
}

std::optional<models::User> UserService::GetUserById(int64_t user_id) {
  try {
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, name, surname, email, created_at::text AS created_at "
        "FROM users WHERE id = $1",
        user_id);

    if (result.IsEmpty()) {
      return std::nullopt;
    }

    auto row = result[0];
    return models::User{
        row["id"].As<int64_t>(),
        row["login"].As<std::string>(),
        row["name"].As<std::string>(),
        row["surname"].As<std::string>(),
        row["email"].As<std::string>(),
        row["created_at"].As<std::string>()};
  } catch (const std::exception& e) {
    spdlog::error("Database error: {}", e.what());
    return std::nullopt;
  }
}

}  // namespace services
