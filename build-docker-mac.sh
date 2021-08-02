#!/bin/zsh

export IP=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')

# setting XServer
open -a XQuartz
xhost + $IP

# docker-compose build
echo "DISPLAY=${IP}:0" > .display
docker-compose build
docker-compose up -d $@

# setting ffmpeg for video stream
ffmpeg -f avfoundation \
    -framerate 30 -video_size 640x480 \
    -i "0:none" \
    -vcodec mpeg4 \
    -flags low_delay -preset ultrafast -strict experimental \
    -f h264 \
    udp://localhost:8000
