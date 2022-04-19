#!/bin/bash

echo "test lib"

buildStaticA(){
    cd staticA
    echo "Build Static A"
    gcc -c staticA.c
    ar -crv libstaticA.a staticA.o
    echo "=======nm libstaticA.a======"
    nm libstaticA.a
    cd ..
}

buildStaticB(){
    echo "Build Static B"
    cd staticB
    path=""
    lib=""
    case "$1" in
        1)
        path=staticA
        lib=staticA
        ;;
        2)
        path=sharedA
        lib=sharedA
        ;;
        *)
        ;;
    esac
    gcc -c -I${PWD}/../${path}/ staticB.c -L${PWD}/../${path}/ -l${lib}
    ar -crv libstaticB.a staticB.o
    echo "=======nm libstaticB.a======"
    nm libstaticB.a
    cd ..
}

buildSharedA(){
    echo "Build Shared A"
    cd sharedA
    gcc -c sharedA.c
    gcc -shared -fPIC -o libsharedA.so sharedA.o
    echo "=======nm libsharedA.so======"
    nm libsharedA.so | grep "func*"
    cd ..
}

buildSharedB(){
    echo "Build Shared B"
    cd sharedB
    path=""
    lib=""
    case "$1" in
        1)
        path=staticA
        lib=staticA
        ;;
        2)
        path=sharedA
        lib=sharedA
        ;;
        *)
        ;;
    esac
    gcc -c -I${PWD}/../${path}/ sharedB.c -L${PWD}/../${path}/ -l${lib}
    gcc -shared -fPIC -o libsharedB.so sharedB.o
    echo "=======nm libsharedB.so======"
    nm libsharedB.so | grep "func*"
    cd ..
}

buildAllStaticA(){
    cd staticA
    echo "Build Static A"
    gcc -c staticA.c
    ar -crv libA.a staticA.o
    echo "=======nm libA.a======"
    nm libA.a | grep -E "funcA|funcB"
    gcc -shared -fPIC -o libA.so staticA.o
    echo "=======nm libA.so======"
    nm libA.so | grep -E "funcA|funcB"
    cd ..
}

buildAllStaticB(){
    echo "Build Static B"
    cd staticB
    path=staticA
    lib=staticA
    gcc -c -I${PWD}/../${path}/ staticB.c -L${PWD}/../${path}/ -l${lib}
    ar -crv libB.a staticB.o
    echo "=======nm libB.a======"
    nm libB.a | grep -E "funcA|funcB"
    gcc -shared -fPIC -o libB.so staticB.o
    echo "=======nm libB.so======"
    nm libB.so | grep -E "funcA|funcB"
    cd ..
}

buildMain(){
    echo "Build Main"
    rm a.out *.a -rf

    case "$1" in
        1)
        # ./build.sh 1 1 1
        gcc -I${PWD}/staticB/ -I${PWD}/staticA/ main.c -L${PWD}/staticB/ -lstaticB -L${PWD}/staticA/ -lstaticA
        echo "=======nm a.out======"
        nm a.out | grep "func*"
        echo "=======run a.out======"
        ./a.out
        ;;
        2)
        # ./build.sh 1 1 2
        ar -crv libstaticAB.a ${PWD}/staticA/staticA.o ${PWD}/staticB/staticB.o
        echo "=======nm libstaticAB.a======"
        nm libstaticAB.a
        gcc -I${PWD}/staticB/ main.c -L${PWD} -lstaticAB
        echo "=======nm a.out======"
        nm a.out | grep "func*"
        echo "=======run a.out======"
        ./a.out
        ;;
        3)
        # ./build.sh 2 1 3
        gcc -I${PWD}/staticB/ -I${PWD}/sharedA/ main.c -L${PWD}/staticB/ -lstaticB -L${PWD}/sharedA/ -lsharedA
        echo "=======nm a.out======"
        nm a.out | grep "func*"
        echo "=======run a.out======"
        export LD_LIBRARY_PATH=${PWD}/sharedA;./a.out
        ;;
        4)
        # ./build.sh 2 2 4
        gcc -I${PWD}/sharedB/ -I${PWD}/sharedA/ main.c -L${PWD}/sharedB/ -lsharedB -L${PWD}/sharedA/ -lsharedA
        echo "=======nm a.out======"
        nm a.out | grep "func*"
        echo "=======run a.out======"
        export LD_LIBRARY_PATH=${PWD}/sharedB:${PWD}/sharedA;./a.out
        ;;
        5)
        # ./build.sh 1 2 5
        gcc -I${PWD}/sharedB/ -I${PWD}/sharedA/ main.c -L${PWD}/sharedB/ -lsharedB -L${PWD}/staticA/ -lstaticA
        echo "=======nm a.out======"
        nm a.out | grep "func*"
        echo "=======run a.out======"
        export LD_LIBRARY_PATH=${PWD}/sharedB;./a.out
        ;;
        6)
        # ./build.sh 2 2 6
        gcc -I${PWD}/sharedB/ -I${PWD}/sharedA/ main.c -static -L${PWD}/sharedB/ -lsharedB -L${PWD}/sharedA/ -lsharedA
        ;;
        7)
        # ./build.sh 3 3 7
        buildAllStaticA
        buildAllStaticB
        gcc -I${PWD}/staticB/ -I${PWD}/staticA/ main.c -L${PWD}/staticB/ -lB -L${PWD}/staticA/ -lA
        echo "=======nm a.out======"
        nm a.out | grep -E "funcA|funcB"
        echo "=======run a.out======"
        #./a.out
        export LD_LIBRARY_PATH=${PWD}/staticB:${PWD}/staticA;./a.out
        ;;
        8)
        # ./build.sh 3 3 8
        buildAllStaticA
        buildAllStaticB
        gcc -I${PWD}/staticB/ -I${PWD}/staticA/ -static main.c -L${PWD}/staticB/ -lB -L${PWD}/staticA/ -lA
        echo "=======nm a.out======"
        nm a.out | grep -E "funcA|funcB"
        echo "=======run a.out======"
        ./a.out
        ;;
        *)
        echo "do nothing"
        ;;
    esac
}

clear() {
find . -name "*.o" | xargs rm
find . -name "*.a" | xargs rm
find . -name "*.so" | xargs rm
rm a.out
}

clear

case "$1" in
    1)
    buildStaticA
    ;;
    2)
    buildSharedA
    ;;
    *)
    echo "do nothing"
    ;;
esac

case "$2" in
    1)
    buildStaticB $1
    ;;
    2)
    buildSharedB $1
    ;;
    *)
    echo "do nothing"
    ;;
esac

buildMain $3
