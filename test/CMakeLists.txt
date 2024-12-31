###############################################################################
# Download Catch2
###############################################################################
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

###############################################################################
# sample test1
###############################################################################

add_executable(test1 test1.cc)
#in most case, Catch2WithMain is what you need, the docs about custom main:
#https://github.com/catchorg/Catch2/blob/devel/docs/own-main.md
target_link_libraries(test1 PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(test1)


add_executable(test_ebus test_ebus.cc)
target_link_libraries(test_ebus PRIVATE Catch2::Catch2WithMain ebus)
catch_discover_tests(test_ebus)

add_executable(test_task test_task.cc)
target_link_libraries(test_task PRIVATE Catch2::Catch2WithMain ebus)
catch_discover_tests(test_task)

add_executable(test_event test_event.cc)
target_link_libraries(test_event PRIVATE Catch2::Catch2WithMain ebus)
catch_discover_tests(test_event)

add_executable(test_ebus_ref test_ebus_ref.cc)
target_link_libraries(test_ebus_ref PRIVATE Catch2::Catch2WithMain ebus)
catch_discover_tests(test_ebus_ref)


add_executable(test_task_oneshot test_task_oneshot.cc util.cc)
target_link_libraries(test_task_oneshot PRIVATE Catch2::Catch2WithMain ebus)
catch_discover_tests(test_task_oneshot)

add_executable(test_task_rescheduable test_task_reschedule.cc util.cc)
target_link_libraries(test_task_rescheduable PRIVATE Catch2::Catch2WithMain
  ebus)
catch_discover_tests(test_task_rescheduable)

#then I can simply run `ctest --test-dir build/test` to run the tests
