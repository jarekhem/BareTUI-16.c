cmake_minimum_required(VERSION 3.15)

add_library(${PROJECT_NAME}-SIM
        src/main.c
        src/MainScreen.c
)

target_include_directories(${PROJECT_NAME}-SIM PUBLIC inc)
target_link_libraries(${PROJECT_NAME}-SIM PUBLIC ${PROJECT_NAME})

define_filename_macro(${PROJECT_NAME}-SIM)

add_subdirectory(platform)
