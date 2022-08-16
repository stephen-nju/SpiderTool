


aux_source_directory(. SPIDER_SRC)
add_executable(spider ${SPIDER_SRC})


include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 1.9.1)
# The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(spider PRIVATE cpr::cpr spdlog::spdlog)
target_link_libraries(spider PRIVATE absl::strings utf8proc)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(spider PUBLIC OpenMP::OpenMP_CXX)
endif()