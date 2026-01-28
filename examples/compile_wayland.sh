#!/usr/bin/bash

# Basic script for building one of the example using wayland for testing purposes.
if [ $# -eq 0 ]
then
    echo "must specify an example to build. Ex:"
    echo "$0 olc_Polygons"
    echo "$0 olcPGE3_CirclesEllipses"
    exit 1
fi

if [ ! -f "/usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml" ]
then
    echo "You may need to install wayland-protocols via your package manager"
    exit 1
fi

if [ $(pkg-config --exists xkbcommon; echo $?) -eq 1 ]
then
    echo "You may need to install libxkbcommon-dev via your package manager"
    exit 1
fi

# May need to install wayland-protocols to get the xdg-decoration to build
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.c
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.h
wayland-scanner private-code /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml xdg-decoration.c
wayland-scanner client-header /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml xdg-decoration.h

gcc -c xdg-shell.c
gcc -c xdg-decoration.c

g++ -ggdb -fmax-errors=5 -std=c++20 -DOLC_HOST=3 -I.  -o $1 $1.cpp $(pkg-config --libs xkbcommon) -lpng -lGL -lwayland-client -lwayland-egl -lEGL -Wall -lpthread xdg-shell.o xdg-decoration.o
