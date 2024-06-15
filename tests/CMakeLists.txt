cmake_minimum_required(VERSION 3.10.2)
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(tests main.cpp command_parser.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../googletest/googlemock/include/" "${GTEST_INCLUDE_DIRS}" "${CMAKE_CURRENT_LIST_DIR}/../include/")
target_link_libraries(tests gtest_main gmock_main)