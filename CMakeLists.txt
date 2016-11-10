cmake_minimum_required(VERSION 2.8)

project(texas)

set(CMAKE_C_FLAGS_RELEASE "-Wall -O2")
set(CMAKE_C_FLAGS_DEBUG "-g")

include_directories(include)

# common lib

add_library(poker
        src/poker.cpp
        src/printer.cpp
        src/strategy.cpp
        src/table.cpp
)

# lib test

add_executable(test.exe
        src/test.cpp
        src/poker.test.cpp
)
target_link_libraries(test.exe poker cppunit)

# simulator and tools

add_executable(sim.exe src/sim.cpp)
target_link_libraries(sim.exe poker)

add_executable(dump.exe src/dump.cpp)
target_link_libraries(dump.exe poker)

add_executable(dat2range.exe src/dat2range.cpp)
target_link_libraries(dat2range.exe poker)

add_executable(sim_hu.exe src/sim_hu.cpp)
target_link_libraries(sim_hu.exe poker)


# test

enable_testing()

add_test(poker_unittests test.exe)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
  )

add_dependencies(check test.exe)

set_tests_properties(
        poker_unittests
        PROPERTIES FAIL_REGULAR_EXPRESSION "FAILURE!")

# sim

set(table_path ${CMAKE_CURRENT_SOURCE_DIR}/table)
set(sim_exe ${CMAKE_BINARY_DIR}/sim.exe)
set(dump_exe ${CMAKE_BINARY_DIR}/dump.exe)
set(test_dat ${CMAKE_BINARY_DIR}/test.dat)

add_custom_target(sim_calling_station
        COMMAND ./sim.exe ${table_path}/calling_station 100000 test.dat && ./dump.exe test.dat
)
add_dependencies(sim_calling_station sim.exe dump.exe)

add_custom_target(sim_tag
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && ${sim_exe} table/tag 100000 ${test_dat} && ${dump_exe} ${test_dat}
)
add_dependencies(sim_tag sim.exe dump.exe)

add_custom_target(sim_hu
        COMMAND ./sim_hu.exe KsKd-3s8cQs 500000
)
add_dependencies(sim_hu sim_hu.exe)



