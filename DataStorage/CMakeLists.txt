cmake_minimum_required(VERSION 3.5)

set(DATA_STORAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# MESSAGE(STATUS "EGOR EGOR EGOR LIST OF DIRS")
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

add_library(data_storage STATIC ${DATA_STORAGE_SOURCES})