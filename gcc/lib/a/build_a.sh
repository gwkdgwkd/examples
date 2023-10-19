#!/bin/bash

buildStatic() {
    echo "Build Static A"
    gcc -DA -c func_a.c
    ar -crv libfunc_a.a func_a.o
}

buildStaticWithC() {
    echo "Build Static A with C"
    gcc -DA -c func_a.c
    ar -crv libfunc_a.a func_a.o ../c/func_c.o
}

buildShared() {
    echo "Build Shared A"
    gcc -DSO -c func_a.c
    gcc -shared -fPIC -o libfunc_a.so func_a.o
}

buildSharedWithC() {
    echo "Build Shared A with C"
    gcc -DSO -c func_a.c
    gcc -shared -fPIC -o libfunc_a.so func_a.o -L${PWD}/../c/ -lfunc_c
}

buildStaticWithCIR() {
    echo "Build Static A with C"
    gcc -DA -DCIRCULAR -c func_a.c
    ar -crv libfunc_a.a func_a.o ../c/func_c.o
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
    buildStaticWithC
    ;;
    3)
    buildShared
    ;;
    4)
    buildSharedWithC
    ;;
    5)
    buildStaticWithCIR
    ;;
    6)
    clear
    ;;
    *)
    echo "do nothing"
    ;;
esac
