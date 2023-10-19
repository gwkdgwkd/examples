#!/bin/bash

buildMain(){
    echo "Build Main"

    case "$1" in
        1)
        gcc -DUSEC main.c -L${PWD}/c/ -lfunc_c

        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        2)
        gcc -DUSEC main.c -L${PWD}/c/ -lfunc_c
        ls c/libfunc_c.*

        echo
        echo "nm c/libfunc_c.so"
        nm c/libfunc_c.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/c;./a.out
        ;;
        3)
        gcc -DUSEC -static main.c -L${PWD}/c/ -lfunc_c

        ls c/libfunc_c.*
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "funcC"
        echo

        ./a.out
        ;;
        4)
        gcc -DUSEA main.c -L${PWD}/c/ -L${PWD}/a/ -lfunc_a -lfunc_c

        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        5)
        gcc -DUSEA main.c -L${PWD}/c/ -L${PWD}/a/ -lfunc_c -lfunc_a
        # 下面两种方法，可以解决上面语句静态库顺序不对的问题（包括循环依赖）：
        # gcc -DUSEA main.c -L${PWD}/c/ -L${PWD}/a/ -Xlinker "-(" -lfunc_c -lfunc_a -Xlinker "-)"
        # gcc -DUSEA main.c -L${PWD}/c/ -L${PWD}/a/ -Wl,--start-group -lfunc_c -lfunc_a -Wl,--end-group
        ;;
        6)
        gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a
        
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        7)
        gcc -DUSEA main.c -L${PWD}/c/ -L${PWD}/a/ -lfunc_a -lfunc_c
        
        echo
        echo "nm c/libfunc_c.so"
        nm c/libfunc_c.so | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/c:${PWD}/a;./a.out
        ;;
        8)
        gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a
        
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/a;./a.out
        ;;
        9)
        # gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a -Wl,-unresolved-symbols=ignore-in-shared-libs
        # gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a -Wl,-rpath-link=${PWD}/c
        gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a -Wl,-rpath=${PWD}/c

        echo
        echo "nm c/libfunc_c.so"
        nm c/libfunc_c.so | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/c:${PWD}/a;./a.out
        ;;
        10)
        gcc -DUSEA main.c -L${PWD}/a/ -lfunc_a -L${PWD}/c/ -lfunc_c
        
        echo
        echo "nm c/libfunc_c.so"
        nm c/libfunc_c.so | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/c;./a.out
        ;;
        11)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b -L${PWD}/c/ -lfunc_c
        
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm b/libfunc_b.a"
        nm b/libfunc_b.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        12)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b
        
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm b/libfunc_b.a"
        nm b/libfunc_b.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        13)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b -L${PWD}/c/ -lfunc_c

        echo
        echo "nm c/libfunc_c.so"
        nm c/libfunc_c.so | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm b/libfunc_b.so"
        nm b/libfunc_b.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/c:${PWD}/b:${PWD}/a;./a.out
        ;;
        14)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b

        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm b/libfunc_b.so"
        nm b/libfunc_b.so | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/b:${PWD}/a;./a.out
        ;;
        15)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b

        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.so"
        nm a/libfunc_a.so | grep "func*"
        echo "nm b/libfunc_b.a"
        nm b/libfunc_b.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        export LD_LIBRARY_PATH=${PWD}/a;./a.out
        ;;
        16)
        gcc -DUSEA -DUSEB main.c -L${PWD}/a/ -lfunc_a -L${PWD}/b/ -lfunc_b
        
        echo
        echo "nm c/libfunc_c.a"
        nm c/libfunc_c.a | grep "func*"
        echo "nm a/libfunc_a.a"
        nm a/libfunc_a.a | grep "func*"
        echo "nm b/libfunc_b.a"
        nm b/libfunc_b.a | grep "func*"
        echo "nm a.out"
        nm a.out | grep "func*"
        echo

        ./a.out
        ;;
        *)
        echo "do nothing"
        ;;
    esac
}

clear() {
    find . -name "*.o" | xargs rm -f
    find . -name "*.a" | xargs rm -f
    find . -name "*.so" | xargs rm -f
    rm a.out -f
}

clear

echo =======================================
echo $0 $@

# build c
cd c && ./build_c.sh $2 && cd ..


