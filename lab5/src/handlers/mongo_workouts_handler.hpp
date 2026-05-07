#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/mongo/pool.hpp>

namespace handlers {

class MongoWorkoutsHandler final
    : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-mongo-workouts";

  MongoWorkoutsHandler(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  std::string ListWorkouts(
      const userver::server::http::HttpRequest& request) const;
  std::string CreateWorkout(
      const userver::server::http::HttpRequest& request) const;

  userver::storages::mongo::PoolPtr pool_;
};

}  // namespace handlers
