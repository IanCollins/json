cmake_minimum_required(VERSION 3.5)
project(json)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare -Wno-ambiguous-reversed-operator")
Include(FetchContent)

# We depend on masuma-system, so import it.
#
FetchContent_Declare(
        masuma-system
        GIT_REPOSITORY https://github.com/IanCollins/system.git
        GIT_TAG        v0.3.0
)

# HTTP only, no async curl.
set(INFLUXCXX_WITH_BOOST OFF CACHE INTERNAL "No boost")

# We want to statically link masuma-system.
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(masuma-system)

add_library(masuma-system::masuma-system INTERFACE IMPORTED)

target_link_libraries(masuma-system::masuma-system INTERFACE InfluxDB)

include_directories(
        ${masuma-system_SOURCE_DIR}/include
)

file(GLOB json_sources CONFIGURE_DEPENDS *.cc)
file(GLOB json_headers CONFIGURE_DEPENDS *.h)

enable_testing()
add_subdirectory(tests)

add_library(json STATIC ${json_sources} ${json_headers})

install(TARGETS json DESTINATION lib)
install(FILES ${json_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
