#!/bin/sh
#MYID=$1
MYID=1

URL_bunny1=https://www.youtube.com/watch?v=8Upjx4EsE7U
URL_bunny2=https://www.youtube.com/watch?v=rfTLM8UF27o 
URL_bunny3=https://www.youtube.com/watch?v=GEDrMpKVJ4M 
URL_bunny4=https://www.youtube.com/watch?v=UqIu58T82k8 

URL_empire1=https://www.youtube.com/watch?v=8Upjx4EsE7U
URL_empire2=https://www.youtube.com/watch?v=rfTLM8UF27o 
URL_empire3=https://www.youtube.com/watch?v=GEDrMpKVJ4M 
URL_empire4=https://www.youtube.com/watch?v=UqIu58T82k8 

sudo modprobe bcm2835-v4l2
youtube-dl $URL_bunny2 -o './data/bunny'$myID'.mp4'
#youtube-dl $URL_empire2 -o './data/empire'$myID'.mp4'
./hung_pixel_dl $MYID
