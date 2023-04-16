file(GLOB SOURCES *.cpp)
add_library(crypto-platform ${SOURCES})
target_include_directories(crypto-platform PUBLIC ..)