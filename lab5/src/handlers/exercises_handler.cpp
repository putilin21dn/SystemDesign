#include "exercises_handler.hpp"

#include <chrono>

#include <userver/components/component_context.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

#include "models/exercise.hpp"
#include "utils/http_response.hpp"
#include "utils/response_cache.hpp"

namespace handlers {
namespace json = userver::formats::json;

ExercisesHandler::ExercisesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
  auto& pg_cluster =
      context.FindComponent<userver::components::Postgres>("postgres-db");
  exercise_service_ =
      std::make_unique<services::ExerciseService>(pg_cluster.GetCluster());
}

std::string ExercisesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  if (request.GetMethod() != userver::server::http::HttpMethod::kGet) {
    return utils::MakeErrorResponse(request,
                                    userver::server::http::HttpStatus::kMethodNotAllowed,
                                    "Method not allowed");
  }

  constexpr auto kCacheKey = "exercises:list";
  if (const auto cached = utils::GlobalResponseCache().Get(kCacheKey)) {
    utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
    utils::SetHeader(request, "X-Cache", "HIT");
    return *cached;
  }

  const auto exercises = exercise_service_->GetAllExercises();

  json::ValueBuilder response;
  json::ValueBuilder exercises_array(json::Type::kArray);
  for (const auto& exercise : exercises) {
    exercises_array.PushBack(models::Serialize(
        exercise,
        userver::formats::serialize::To<userver::formats::json::Value>{}));
  }

  response["exercises"] = exercises_array.ExtractValue();
  const auto body = json::ToString(response.ExtractValue());
  utils::GlobalResponseCache().Put(kCacheKey, body, std::chrono::seconds{60});
  utils::SetStatus(request, userver::server::http::HttpStatus::kOk);
  utils::SetHeader(request, "X-Cache", "MISS");
  return body;
}

}  // namespace handlers
