#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/mongo/pool.hpp>

namespace handlers {

class MongoStatsHandler final
    : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-mongo-stats";

  MongoStatsHandler(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  userver::storages::mongo::PoolPtr pool_;
};

}  // namespace handlers
