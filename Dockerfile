FROM ubuntu:18.04

# Essential Packages
RUN apt update && apt upgrade -y
RUN apt install -y apt-utils gcc g++ openssh-server cmake build-essential gdb gdbserver rsync vim

RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed  's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22 7777

RUN useradd -ms /bin/bash debugger
RUN echo 'debuger:pwd' | chpasswd

# Custom Packages

# Enable SSH
CMD ["/usr/sbin/sshd", "-D"]
