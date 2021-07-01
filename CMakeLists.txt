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
set(OpenCV_DIR "/usr/local/include/opencv4")
find_package(Threads REQUIRED)
find_package(Boost REQUIRED regex date_time system)
find_package(OpenCV)
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
        src/Control/CtrlMain.cc
        src/Control/CtrlCommPanel.cc
        include/Control/CtrlCommPanel.hh
        include/Control/CtrlGeneral.hh)
target_link_libraries(ControlNodeMain DRIVER_LIB)
target_include_directories(ControlNodeMain
        PRIVATE driver
        PRIVATE include
        PRIVATE pthread)


################################################################
# Exec: Build Lamppost Host Program
################################################################
add_executable(LamppostHost
        src/LPHost/LamppostHostMain.cc
        src/LPHost/LamppostHostGeneral.cc
        src/LPHost/LamppostHostRBDetection.cc
        src/LPHost/LamppostHostCommPanel.cc
        src/utils.cc
        include/LPHost/LamppostHostCommPanel.hh
        include/LPHost/LamppostHostGeneral.hh
        include/LPHost/LamppostHostRBDetection.hh
        include/LPHost/LamppostHostUtils.hh
        include/utils.hh)
target_link_libraries(LamppostHost
        btp
        ipc
        ${Boost_LIBRARIES}
        ${OpenCV_LIBS}
        aruco
        marker_mapper
        Threads::Threads)
target_include_directories(LamppostHost
        PRIVATE include
        PRIVATE pthread
        PRIVATE "/usr/local/include/aruco"
        PRIVATE "/usr/local/include/maker_mapper")

################################################################
# Exec: Build Lamppost Hook Program
################################################################
add_executable(LamppostHook
        src/LPHook/LamppostHookMain.cc
        src/LPHook/LamppostHookCommPanel.cc
        src/LPHook/LamppostHookGeneral.cc
        src/utils.cc
        include/LPHook/LamppostHookCommPanel.hh
        include/LPHook/LamppostHookGeneral.hh
        include/LPHook/LamppostHookUtils.hh)
target_link_libraries(LamppostHook
        DRIVER_LIB
        ${Boost_LIBRARIES}
        Threads::Threads)
target_include_directories(LamppostHook
        PRIVATE driver
        PRIVATE include
        PRIVATE pthread)