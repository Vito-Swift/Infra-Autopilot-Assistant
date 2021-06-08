# cmake_minimum_required(VERSION <specify CMake version here>)
project(LamppostAutoCarDemo)
message(${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.10)

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

add_definitions(-L/usr/lib/libwiringPi.so -lwiringPi)

add_library(DRIVER_LIB SHARED ${DRIVER_SRCS} ${DRIVER_INCLUDE})

add_executable(ControlNodeMain
        src/CN_Main.cc
        src/CN_CommPanel.cc
        include/CN_CommPanel.hh
        include/CN_General.hh)
target_link_libraries(ControlNodeMain DRIVER_LIB)
target_include_directories(ControlNodeMain
        PRIVATE driver
        PRIVATE include
        PRIVATE pthread)

add_executable(LamppostHost
        src/LamppostHostMain.cc
        src/LamppostHostGeneral.cc
        src/LamppostHostRBDetection.cc
        include/LamppostHostCommPanel.hh
        include/LamppostHostGeneral.hh
        include/LamppostHostRBDetection.hh
        include/utils.hh)
target_include_directories(LamppostHost
        PRIVATE include
        PRIVATE pthread)
