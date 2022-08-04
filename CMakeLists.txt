cmake_minimum_required(VERSION 3.15)
project(spiderTool VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(cmake/BuildLocation.cmake)
include(cmake/BuildType.cmake)
include(cmake/Common.cmake)
# 该变量放在add_subdirectory()src前面
set(3RDPARTY_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libnlpcxx/include    
)
include_directories(${3RDPARTY_INCLUDE_DIR})


add_subdirectory(src)
add_subdirectory(3rdparty)

