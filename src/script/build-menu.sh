#!/bin/sh
gcc -O3 ../menu.c ../lib/lib_gpio.c ../lib/lib_png.c ../lib/lib_spi.c ../lib/lib_oled.c ../lib/lib_ui.c ../lib/lib_jpeg.c -I../lib/ -std=gnu11 -o ../menu $(pkg-config --cflags --libs cairo) && mv ../menu ../../bin/