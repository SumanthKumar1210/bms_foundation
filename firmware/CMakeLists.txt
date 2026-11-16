cmake_minimum_required(VERSION 3.16)
project(bms_firmware C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)

# Application library (target independent). List every source explicitly.
add_library(bms STATIC
    src/balancing.c
    src/bms_app.c
    src/can_protocol.c
    src/cell_monitor.c
    src/contactor.c
    src/crc8.c
    src/fault_manager.c
    src/pyro_fuse.c
)
target_include_directories(bms PUBLIC include)

# Host unit tests: each test links the library and the HAL stub.
enable_testing()
set(BMS_TESTS
    test_balancing
    test_can_protocol
    test_cell_monitor
    test_contactor
    test_fault_manager
    test_pyro_fuse
)
foreach(test_name IN LISTS BMS_TESTS)
    add_executable(${test_name} test/${test_name}.c test/hal_stub.c)
    target_include_directories(${test_name} PRIVATE test)
    target_link_libraries(${test_name} PRIVATE bms)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
