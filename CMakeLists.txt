cmake_minimum_required (VERSION 3.5)
project (HighloadCup2017)

set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14 -pthread")

# include(CMakeToolsHelpers OPTIONAL)

# temp
include_directories(${SERVER_DIR}/DataStorage)
include_directories(${SERVER_DIR}/Submodules/rapidjson/include)
include_directories(${SERVER_DIR}/Utils)
# temp

# include_directories(${SERVER_DIR}/HttpServer)

file(GLOB SERVER_SOURCES ${SERVER_DIR}/*.cpp)

add_executable(Server ${SERVER_SOURCES})

add_subdirectory(Submodules)
add_subdirectory(DataStorage)
add_subdirectory(Tests)

target_link_libraries(Server data_storage)