# build a
if [ $# -ge 3 ];then
cd a && ./build_a.sh $3 && cd ..
fi

# build b
if [ $# -ge 4 ];then
cd b && ./build_b.sh $4 && cd ..
fi

# build main
buildMain $1


# =======================================
# ./build.sh 1 1
# Build Static C
# a - func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 000000000000118b T funcC

# func C form libfunc_c.a
# main

# a.out -> libfunc_c.a
# 可执行程序使用静态库，funC被链接到a.out中，直接运行就行

# =======================================
# ./build.sh 1 3
# Build Static C
# a - func_c.o
# Build Shared C
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
#                  U funcC

# ./a.out: error while loading shared libraries: libfunc_c.so: cannot open shared object file: No such file or directory
# 同时存在动态库和静态库，优先使用动态库，运行时没有指定动态库的查找路径，报错

# =======================================
# ./build.sh 2 2
# Build Shared C
# Build Main
# c/libfunc_c.so

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a.out
#                  U funcC

# func C form libfunc_c.so
# main

# a.out -> libfunc_c.so
# 使用动态库，funC没被链接到a.out中，运行时需要指定libfunc_c.so的路径

# =======================================
# ./build.sh 2 3
# Build Static C
# a - func_c.o
# Build Shared C
# Build Main
# c/libfunc_c.a  c/libfunc_c.so

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a.out
#                  U funcC

# func C form libfunc_c.so
# main

# a.out -> libfunc_c.so
# 同时存在动态库和静态库，优先使用动态库，运行时需要指定动态库的查找路径

# =======================================
# ./build.sh 3 3
# Build Static C
# a - func_c.o
# Build Shared C
# Build Main
# c/libfunc_c.a  c/libfunc_c.so

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 0000000000401bee T funcC

# func C form libfunc_c.a
# main

# a.out -> libfunc_c.a
# 同时存在a和so，使用-static强制使用静态库

# =======================================
# ./build.sh 4 1 1
# Build Static C
# a - func_c.o
# Build Static A
# a - func_a.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# nm a.out
# 000000000000118b T funcA
# 00000000000011b4 T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# main

# a.out -> libfunc_a.a -> libfunc_c.a
# 编译libfunc_a.a时，没有链接libfunc_c.a，所以，编译可执行程序时，两个库都要链接

# =======================================
# ./build.sh 5 1 1
# Build Static C
# a - func_c.o
# Build Static A
# a - func_a.o
# Build Main
# /usr/bin/ld: /disk/me/examples/gcc/lib/a//libfunc_a.a(func_a.o): in function `funcA':
# func_a.c:(.text+0xa): undefined reference to `funcC'
# collect2: error: ld returned 1 exit status

# 库的顺序不能改变，a依赖c，所以func_a的要放到func_c前

# =======================================
# ./build.sh 6 1 1
# Build Static C
# a - func_c.o
# Build Static A
# a - func_a.o
# Build Main
# /usr/bin/ld: /disk/me/examples/gcc/lib/a//libfunc_a.a(func_a.o): in function `funcA':
# func_a.c:(.text+0xa): undefined reference to `funcC'
# collect2: error: ld returned 1 exit status

# 编译libfunc_a.a时，没有链接libfunc_c.a，如果编译可执行程序没有链接libfunc_c.a，报错

# =======================================
# ./build.sh 6 1 2
# Build Static C
# a - func_c.o
# Build Static A with C
# a - func_a.o
# a - ../c/func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 000000000000118b T funcA
# 00000000000011b4 T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# main

# a.out -> libfunc_a.a(libfunc_c.a)
# 编译libfunc_a.a时，把libfunc_c.a也压缩到一起了，所以编译可执行程序时，不用再依赖c

# =======================================
# ./build.sh 7 2 3
# Build Shared C
# Build Shared A
# Build Main

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
#                  U funcC
# nm a.out
#                  U funcA

# func C form libfunc_c.so
# func A form libfunc_a.so
# main

# a.out -> libfunc_a.so -> libfunc_c.so
# 都使用动态库，可执行程序中没有符号的定义，运行时要指定两个动态库的查找路径

# =======================================
# ./build.sh 8 1 4
# Build Static C
# a - func_c.o
# Build Shared A with C
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
# 0000000000001163 T funcC
# nm a.out
#                  U funcA

# func C form libfunc_c.a
# func A form libfunc_a.so
# main

# a.out -> libfunc_a.so(libfunc_c.a)
# 编译libfunc_a.so时链接了libfunc_c.a，所以编译和运行可执行程序时，只依赖libfunc_a.so

# =======================================
# ./build.sh 8 2 4
# Build Shared C
# Build Shared A with C
# Build Main
# /usr/bin/ld: warning: libfunc_c.so, needed by /disk/me/examples/gcc/lib/a//libfunc_a.so, not found (try using -rpath or -rpath-link)
# /usr/bin/ld: /disk/me/examples/gcc/lib/a//libfunc_a.so: undefined reference to `funcC'
# collect2: error: ld returned 1 exit status

# 编译libfunc_a.so时链接了libfunc_c.so，但编译程序需要同时依赖libfunc_a.so和libfunc_c.so

# =======================================
# ./build.sh 9 2 4
# Build Shared C
# Build Shared A with C
# Build Main

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
#                  U funcC
# nm a.out
#                  U funcA

# func C form libfunc_c.so
# func A form libfunc_a.so
# main

# a.out -> libfunc_a.so(libfunc_c.so)
# 编译可执行程序时，只依赖libfunc_a.so，但运行时，要指定libfunc_a.so和libfunc_c.so所在的路径。

# 当链接可执行程序时，它有一些未解析的符号，它将查找随后的库列表，尝试解决未解析的符号。
# 在此过程中，它发现一些符号是由libfunc_a.so提供的，因此它指出它们现在已由该库解析。
# 但是，它还发现其中一些符号使用了可执行文件中尚未解析的其他符号，因此它现在也需要解析这些符号。
# 如果不链接libfunc_a.so，可执行程序将是不完整的。一旦链接到libfunc_a.so，所有符号都会被解析并且链接完成。
# 使用-unresolved-symbols-in-shared-libs并不能解决问题，它只是推迟它，以便在运行时解析这些符号。
# 这就是-rpath的用途，指定运行时要搜索的库。如果无法解析这些符号，可执行程序将无法启动。

# =======================================
# ./build.sh 10 2 1
# Build Shared C
# Build Static A
# a - func_a.o
# Build Main

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# nm a.out
# 000000000000119b T funcA
#                  U funcC

# func C form libfunc_c.so
# func A form libfunc_a.a
# main

# a.out -> libfunc_a.a -> libfunc_c.so
# 编译libfunc_a.a是没有链接libfunc_c.so，所以可执行程序编译和运行时，都依赖libfunc_c.so

# =======================================
# ./build.sh 11 1 1 1
# Build Static C
# a - func_c.o
# Build Static A
# a - func_a.o
# Build Static B
# a - func_b.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# nm b/libfunc_b.a
# func_b.o:
# 0000000000000000 T funcB
#                  U funcC
# nm a.out
# 0000000000001195 T funcA
# 00000000000011be T funcB
# 00000000000011e7 T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# func C form libfunc_c.a
# func B form libfunc_b.a
# main

#      ↗ libfunc_a.a ↘
# a.out               libfunc_c.a
#      ↘ libfunc_b.a ↗
# =======================================
# ./build.sh 12 1 2 2
# Build Static C
# a - func_c.o
# Build Static A with C
# a - func_a.o
# a - ../c/func_c.o
# Build Static B with C
# a - func_b.o
# a - ../c/func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm b/libfunc_b.a
# func_b.o:
# 0000000000000000 T funcB
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 0000000000001195 T funcA
# 00000000000011dd T funcB
# 00000000000011be T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# func C form libfunc_c.a
# func B form libfunc_b.a
# main

#      ↗ libfunc_a.a(libfunc_c.a)
# a.out               
#      ↘ libfunc_b.a(libfunc_c.a)

# =======================================
# ./build.sh 12 1 2 1
# Build Static C
# a - func_c.o
# Build Static A with C
# a - func_a.o
# a - ../c/func_c.o
# Build Static B
# a - func_b.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm b/libfunc_b.a
# func_b.o:
# 0000000000000000 T funcB
#                  U funcC
# nm a.out
# 0000000000001195 T funcA
# 00000000000011dd T funcB
# 00000000000011be T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# func C form libfunc_c.a
# func B form libfunc_b.a
# main

#      ↗ libfunc_a.a(libfunc_c.a)
# a.out                            
#      ↘ libfunc_b.a  ->  libfunc_c.a
# 编译可执行程序时，不用依赖libfunc_c.a，因为libfunc_a.a已经包含了libfunc_c.a

# =======================================
# ./build.sh 12 1 1 2
# Build Static C
# a - func_c.o
# Build Static A
# a - func_a.o
# Build Static B with C
# a - func_b.o
# a - ../c/func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000000 T funcA
#                  U funcC
# nm b/libfunc_b.a
# func_b.o:
# 0000000000000000 T funcB
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 0000000000001195 T funcA
# 00000000000011be T funcB
# 00000000000011e7 T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# func C form libfunc_c.a
# func B form libfunc_b.a
# main

#      ↗ libfunc_a.a -> libfunc_c.a
# a.out                            
#      ↘ libfunc_b.a(libfunc_c.a)

# =======================================
# ./build.sh 13 2 3 3
# Build Shared C
# Build Shared A
# Build Shared B
# Build Main

# nm c/libfunc_c.so
# 000000000000112a T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
#                  U funcC
# nm b/libfunc_b.so
# 000000000000113a T funcB
#                  U funcC
# nm a.out
#                  U funcA
#                  U funcB

# func C form libfunc_c.so
# func A form libfunc_a.so
# func C form libfunc_c.so
# func B form libfunc_b.so
# main

#      ↗ libfunc_a.so ↘
# a.out               libfunc_c.so
#      ↘ libfunc_b.so ↗

# =======================================
# ./build.sh 14 1 4 4
# Build Static C
# a - func_c.o
# Build Shared A with C
# Build Shared B with C
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
# 0000000000001163 T funcC
# nm b/libfunc_b.so
# 000000000000113a T funcB
# 0000000000001163 T funcC
# nm a.out
#                  U funcA
#                  U funcB

# func C form libfunc_c.a
# func A form libfunc_a.so
# func C form libfunc_c.a
# func B form libfunc_b.so
# main

#      ↗ libfunc_a.so(libfunc_c.a)
# a.out                            
#      ↘ libfunc_b.so(libfunc_c.a)

# =======================================
# ./build.sh 15 1 3 2
# Build Static C
# a - func_c.o
# Build Shared A
# Build Static B with C
# a - func_b.o
# a - ../c/func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.so
# 000000000000113a T funcA
#                  U funcC
# nm b/libfunc_b.a
# func_b.o:
# 0000000000000000 T funcB
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
#                  U funcA
# 00000000000011a5 T funcB
# 00000000000011ce T funcC

# func C form libfunc_c.a
# func A form libfunc_a.so
# func C form libfunc_c.a
# func B form libfunc_b.a
# main

#      ↗ libfunc_a.so -> libfunc_c.a
# a.out                            
#      ↘ libfunc_b.a(libfunc_c.a)

# =======================================
# ./build.sh 16 1 5 5
# Build Static C
# a - func_c.o
# Build Static A with C
# a - func_a.o
# a - ../c/func_c.o
# Build Static B with C
# a - func_b.o
# a - ../c/func_c.o
# Build Main

# nm c/libfunc_c.a
# func_c.o:
# 0000000000000000 T funcC
# nm a/libfunc_a.a
# func_a.o:
# 0000000000000029 T func_a
# 0000000000000000 T funcA
#                  U func_b
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm b/libfunc_b.a
# func_b.o:
#                  U func_a
# 0000000000000033 T func_b
# 0000000000000000 T funcB
#                  U funcC
# func_c.o:
# 0000000000000000 T funcC
# nm a.out
# 00000000000011be T func_a
# 0000000000001195 T funcA
# 000000000000122d T func_b
# 00000000000011fa T funcB
# 00000000000011db T funcC

# func C form libfunc_c.a
# func A form libfunc_a.a
# func C form libfunc_c.a
# func B form libfunc_b.a
# func a form libfunc_a.a
# func b form libfunc_b.a
# main

# 为啥能编译通过呢？怎么模拟循环依赖的场景？
