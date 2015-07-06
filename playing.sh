#!/bin/sh
#MYID=$1
myID=1

URL_obunny1=https://www.youtube.com/watch?v=R2OMnTKmWbo
URL_obunny2=https://www.youtube.com/watch?v=qUYjjpeCbE0
URL_obunny3=https://www.youtube.com/watch?v=WiwJVtMkB30
URL_obunny4=https://www.youtube.com/watch?v=rWQFcFcFTIU
URL_obunny720p=https://www.youtube.com/watch?v=APmO-tLnofs

URL_bunny1=https://www.youtube.com/watch?v=8Upjx4EsE7U
URL_bunny2=https://www.youtube.com/watch?v=rfTLM8UF27o 
URL_bunny3=https://www.youtube.com/watch?v=GEDrMpKVJ4M 
URL_bunny4=https://www.youtube.com/watch?v=UqIu58T82k8 
URL_bunny720p=https://www.youtube.com/watch?v=CVyIsK2pNA0

URL_empire1=https://www.youtube.com/watch?v=lp0zuoGkN90
URL_empire2=https://www.youtube.com/watch?v=yXJDpK99qeg
URL_empire3=https://www.youtube.com/watch?v=XjtcRnGMNXo
URL_empire4=https://www.youtube.com/watch?v=DMocUiTgTq8
URL_empire720p=https://www.youtube.com/watch?v=AabHZAw2CmY

sudo modprobe bcm2835-v4l2

youtube-dl $URL_obunny1 -o './data/testvideo.mp4'
#youtube-dl $URL_obunny720p -o './data/testvideo.mp4'
#youtube-dl $URL_bunny1 -o './data/testvideo.mp4'
#youtube-dl $URL_bunny720p -o './data/testvideo.mp4'
youtube-dl $URL_empire2 -o './data/testvideo.mp4'
youtube-dl $URL_empire720p -o './data/testvideo.mp4'
./hung_pixel_dl $myID
