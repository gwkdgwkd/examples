#!/bin/bash

buildStatic() {
    echo "Build Static C"
    gcc -DA -c func_c.c
    ar -crv libfunc_c.a func_c.o
}

buildShared() {
    echo "Build Shared C"
    gcc -DSO -c func_c.c
    gcc -shared -fPIC -o libfunc_c.so func_c.o
}


clear() {
    rm *.o *.a *.so -f
}

clear

case "$1" in
    1)
    buildStatic
    ;;
    2)
    buildShared
    ;;
    3)
    buildStatic
    buildShared
    ;;
    4)
    clear
    ;;
    *)
    echo "do nothing"
    ;;
esac
