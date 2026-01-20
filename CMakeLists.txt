cmake_minimum_required(VERSION 3.5)
project(json)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-sign-compare -Wno-ambiguous-reversed-operator")

Include(FetchContent)

# We depend on system, so import it.
#
FetchContent_Declare(
        system
        GIT_REPOSITORY https://github.com/IanCollins/system.git
        GIT_TAG        v0.4.0
)

# We want to statically link system.
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(system)

add_library(System::System INTERFACE IMPORTED)

target_link_libraries(System::System INTERFACE System)

include_directories(
        include/json
        ${system_SOURCE_DIR}/include
)

file(GLOB json_sources CONFIGURE_DEPENDS *.cc)
file(GLOB json_headers CONFIGURE_DEPENDS include/json/*.h)

add_library(Json STATIC ${json_sources})
add_library(masuma::Json ALIAS Json)

install(TARGETS Json DESTINATION lib)
install(FILES ${json_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

enable_testing()
add_subdirectory(tests)
