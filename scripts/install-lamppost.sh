#!/bin/bash

pwd=`pwd`
script_path="$(dirname "$0")"

# Build OpenCV
cd ~ && mkdir OPENCV_BUILD && cd OPENCV_BUILD
sudo apt update && sudo apt install -y \
        build-essential \
        cmake \
        git \
        wget \
        unzip \
        yasm \
        pkg-config \
        libswscale-dev \
        libtbb2 \
        libtbb-dev \
        libjpeg-dev \
        libpng-dev \
        libtiff-dev \
        libavformat-dev \
        libpq-dev \
        libgtk2.0-dev \
        pkg-config
sudo apt install -y python3
sudo apt install -y python3-pip
sudo pip3 install numpy
export OPENCV_VERSION="4.1.0"
wget https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip \
        && unzip ${OPENCV_VERSION}.zip \
        && rm ${OPENCV_VERSION}.zip
wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip \
        && unzip ${OPENCV_VERSION}.zip \
        && mkdir opencv-${OPENCV_VERSION}/cmake_binary \
        && cd opencv-${OPENCV_VERSION}/cmake_binary \
        && cmake -DBUILD_TIFF=ON \
          -DBUILD_opencv_java=OFF \
          -DOPENCV_EXTRA_MODULES_PATH=~/OPENCV_BUILD/opencv_contrib-${OPENCV_VERSION}/modules \
          -DWITH_CUDA=OFF \
          -DWITH_OPENGL=ON \
          -DWITH_OPENCL=ON \
          -DWITH_IPP=ON \
          -DWITH_TBB=ON \
          -DWITH_EIGEN=ON \
          -DWITH_V4L=ON \
          -DBUILD_TESTS=OFF \
          -DBUILD_PERF_TESTS=OFF \
          -DCMAKE_BUILD_TYPE=RELEASE \
          .. \
        && make -j4 && sudo make install \
        && rm ${OPENCV_VERSION}.zip \
        && rm -r opencv-${OPENCV_VERSION}

# install aruco marker and marker mapper libraries
cd $script_path && cd ../3rdparty/aruco/ && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/ && make -j4 && sudo make install
sudo echo "/usr/local/lib" >> /etc/ld.so.conf.d/aruco.conf && sudo ldconfig
cd $script_path && cd ../3rdparty/marker_mapper && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && make -j4 && sudo make install
sudo ldconfig

# install mysql connector
sudo apt install -y libmysqlcppconn-dev

cd $script_path
