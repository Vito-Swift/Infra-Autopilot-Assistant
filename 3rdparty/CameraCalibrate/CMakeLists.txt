project(camera_calibration)
cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "/usr/local/include/opencv4")
find_package(OpenCV REQUIRED)

add_executable(calibration_camera calibration_camera.cc calibration_camera.hh)
target_link_libraries(calibration_camera ${OpenCV_LIBS})
