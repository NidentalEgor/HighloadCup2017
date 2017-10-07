set(HTTP_SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/DataStorage)

file(GLOB HTTP_SERVER_SOURCES ${SERVER_DIR}/*.cpp ${SERVER_DIR}/*.h)

add_library(http_server STATIC ${HTTP_SERVER_SOURCES})

target_link_libraries(http_server data_storage)