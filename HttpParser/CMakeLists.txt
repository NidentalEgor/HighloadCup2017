add_library(HttpParser STATIC HttpParser.cpp HttpParser.h)

target_link_libraries(HttpParser ExternalHttpParser Libyuarel QsParse)