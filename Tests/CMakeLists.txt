enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/Submodules/googletest/googletest/include)

add_executable(Tests
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

target_link_libraries(Tests GoogleTest DataStorage)