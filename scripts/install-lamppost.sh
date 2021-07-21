#!/bin/bash

pwd=`pwd`
script_path="$(dirname "$0")"

# Build OpenCV
sudo cp /usr/lib/python3/dist-packages/lsb_release.py /usr/local/lib/python3.6/ && \
cd ~ && mkdir OPENCV_BUILD && cd OPENCV_BUILD && \
sudo apt update && sudo apt upgrade -y && sudo apt install -y \
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
sudo apt install -y python3 && \
sudo apt install -y python3-pip && \
sudo pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple numpy && \
export OPENCV_VERSION="4.1.0" && \
wget https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip \
        && unzip ${OPENCV_VERSION}.zip \
        && rm ${OPENCV_VERSION}.zip && \
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
        && rm -r opencv-${OPENCV_VERSION} && \

# install aruco marker and marker mapper libraries
cd $script_path && cd ../3rdparty/aruco/ && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/ && make -j4 && sudo make install && \
sudo echo "/usr/local/lib" >> /etc/ld.so.conf.d/aruco.conf && sudo ldconfig && \
cd $script_path && cd ../3rdparty/marker_mapper && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && make -j4 && sudo make install && \
sudo ldconfig

# install BATS Pro2
cd ~ && \
sudo apt install -y libboost-all-dev && \
git clone github.com/batsiot/batspro2.git && \
cd batspro2 && dpkg -i libbats-0.1.3-Linux-amd64.deb && echo $'install (TARGETS btp ipc DESTINATION lib)\n\
                         install (FILES include/BCMPPacket.h \
                                        include/BPPacket.h \
                                        include/BTPPacket.h \
                                        include/INIReader.h \
                                        include/Queue.h \
                                        include/Thread.h \
                                        include/ThreadPool.h \
                                        include/bats-protocol.h \
                                        Socket/include/BATSSocket.h \
                                        Socket/include/BPSocket.h \
                                        Socket/include/IPC_UDS.h \
                                        DESTINATION include)' | tee -a BTP/CMakeLists.txt && \
sed -i 's/LOG_MSQ_MAX_MSGS\s16/LOG_MSQ_MAX_MSGS 10/g' Utilities/BATSLogger/src/BATSLogger.h && \
mkdir build && cd build && cmake .. && make -j4 && sudo make install && \

# install mysql connector
sudo apt install -y libmysqlcppconn-dev && \

cd $script_path
