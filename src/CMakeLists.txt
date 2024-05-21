add_library(ebus
  ebus/ebus.cc
  task/task_worker.cc
)
target_include_directories(ebus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
