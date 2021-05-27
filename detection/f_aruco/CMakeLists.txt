# ----------------------------------------------------------------------------
#   Basic Configuration
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)
project(f_aruco LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON) #...with compiler extensions like gnu++11

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )

find_package( OpenCV REQUIRED )
set(aruco_DIR "/usr/local/share/aruco/cmake")
find_package( aruco REQUIRED )

# If could not find could not find a package configuration file provided by "aruco", 
# set the "aruco_INCLUDE_DIRS" and the "aruco_LIBS" by mabual setting directly.
set(aruco_INCLUDE_DIRS  /usr/local/include/aruco)
set(aruco_LIBS  /usr/local/lib/libaruco.so)

include_directories( include ${OpenCV_INCLUDE_DIRS} ${aruco_INCLUDE_DIRS})

add_executable( cam_pose_estimator test/cam_pose_estimator.cpp )
target_link_libraries( cam_pose_estimator ${OpenCV_LIBS} ${aruco_LIBS} )

