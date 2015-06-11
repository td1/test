#!/bin/sh

#ffmpeg -loop 1 -i image.jpg -i audio.wav -c:v libx264 -tune stillimage -c:a aac -strict experimental -b:a 192k -pix_fmt yuv420p -shortest out.mp4
#ffmpeg -loop 1 -i testpattern.jpg -c:v libx264 -tune stillimage -c:a aac -strict experimental -b:a 192k -pix_fmt yuv420p -shortest testpattern.mp4
#using this on ubuntu to creat a mp4 file from single image jpg file
avconv -loop 1 -i testpattern.jpg -c:v libx264 -tune stillimage -c:a aac -strict experimental -b:a 192k -pix_fmt yuv420p -shortest testpattern.mp4
