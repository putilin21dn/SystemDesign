#pragma once

#include <string>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/serialize.hpp>

namespace models {

struct User {
  int64_t id;
  std::string login;
  std::string name;
  std::string surname;
  std::string email;
  std::string created_at;
};

inline userver::formats::json::Value Serialize(
    const User& user,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["id"] = user.id;
  builder["login"] = user.login;
  builder["name"] = user.name;
  builder["surname"] = user.surname;
  builder["email"] = user.email;
  builder["created_at"] = user.created_at;
  return builder.ExtractValue();
}

inline User Parse(const userver::formats::json::Value& json,
                  userver::formats::parse::To<User>) {
  return User{
      json["id"].As<int64_t>(),
      json["login"].As<std::string>(),
      json["name"].As<std::string>(),
      json["surname"].As<std::string>(),
      json["email"].As<std::string>(),
      json["created_at"].As<std::string>()};
}

}  // namespace models
