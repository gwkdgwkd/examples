#!/bin/bash

buildStatic() {
    echo "Build Static B"
    gcc -DA -c func_b.c
    ar -crv libfunc_b.a func_b.o
}

buildStaticWithC() {
    echo "Build Static B with C"
    gcc -DA -c func_b.c
    ar -crv libfunc_b.a func_b.o ../c/func_c.o
}

buildShared() {
    echo "Build Shared B"
    gcc -DSO -c func_b.c
    gcc -shared -fPIC -o libfunc_b.so func_b.o
}

buildSharedWithC() {
    echo "Build Shared B with C"
    gcc -DSO -c func_b.c
    gcc -shared -fPIC -o libfunc_b.so func_b.o -L${PWD}/../c/ -lfunc_c
}

buildStaticWithCIR() {
    echo "Build Static B with C"
    gcc -DA -DCIRCULAR -c func_b.c
    ar -crv libfunc_b.a func_b.o ../c/func_c.o
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
