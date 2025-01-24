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

if(NOT DEFINED EBUS_NAMESPACE)
  set(EBUS_NAMESPACE "_ebus_")
endif()

if (NOT DEFINED INTRUSIVE_NAMESPACE)
  set(INTRUSIVE_NAMESPACE ${EBUS_NAMESPACE})
endif()


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(ebus
  src/task/task_worker.cc
  src/task/task_scheduler.cc
)

target_include_directories(ebus
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_definitions(ebus PUBLIC
  EBUS_NS=${EBUS_NAMESPACE}
  INTRUSIVE_NS=${INTRUSIVE_NAMESPACE})


if (${EBUS_ENABLE_TESTING})
  add_subdirectory(test)
endif()
