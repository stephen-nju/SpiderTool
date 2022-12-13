cmake_minimum_required(VERSION 3.15)
project(spider VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SPIDER_INSTALL "generate install project spider " ON)

include(cmake/BuildLocation.cmake)
include(cmake/BuildType.cmake)
include(cmake/Common.cmake)

if(${SPIDER_INSTALL})
    set(SPDLOG_INSTALL ON)
endif()

# 该变量放在add_subdirectory()src前面
set(3RDPARTY_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libnlpcxx/include
)
include_directories(${3RDPARTY_INCLUDE_DIR})

add_subdirectory(3rdparty)
add_subdirectory(src)

if(SPIDER_INSTALL)
    message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # 将库的相关信息输出为spiderConfig.cmake，供find_package查找
    install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION "lib/cmake/${PROJECT_NAME}")

    configure_package_config_file(Config.cmake.in
        ${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}")

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION "lib/cmake/${PROJECT_NAME}")
endif()