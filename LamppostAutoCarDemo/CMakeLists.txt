# cmake_minimum_required(VERSION <specify CMake version here>)
project(LamppostAutoCarDemo)

cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DRIVER_SRCS
        driver/CC2520.c
        driver/CC2520_Helper.c
        driver/CC2520_HW.c)

set(DRIVER_INCLUDE
        driver/CC2520.h
        driver/CC2520_Helper.h
        driver/CC2520_HW.h)

include_directories(driver)
include_directories(include)

find_library(wiringPi_LIB wiringPi)

add_library(DRIVER_LIB SHARED ${DRIVER_SRCS} ${DRIVER_INCLUDE})
target_link_libraries(DRIVER_LIB ${wiringPi_LIB})

add_executable(ControlNodeMain
        src/CN_Main.cc
        src/CN_CommPanel.cc
        include/CN_CommPanel.hh
        include/ControlNode.hh)
target_link_libraries(ControlNodeMain DRIVER_LIB)
target_include_directories(ControlNodeMain
        PRIVATE driver
        PRIVATE include
        PRIVATE pthread)

add_executable(LamppostMain
        src/LP_Main.cc
        src/LP_CommPanel.cc
        include/LP_CommPanel.hh
        include/Lamppost.hh)
target_link_libraries(LamppostMain DRIVER_LIB)
target_include_directories(LamppostMain
        PRIVATE driver
        PRIVATE include
        PRIVATE pthread)
