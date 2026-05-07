#pragma once

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace database {

class DatabaseComponent final
    : public userver::components::Component {
 public:
  static constexpr std::string_view kName = "database";

  DatabaseComponent(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& context);

  std::shared_ptr<userver::storages::postgres::Cluster> GetCluster() const {
    return cluster_;
  }

  static yaml_config::Schema GetStaticConfigSchema();

 private:
  std::shared_ptr<userver::storages::postgres::Cluster> cluster_;
};

}  // namespace database
