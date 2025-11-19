# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/tomoview_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/tomoview_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/tests_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/tests_autogen.dir/ParseCache.txt"
  "tests/tests_autogen"
  "tomoview_autogen"
  )
endif()
