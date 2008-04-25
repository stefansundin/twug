#!/bin/sh
gcc -o server server.c /usr/lib/libportaudio.so.2
gcc -o client client.c /usr/lib/libportaudio.so.2
strip server client

