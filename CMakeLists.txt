cmake_minimum_required(VERSION 3.5)
project(json)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare -Wno-ambiguous-reversed-operator")
Include(FetchContent)

# We depend on influxdb-cxx, so import it.
#
FetchContent_Declare(
        masuma-system
        GIT_REPOSITORY https://github.com/offa/influxdb-cxx
        GIT_TAG        v0.6.7
)

# HTTP only, no async curl.
set(INFLUXCXX_WITH_BOOST OFF CACHE INTERNAL "No boost")

# We want to statically link influxdb-cxx.
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(influxdb-cxx)

include_directories("${CMAKE_INSTALL_PREFIX}/include/masuma")

file(GLOB json_sources CONFIGURE_DEPENDS *.cc)
file(GLOB json_headers CONFIGURE_DEPENDS *.h)

enable_testing()
add_subdirectory(tests)

add_library(json STATIC ${json_sources} ${json_headers})

install(TARGETS json DESTINATION lib)
install(FILES ${json_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
