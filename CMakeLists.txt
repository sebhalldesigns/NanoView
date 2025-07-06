cmake_minimum_required(VERSION 3.10)

project(NanoView)

add_library(NanoView STATIC 
    lib/nanoview.c
    views/nkdockview/nkdockview.c
)

target_include_directories(NanoView PUBLIC
    lib
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(NanoView PUBLIC
    NanoDraw
)
