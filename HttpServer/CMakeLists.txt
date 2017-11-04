include_directories(${PROJECT_SOURCE_DIR}/DataStorage)
# include_directories(${PROJECT_SOURCE_DIR}/Submodules/http-parser)
include_directories(${PROJECT_SOURCE_DIR}/HttpParser)

# link_directories(${PROJECT_SOURCE_DIR}/Submodules/http-parser)

file(GLOB HTTP_SERVER_SOURCES HttpServer.cpp HttpServer.h)

add_library(http_server STATIC ${HTTP_SERVER_SOURCES})

target_link_libraries(http_server data_storage Http_Parser)