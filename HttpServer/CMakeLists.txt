add_library(HttpServer STATIC HttpServer.cpp HttpServer.h)

target_link_libraries(HttpServer DataStorage HttpParser Traceable)