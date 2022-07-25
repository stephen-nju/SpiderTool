cmake_minimum_required(VERSION 3.15)
project(spiderTool VERSION 0.1.0)

include(cmake/BuildLocation.cmake)
include(cmake/BuildType.cmake)
include(cmake/Common.cmake)
add_subdirectory(src)
add_subdirectory(3rdparty)
include_directories(3rdparty/spdlog/include)