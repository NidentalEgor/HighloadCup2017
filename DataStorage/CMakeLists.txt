cmake_minimum_required(VERSION 3.5)

set(DATA_STORAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

# target_include_directories(data_storage ${PROJECT_SOURCE_DIR}/Submodules/rapidjson/include)
# target_include_directories(data_storage ${PROJECT_SOURCE_DIR}/Utils)

file(GLOB DATA_STORAGE_SOURCES 
  ${DATA_STORAGE_DIR}/*.cpp
  ${DATA_STORAGE_DIR}/*.h
  ${DATA_STORAGE_DIR}/Entities/*.cpp
  ${DATA_STORAGE_DIR}/Entities/*.h)

# include_directories(${PROJECT_SOURCE_DIR}/Submodules/http-parser)
# link_directories(${PROJECT_SOURCE_DIR}/Submodules/zipper)

add_library(data_storage STATIC ${DATA_STORAGE_SOURCES})

target_link_libraries(data_storage Traceable Zipper-static)