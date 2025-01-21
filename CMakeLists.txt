cmake_minimum_required(VERSION 3.27)
project(infotecs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


add_library(lib SHARED src/lib/lib.cpp)

add_executable(${PROJECT_NAME}_1 src/program1/main.cpp)
add_executable(${PROJECT_NAME}_2 src/program2/main.cpp)
target_link_libraries(${PROJECT_NAME}_1 lib)
target_link_libraries(${PROJECT_NAME}_2 lib)