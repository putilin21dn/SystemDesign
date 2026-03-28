#include "auth_service.hpp"

#include <array>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>

namespace services {
namespace {
constexpr std::size_t kTokenBytes = 32;
constexpr std::string_view kPasswordSalt = "fitness-tracker-salt-v1";
}  // namespace

AuthService::AuthService(
    std::shared_ptr<userver::storages::postgres::Cluster> cluster)
    : cluster_(std::move(cluster)) {}

models::User AuthService::RegisterUser(const std::string& login,
                                       const std::string& name,
                                       const std::string& surname,
                                       const std::string& email,
                                       const std::string& password) {
  const auto password_hash = HashPassword(password);

  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO users (login, name, surname, email, password_hash) "
      "VALUES ($1, $2, $3, $4, $5) "
      "RETURNING id, login, name, surname, email, created_at::text AS created_at",
      login, name, surname, email, password_hash);

  const auto row = result[0];
  return models::User{row["id"].As<int64_t>(),
                      row["login"].As<std::string>(),
                      row["name"].As<std::string>(),
                      row["surname"].As<std::string>(),
                      row["email"].As<std::string>(),
                      row["created_at"].As<std::string>()};
}

std::optional<LoginResult> AuthService::Login(const std::string& login,
                                              const std::string& password) {
  const auto user_result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT id, password_hash FROM users WHERE login = $1", login);

  if (user_result.IsEmpty()) {
    return std::nullopt;
  }

  const auto row = user_result[0];
  const auto user_id = row["id"].As<int64_t>();
  const auto expected_hash = row["password_hash"].As<std::string>();

  if (expected_hash != HashPassword(password)) {
    return std::nullopt;
  }

  const auto token = GenerateToken();

  auto session_result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO auth_sessions (token, user_id, expires_at) "
      "VALUES ($1, $2, NOW() + INTERVAL '24 hours') "
      "RETURNING expires_at::text AS expires_at",
      token, user_id);

  return LoginResult{token,
                     user_id,
                     session_result[0]["expires_at"].As<std::string>()};
}

std::optional<int64_t> AuthService::ValidateBearerToken(const std::string& token) {
  const auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT user_id FROM auth_sessions WHERE token = $1 AND expires_at > NOW()",
      token);

  if (result.IsEmpty()) {
    return std::nullopt;
  }

  return result[0]["user_id"].As<int64_t>();
}

std::string AuthService::HashPassword(const std::string& password) {
  const auto mixed = std::string(kPasswordSalt) + password;
  const auto digest = std::hash<std::string>{}(mixed);

  std::ostringstream stream;
  stream << std::hex << digest;
  return stream.str();
}

std::string AuthService::GenerateToken() {
  std::array<unsigned char, kTokenBytes> bytes{};

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 255);

  for (auto& value : bytes) {
    value = static_cast<unsigned char>(dist(gen));
  }

  std::ostringstream stream;
  stream << std::hex << std::setfill('0');
  for (const auto value : bytes) {
    stream << std::setw(2) << static_cast<int>(value);
  }

  return stream.str();
}

}  // namespace services
