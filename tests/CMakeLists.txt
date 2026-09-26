include(FetchContent)

FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
# Match the project's dynamic CRT (/MD).
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(AionTests ${TEST_SOURCES})
target_link_libraries(AionTests PRIVATE Aion GTest::gtest_main AionWarnings)

include(GoogleTest)
gtest_discover_tests(AionTests DISCOVERY_MODE PRE_TEST)
