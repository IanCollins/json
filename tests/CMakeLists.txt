include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

target_link_libraries(GTest::GTest INTERFACE gtest_main)

include_directories("..")

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_executable(testJSON TestJSON.cc TestAlgorithm.cc ${json_sources})

set(CMAKE_LIBRARY_PATH /opt/build/lib)

#add_library(utils STATIC IMPORTED)
#set_target_properties(utils PROPERTIES IMPORTED_LOCATION ${CMAKE_LIBRARY_PATH}/libsystem.a )

target_link_libraries(testJSON PRIVATE
        GTest::GTest
        System::System
        Json
        $<$<PLATFORM_ID:SunOS>:socket>
        $<$<PLATFORM_ID:SunOS>:nsl>)

add_test(gtestJSON, testJSON)
