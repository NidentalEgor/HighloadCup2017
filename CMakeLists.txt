cmake_minimum_required (VERSION 3.5)
project (HighloadCup2017)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14 -pthread -rdynamic")

set(Boost_USE_STATIC_LIBS off)
set(Boost_USE_MULTITHREAD on)
find_package(Boost REQUIRED COMPONENTS system thread regex)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

set(BOOST_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

include(CMakeToolsHelpers OPTIONAL)

add_executable(Server ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

add_subdirectory(Submodules)
add_subdirectory(DataStorage)
add_subdirectory(Tests)
add_subdirectory(HttpParser)
#add_subdirectory(DataExtracter)
add_subdirectory(Utils)

TARGET_LINK_LIBRARIES(Server
  DataStorage
  HttpParser
  # DataExtracter
  Traceable
  ${BOOST_LIBS})
