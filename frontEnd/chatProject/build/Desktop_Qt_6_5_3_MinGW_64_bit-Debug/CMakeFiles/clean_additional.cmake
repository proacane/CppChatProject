# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\chatProject_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\chatProject_autogen.dir\\ParseCache.txt"
  "chatProject_autogen"
  )
endif()
