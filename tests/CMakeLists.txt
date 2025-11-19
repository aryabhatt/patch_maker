

include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(tests tests.cpp)
target_link_libraries(tests TIFF::TIFF)
