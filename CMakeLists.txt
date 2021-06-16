################################################################
# Project Infos
################################################################
project(LamppostAutoCarDemo)
message(${CMAKE_CURRENT_SOURCE_DIR})


################################################################
# Toolchain Standard Specifications
################################################################
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


################################################################
# Project Macros
################################################################
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


################################################################
# Lib: Build CC2520 Driver
################################################################
add_definitions(-L/usr/lib/libwiringPi.so -lwiringPi)
add_library(DRIVER_LIB SHARED ${DRIVER_SRCS} ${DRIVER_INCLUDE})


################################################################
# Exec: Build Control Node Program
################################################################
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


################################################################
# Exec: Build Lamppost Host Program
################################################################
add_executable(LamppostHost
        src/LamppostHostMain.cc
        src/LamppostHostGeneral.cc
        src/LamppostHostRBDetection.cc
        src/LamppostHostCommPanel.cc
        include/LamppostHostCommPanel.hh
        include/LamppostHostGeneral.hh
        include/LamppostHostRBDetection.hh
        include/utils.hh)
target_link_libraries(LamppostHost
        btp
        ipc)
target_include_directories(LamppostHost
        PRIVATE include
        PRIVATE pthread)
