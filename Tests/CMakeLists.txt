enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/Submodules/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/DataStorage)

file(GLOB TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

add_executable(Tests ${TEST_SOURCES})

target_link_libraries(Tests GoogleTest data_storage)