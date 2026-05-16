cmake_minimum_required(VERSION 3.10)
project(WindowCPlusPlus)

set(CMAKE_CXX_STANDARD 17)

# IMPORTANT: help CMake find Homebrew OpenCV
#set(OpenCV_DIR "/opt/homebrew/opt/opencv/lib/cmake/opencv4")
set(OpenCV_DIR "../packages/opencv2.framework/Versions/A/lib/cmake/opencv4")
# (change this if brew prefix shows /usr/local instead)

find_package(OpenCV REQUIRED)

add_executable(my_app
    src/main.cpp
    src/headtracking.cpp
    src/functions.cpp
)

target_link_libraries(my_app ${OpenCV_LIBS})