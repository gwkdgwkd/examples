package main

import (
	"fmt"
	"time"
)

// goroutine和channel,其支持“顺序通信进程”(communicating sequential processes)或被简称为CSP。
// CSP是一种现代的并发编程模型,在这种编程模型中值会在不同的运行实例(goroutine)中传递,尽管大多数情况下
// 仍然是被限制在单一实例中。
// 传统的并发模型:多线程共享内存

// 在Go语言中,每一个并发的执行单元叫作一个goroutine。

// 当一个程序启动时,其主函数即在一个单独的goroutine中运行,我们叫它main	goroutine。新
// 的goroutine会用go语句来创建。在语法上,go语句是一个普通的函数或方法调用前加上关键
// 字go。go语句会使其语句中的函数在一个新创建的goroutine中运行。而go语句本身会迅速地完成。

// 主函数返回时,所有的goroutine都会被直接打断,程序退出。除了从主函
// 数退出或者直接终止程序之外,没有其它的编程方法能够让一个goroutine来打断另一个的执
// 行,但是可以通过goroutine之间的通信来让一个goroutine请求其它的goroutine,
// 并让被请求的goroutine自行结束执行。

// 网络编程是并发大显身手的一个领域,由于服务器是最典型的需要同时处理很多连接的程
// 序,这些连接一般来自于彼此独立的客户端。

// net包提供编写一个网络客户端或者服务器程序的基本组件,无论两者间通信是使用TCP,UDP
// 或者Unix domain sockets。

func main() {
	go spinner(100 * time.Millisecond)
	const n = 45
	fibN := fib(n)                                //	slow
	fmt.Printf("\rFibonacci(%d) = %d\n", n, fibN) // Fibonacci(45) = 1134903170
}
func spinner(delay time.Duration) {
	for {
		for _, r := range `-\|/` {
			fmt.Printf("\r%c", r)
			time.Sleep(delay)
		}
	}
}
func fib(x int) int {
	if x < 2 {
		return x
	}
	return fib(x-1) + fib(x-2)
}
