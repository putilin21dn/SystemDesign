#include "mongo_stats_handler.hpp"

#include <chrono>

#include <userver/components/component_context.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/mongo/component.hpp>

#include "utils/http_response.hpp"
#include "utils/response_cache.hpp"

namespace handlers {
namespace bson = userver::formats::bson;
namespace json = userver::formats::json;

MongoStatsHandler::MongoStatsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pool_(context.FindComponent<userver::components::Mongo>("mongo-db")
                .GetPool()) {}

std::string MongoStatsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
    return utils::MakeErrorResponse(
        request, userver::server::http::HttpStatus::kMethodNotAllowed,
        "Method not allowed");
  }

  constexpr auto kCacheKey = "mongo:stats:users";
  if (const auto cached = utils::GlobalResponseCache().Get(kCacheKey)) {
    utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
    utils::SetHeader(request, "X-Cache", "HIT");
    return *cached;
  }

  auto collection = pool_->GetCollection("workouts");
  auto cursor = collection.Aggregate(bson::MakeArray(
      bson::MakeDoc("$group",
                    bson::MakeDoc("_id", "$user_id", "workouts_count",
                                  bson::MakeDoc("$sum", 1), "total_duration",
                                  bson::MakeDoc("$sum", "$duration"),
                                  "avg_duration",
                                  bson::MakeDoc("$avg", "$duration"))),
      bson::MakeDoc("$sort", bson::MakeDoc("total_duration", -1))));

  json::ValueBuilder response;
  auto users = response["users"];
  if (cursor) {
    for (const auto& doc : cursor) {
      json::ValueBuilder item;
      item["user_id"] = doc["_id"].As<std::string>();
      item["workouts_count"] = doc["workouts_count"].As<int64_t>();
      item["total_duration"] = doc["total_duration"].As<int64_t>();
      item["avg_duration"] = doc["avg_duration"].As<double>();
      users.PushBack(item.ExtractValue());
    }
  }

  const auto body = json::ToString(response.ExtractValue());
  utils::GlobalResponseCache().Put(kCacheKey, body, std::chrono::seconds{120});
  utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
  utils::SetHeader(request, "X-Cache", "MISS");
  return body;
}

}  // namespace handlers
