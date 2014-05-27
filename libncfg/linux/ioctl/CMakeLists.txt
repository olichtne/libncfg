include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB files "*.cpp")

foreach(file ${files})
    string(REGEX MATCH "[^/]*.cpp" lib_name ${file})
    string(REPLACE ".cpp" "" lib_name ${lib_name})
    add_library(${lib_name} SHARED ${file})
    target_link_libraries(${lib_name} ncfg-common)
endforeach()
