FROM ubuntu:18.04

# Essential Packages
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

# Enable SSH
CMD ["/usr/sbin/sshd", "-D"]
