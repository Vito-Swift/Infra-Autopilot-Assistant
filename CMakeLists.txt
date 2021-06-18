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
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED regex date_time system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")


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
        src/utils.cc
        include/LamppostHostCommPanel.hh
        include/LamppostHostGeneral.hh
        include/LamppostHostRBDetection.hh
        include/LamppostHostUtils.hh
        include/utils.hh)
target_link_libraries(LamppostHost
        btp
        ipc
        ${Boost_LIBRARIES}
        Threads::Threads)
target_include_directories(LamppostHost
        PRIVATE include
        PRIVATE pthread)
