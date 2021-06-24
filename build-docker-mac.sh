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
  -framerate 30 -video_size 320x240 \
  -i "1:none" -vcodec libx265 \
  -maxrate 2000k -fflags nobuffer -flags low_delay -preset ultrafast \
  -strict experimental \
  -f rtp_mpegts rtp://127.0.0.1:12345
