include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(linux)

add_library(lib-ncfg libncfg.cpp)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(lib-ncfg dl)
    target_link_libraries(lib-ncfg mnl)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
