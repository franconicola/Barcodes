cmake_minimum_required(VERSION 3.13)
project(Barcodes)
find_package(OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 14)
add_executable(Barcodes src/main.cpp src/Barcodes.cpp)

TARGET_LINK_LIBRARIES (Barcodes ${OpenCV_LIBS})
include_directories (include/)