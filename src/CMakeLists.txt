include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(linux)

add_library(lib-ncfg libncfg.cpp)
