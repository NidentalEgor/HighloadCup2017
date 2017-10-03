cmake_minimum_required (VERSION 3.1)
project (HighloadCup2017)

set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14 -pthread")

include_directories(${SERVER_DIR}/DataStorage)
include_directories(${SERVER_DIR}/Entities)
include_directories(${SERVER_DIR}/HttpServer)
include_directories(${SERVER_DIR}/Utils)
include_directories(${SERVER_DIR}/Submodules/rapidjson/include)

# SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

file(GLOB SERVER_SOURCES
    ${SERVER_DIR}/*.cpp
    ${SERVER_DIR}/DataStorage/*.cpp
    ${SERVER_DIR}/Entities/*.cpp
    ${SERVER_DIR}/HttpServer/*.cpp
    ${SERVER_DIR}/Utils/*.h)

add_executable(server ${SERVER_SOURCES})

add_definitions(-DRAPIDJSON_HAS_CXX11_RVALUE_REFS)
add_subdirectory(Submodules)
# add_subdirectory(HttpServer)
# add_subdirectory(Entities)
# add_subdirectory(DataStarage)

# set_target_properties(server PROPERTIES
# CXX_STANDARD 17
# CXX_STANDARD_REQUIRED YES
# CXX_EXTENSIONS NO
# )