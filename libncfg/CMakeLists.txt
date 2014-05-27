include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(linux)

add_library(ncfg libncfg.cpp)
add_library(ncfg-common SHARED common.cpp)

target_link_libraries(ncfg ncfg-common)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #find_package(PkgConfig)
    #pkg_search_module(LIBNL REQUIRED libnl-3.0)
    #include_directories(${LIBNL_INCLUDE_DIRS})
    #target_link_libraries(lib-ncfg ${LIBNL_LIBRARIES})
    #set_target_properties(lib-ncfg PROPERTIES
        #COMPILE_FLAGS ${LIBNL_CFLAGS})

    target_link_libraries(ncfg dl)
    target_link_libraries(ncfg mnl)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
