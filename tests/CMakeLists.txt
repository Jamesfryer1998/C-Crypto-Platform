file(GLOB SOURCES *.cpp)
add_executable(test-crypto-platform ${SOURCES})
target_link_libraries(test-crypto-platform gtest_main)
target_link_libraries(test-crypto-platform crypto-platform)
include(GoogleTest)
gtest_discover_tests(test-crypto-platform)