#pragma once

#include <string>

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/http/http_status.hpp>

namespace utils {
namespace json = userver::formats::json;

inline std::string MakeErrorResponse(
    const userver::server::http::HttpRequest& request,
    userver::server::http::HttpStatus status,
    const std::string& message) {
  auto& response = request.GetHttpResponse();
  response.SetStatus(status);

  json::ValueBuilder error;
  error["error"] = message;
  return json::ToString(error.ExtractValue());
}

inline void SetStatus(const userver::server::http::HttpRequest& request,
                      userver::server::http::HttpStatus status) {
  request.GetHttpResponse().SetStatus(status);
}

inline void SetHeader(const userver::server::http::HttpRequest& request,
                      const std::string& name,
                      const std::string& value) {
  request.GetHttpResponse().SetHeader(name, value);
}

}  // namespace utils
