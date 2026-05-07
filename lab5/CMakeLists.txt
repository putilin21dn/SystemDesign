cmake_minimum_required(VERSION 3.12)
project(fitness_tracker_api CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(userver REQUIRED COMPONENTS core postgresql mongo)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

set(SOURCES
    src/main.cpp
    src/services/user_service.cpp
    src/services/exercise_service.cpp
    src/services/workout_service.cpp
    src/services/auth_service.cpp
    src/middleware/auth_middleware.cpp
    src/handlers/auth_register_handler.cpp
    src/handlers/auth_login_handler.cpp
    src/handlers/users_handler.cpp
    src/handlers/user_by_login_handler.cpp
    src/handlers/user_search_handler.cpp
    src/handlers/exercises_handler.cpp
    src/handlers/exercises_create_handler.cpp
    src/handlers/workouts_handler.cpp
    src/handlers/workout_exercises_handler.cpp
    src/handlers/user_workouts_handler.cpp
    src/handlers/user_stats_handler.cpp
    src/handlers/mongo_workouts_handler.cpp
    src/handlers/mongo_stats_handler.cpp
)

add_executable(fitness_tracker_api ${SOURCES})

target_link_libraries(fitness_tracker_api
    userver::core
    userver::postgresql
    userver::mongo
    fmt::fmt
    spdlog::spdlog
)

target_include_directories(fitness_tracker_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fitness_tracker_api PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -O2
    )
endif()
