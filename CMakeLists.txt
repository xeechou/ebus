cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(ebus
  VERSION 0.2
  LANGUAGES C CXX)

##### Options
option(CMAKE_EXPORT_COMPILE_COMMANDS "generating compile commands when possible" ON)

##### settings
set(CMAKE_CXX_STANDARD 20) # for concepts
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")
option(EBUS_ENABLE_TESTING "enable testing" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(src)

if (${EBUS_ENABLE_TESTING})
  add_subdirectory(test)
endif()
