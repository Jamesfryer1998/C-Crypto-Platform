file(GLOB SOURCES main.cpp)
add_executable(crypto-platform-app ${SOURCES})
target_link_libraries(crypto-platform-app crypto-platform)
