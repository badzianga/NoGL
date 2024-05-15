cmake_minimum_required(VERSION 3.26)
project(NoGL C)

set(CMAKE_C_STANDARD 11)

add_library(NoGL STATIC src/NoGL.c
        src/NoUtils.c)
target_include_directories(NoGL PRIVATE include)
target_link_libraries(NoGL m X11)

add_executable(App main.c)
target_include_directories(App PRIVATE include)
target_link_libraries(App NoGL)
