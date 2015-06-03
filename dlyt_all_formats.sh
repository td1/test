#!/bin/sh

URL_11=https://www.youtube.com/watch?v=tq5RATqvPLY
URL_12=https://www.youtube.com/watch?v=CVyIsK2pNA0
URL_13=https://www.youtube.com/watch?v=8Upjx4EsE7U
URL_14=https://www.youtube.com/watch?v=rfTLM8UF27o
URL_15=https://www.youtube.com/watch?v=GEDrMpKVJ4M
URL_16=https://www.youtube.com/watch?v=UqIu58T82k8
URL_21=https://www.youtube.com/watch?v=mDjs5HapRp4
URL_22=https://www.youtube.com/watch?v=vaoUcqwE5rI
URL_23=https://www.youtube.com/watch?v=B7MkKVirH4s
URL_24=https://www.youtube.com/watch?v=bls--mDYNvs
URL_25=https://www.youtube.com/watch?v=BWo5L6qXMA8
URL_26=https://www.youtube.com/watch?v=o49NRE9Z6ks

youtube-dl --all-formats $URL_11 -o './bunny720pavi_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_12 -o './bunny720p_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_13 -o './bunny1_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_14 -o './bunny2_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_15 -o './bunny3_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_16 -o './bunny4_%(format)s.%(ext)s'

youtube-dl --all-formats $URL_21 -o './empire1080p_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_22 -o './empire720p_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_23 -o './empire1_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_24 -o './empire2_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_25 -o './empire3_%(format)s.%(ext)s'
youtube-dl --all-formats $URL_26 -o './empire4_%(format)s.%(ext)s'
