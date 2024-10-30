cmake_minimum_required(VERSION 3.5.0)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

add_executable(demo main.cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)