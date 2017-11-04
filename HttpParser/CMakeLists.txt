MESSAGE(EGOR EGOR EGOR ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/Submodules/http-parser)
include_directories(${PROJECT_SOURCE_DIR}/Submodules/libyuarel)
# include_directories(/home/egor/Repositories/highloadcup2017/Submodules/http-parser)

# link_directories(${PROJECT_SOURCE_DIR}/Submodules/http-parser)

add_library(Http_Parser STATIC HttpParser.cpp HttpParser.h
# http_parser.h
# http_parser.c
)

target_link_libraries(Http_Parser http_parser libyuarel)