cmake_minimum_required(VERSION 3.24)
project(unvr-carousal VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Multimedia Test OpenGLWidgets)
find_package(OpenSSL REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
