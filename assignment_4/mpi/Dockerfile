FROM ubuntu:latest
# written by acoustikue@yonsei.ac.kr

RUN apt-get update
RUN apt-get install -y --no-install-recommends apt-utils build-essential sudo git wget vim

RUN apt-get install -y locales
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Seoul

# install tzdata package
RUN apt-get install -y tzdata

# set your timezone
# RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
# RUN dpkg-reconfigure --frontend noninteractive tzdata

RUN gcc --version && g++ --version

RUN mkdir /home/workspace
ENV HOME "/home"

WORKDIR /home/workspace
RUN apt install -y python python3 python3-distutils curl python3-pip

# Git config
RUN git config --global user.name "your_id"
RUN git config --global user.email "your_mail"

# Add more configuration here.
RUN apt install -y mpich