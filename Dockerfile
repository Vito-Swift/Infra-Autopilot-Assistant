FROM ubuntu:18.04
SHELL ["/bin/bash", "-c"]

# Essential Packages
RUN apt update && apt upgrade -y
RUN apt install -y ca-certificates
RUN echo $'deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-updates main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-backports main restricted universe multiverse\n\
            deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ bionic-security main restricted universe multiverse\n' \
             | tee -a /etc/apt/sources.list
RUN apt update && apt upgrade -y
RUN apt install -y apt-utils gcc g++ openssh-server cmake build-essential gdb gdbserver rsync vim git

# Configure sshd
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN echo "PermitRootLogin yes" >> /etc/ssh/sshd_config
RUN echo "StrictHostKeyChecking accept-new" >> /etc/ssh/ssh_config
RUN sed  's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
RUN echo "export VISIBLE=now" >> /etc/profile

# Open port 22 for SSH and 7777 for debugger
EXPOSE 22 7777

# Add debugger user for Clion Remote Dev
RUN useradd -ms /bin/bash debugger
RUN echo 'debugger:pwd' | chpasswd

# Setup SSH Keys to access Github Private Repo
RUN mkdir -p /root/.ssh
ADD ./id_rsa /root/.ssh/id_rsa

# Build and install batscodelib
#WORKDIR /bats-code-library-workdir
#RUN git clone git@github.com:batsiot/bats-code-library-2016
#RUN cd bats-code-library-2016 && mkdir build && cd build && cmake .. && make && make install

# Build batspro2 
WORKDIR /bats-protocol-workdir
RUN apt install -y libboost-all-dev
RUN apt install -y iproute2
RUN apt install -y python3
RUN apt install -y python3-pip
RUN DEBIAN_FRONTEND="noninteractive" apt install -y expect iperf3
RUN git clone git@github.com:batsiot/batspro2
RUN cd batspro2 && dpkg -i libbats-0.1.3-Linux-amd64.deb
RUN cd batspro2 && mkdir build && cd build && cmake .. && make

# Build OpenCV
WORKDIR /opencv-build-workdir
RUN apt install -y pkg-config python-dev python-opencv libopencv-dev ffmpeg  \
                       libjpeg-dev libpng-dev libtiff-dev opencv-data libgtk2.0-dev \
                       python-numpy python-pycurl libatlas-base-dev gfortran webp \
                       python-opencv qt5-default libvtk6-dev zlib1g-dev
RUN apt install -y unzip
RUN wget https://github.com/opencv/opencv/archive/3.0.0.zip
RUN unzip 3.0.0.zip && rm 3.0.0.zip
RUN sed -i 's/-dumpversion/-dumpfullversion/g' opencv-3.0.0/cmake/OpenCVDetectCXXCompiler.cmake
RUN pip3 install numpy
RUN apt install -y g++-7 gcc-7
WORKDIR /opencv-build-workdir/opencv-3.0.0/build
RUN cmake \
    -DWITH_QT=ON \
    -DWITH_OPENGL=ON \
    -DFORCE_VTK=ON \
    -DWITH_TBB=ON \
    -DWITH_GDAL=ON \
    -DWITH_XINE=ON \
    -DBUILD_EXAMPLES=ON \
    -Wno-dev \
    -DENABLE_PRECOMPILED_HEADERS=OFF ..
RUN make -j8
RUN make install
RUN ldconfig

# Enable SSH
CMD ["/usr/sbin/sshd", "-D"]
