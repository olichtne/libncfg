cmake_minimum_required(VERSION 2.8)
project(lib-ncfg)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g")

add_subdirectory(include)
add_subdirectory(src)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(main main.cpp)
target_link_libraries(main lib-ncfg)
