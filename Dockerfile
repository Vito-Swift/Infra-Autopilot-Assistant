#############################################
# Fetch base docker image
#############################################
FROM ubuntu:18.04
SHELL ["/bin/bash", "-c"]

#############################################
# Install essential packages
#############################################
RUN apt update && apt upgrade -y
RUN apt install -y ca-certificates
RUN echo $'deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-updates main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-backports main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-security main restricted universe multiverse\n' \
             | tee -a /etc/apt/sources.list
RUN apt update && apt upgrade -y
RUN apt install -y apt-utils gcc g++ openssh-server cmake build-essential gdb gdbserver rsync vim git

#############################################
# Configure sshd
#############################################
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN echo "PermitRootLogin yes" >> /etc/ssh/sshd_config
RUN echo "StrictHostKeyChecking accept-new" >> /etc/ssh/ssh_config
RUN sed  's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
RUN echo "export VISIBLE=now" >> /etc/profile

#############################################
# Open ports
#############################################
EXPOSE 22 7777

#############################################
# Add debugger user for Clion Remote Dev
#############################################
RUN useradd -ms /bin/bash debugger
RUN echo 'debugger:pwd' | chpasswd

#############################################
# Setup SSH Keys to access Github Private Repo
#############################################
RUN mkdir -p /root/.ssh
ADD ./id_rsa /root/.ssh/id_rsa


#############################################
# Build and install coding library
#############################################
#WORKDIR /bats-code-library-workdir
#RUN git clone git@github.com:batsiot/bats-code-library-2016
#RUN cd bats-code-library-2016 && mkdir build && cd build && cmake .. && make && make install


#############################################
# Build and install OpenCV
#############################################
WORKDIR /opencv-build-workdir
RUN apt-get update \
    && apt-get install -y \
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
        libpq-dev
RUN apt install -y python3
RUN apt install -y python3-pip
RUN pip3 install numpy
WORKDIR /
ENV OPENCV_VERSION="4.1.0"
RUN wget https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip \
&& unzip ${OPENCV_VERSION}.zip \
&& rm ${OPENCV_VERSION}.zip
RUN wget https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip \
&& unzip ${OPENCV_VERSION}.zip \
&& mkdir /opencv-${OPENCV_VERSION}/cmake_binary \
&& cd /opencv-${OPENCV_VERSION}/cmake_binary \
&& cmake -DBUILD_TIFF=ON \
  -DBUILD_opencv_java=OFF \
  -DOPENCV_EXTRA_MODULES_PATH=/opencv_contrib-${OPENCV_VERSION}/modules \
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
&& make -j4 && make install \
&& rm /${OPENCV_VERSION}.zip \
&& rm -r /opencv-${OPENCV_VERSION}


#############################################
# Build and install batspro2
#############################################
WORKDIR /bats-protocol-workdir
RUN apt install -y libboost-all-dev
RUN apt install -y iproute2
RUN apt install -y htop
RUN DEBIAN_FRONTEND="noninteractive" apt install -y expect iperf3
RUN git clone git@github.com:batsiot/batspro2
RUN cd batspro2 && dpkg -i libbats-0.1.3-Linux-amd64.deb
RUN cd batspro2 && echo $'install (TARGETS btp ipc DESTINATION lib)\n\
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
                                        DESTINATION include)' | tee -a /bats-protocol-workdir/batspro2/BTP/CMakeLists.txt
RUN sed -i 's/LOG_MSQ_MAX_MSGS\s16/LOG_MSQ_MAX_MSGS 10/g' batspro2/Utilities/BATSLogger/src/BATSLogger.h
RUN cd batspro2 && mkdir build && cd build && cmake .. && make -j4 && make install


#############################################
# Enable SSH
#############################################
CMD ["/usr/sbin/sshd", "-D"]


#############################################
# Enable BATS network simulation
#############################################
# CMD ["/bats-protocol-workdir/batspro2/Utilities/simulation/bmsim_ipv4.sh", "start", "4"]
# A virtual BATS network is established at the start of the docker container
#   Node 0: 0.0.1.0 <-> Node 1: 0.0.1.1 <-> Node 2: 0.0.1.2 <-> Node 3: 0.0.1.3
