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
    -framerate 10 -video_size 960x540 \
    -i "1:none" \
    -vcodec mpeg4 \
    -flags low_delay -preset ultrafast -strict experimental \
    -f rtp_mpegts \
    rtp://localhost:8000
