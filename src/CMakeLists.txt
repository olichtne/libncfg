include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(lib-ncfg libncfg.cpp)

add_executable(main main.cpp)
target_link_libraries(main lib-ncfg)
