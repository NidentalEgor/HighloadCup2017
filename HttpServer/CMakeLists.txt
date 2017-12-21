# add_library(HttpServer STATIC HttpServerBoost.cpp HttpServerBoost.h)
add_library(HttpServer STATIC HttpServer.cpp HttpServer.h RequestProcessor.h RequestProcessor.cpp)

# target_link_libraries(HttpServer DataStorage HttpParser Traceable)
target_link_libraries(HttpServer DataStorage HttpParser Traceable)
