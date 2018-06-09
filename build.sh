#!/bin/bash

g++ egl1.c -o render2img -lQt5Core -lQt5Gui -L/usr/lib/nvidia-390 -lOpenGL -lGLU -lEGL -lGLESv2 -lX11 -I/usr/include/x86_64-linux-gnu/qt5/ -fPIC -DQT_NO_VERSION_TAGGING

