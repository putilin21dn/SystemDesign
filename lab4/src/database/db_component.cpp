#include "db_component.hpp"

namespace database {

DatabaseComponent::DatabaseComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : Component(config, context) {
  auto& pg_cluster = context.FindComponent<
      userver::components::Postgres>("postgres-db");
  cluster_ = pg_cluster.GetCluster();
}

yaml_config::Schema DatabaseComponent::GetStaticConfigSchema() {
  return yaml_config::Schema::EmptyConfig();
}

}  // namespace database
