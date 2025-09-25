cmake_minimum_required(VERSION 3.16)
project(TomoView)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(TIFF REQUIRED)
find_package(HDF5 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tomoview
    src/main.cpp
    src/image_viewer.cpp
    src/main_window.cpp
)

target_link_libraries(tomoview
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    TIFF::TIFF
    HDF5::HDF5
)

option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
