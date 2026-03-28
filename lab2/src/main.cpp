#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/auth_login_handler.hpp"
#include "handlers/auth_register_handler.hpp"
#include "handlers/exercises_create_handler.hpp"
#include "handlers/exercises_handler.hpp"
#include "handlers/user_by_login_handler.hpp"
#include "handlers/user_search_handler.hpp"
#include "handlers/user_stats_handler.hpp"
#include "handlers/user_workouts_handler.hpp"
#include "handlers/users_handler.hpp"
#include "handlers/workout_exercises_handler.hpp"
#include "handlers/workouts_handler.hpp"

int main(int argc, char* argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::clients::dns::Component>()
          .Append<userver::components::Postgres>("postgres-db")
          .Append<handlers::AuthRegisterHandler>()
          .Append<handlers::AuthLoginHandler>()
          .Append<handlers::UsersHandler>()
          .Append<handlers::UserByLoginHandler>()
          .Append<handlers::UserSearchHandler>()
          .Append<handlers::ExercisesHandler>()
          .Append<handlers::ExercisesCreateHandler>()
          .Append<handlers::WorkoutsHandler>()
          .Append<handlers::WorkoutExercisesHandler>()
          .Append<handlers::UserWorkoutsHandler>()
          .Append<handlers::UserStatsHandler>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}
