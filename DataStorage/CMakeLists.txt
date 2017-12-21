file(GLOB DATA_STORAGE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/*.h)

add_library(DataStorage STATIC ${DATA_STORAGE_SOURCES})

target_link_libraries(DataStorage Traceable Zipper-static)