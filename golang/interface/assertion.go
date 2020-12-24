package main

import (
	"bytes"
	"fmt"
	"io"
	"os"
)

func main() {
	// 类型断言是一个使用在接口值上的操作。语法上它看起来像x.(T)被称为断言类型,这里x表示
	// 一个接口的类型和T表示一个类型。一个类型断言检查它操作对象的动态类型是否和断言的类型匹配。

	// 这里有两种可能。第一种,如果断言的类型T是一个具体类型,然后类型断言检查x的动态类
	// 型是否和T相同。如果这个检查成功了,类型断言的结果是x的动态值,当然它的类型是T。换
	// 句话说,具体类型的类型断言从它的操作对象中获得具体的值。如果检查失败,接下来这个
	// 操作会抛出panic。例如:
	var w io.Writer
	w = os.Stdout
	f := w.(*os.File)     // success: f ==	os.Stdout
	fmt.Printf("%T\n", f) // *os.File
	// c := w.(*bytes.Buffer) // panic: interface holds *os.File, not *bytes.Buffer
	// 第二种,如果相反断言的类型T是一个接口类型,然后类型断言检查是否x的动态类型满足T。
	// 如果这个检查成功了,动态值没有获取到;这个结果仍然是一个有相同类型和值部分的接口
	// 值,但是结果有类型T。换句话说,对一个接口类型的类型断言改变了类型的表述方式,改变
	// 了可以获取的方法集合(通常更大),但是它保护了接口值内部的动态类型和值的部分。
	rw := w.(io.ReadWriter) // success: *os.File has both Read and Write
	fmt.Printf("%T\n", rw)  // *os.File
	// w = new(ByteCounter)
	// rw = w.(io.ReadWriter) // panic: *ByteCounter has no Read method

	// 如果断言操作的对象是一个nil接口值,那么不论被断言的类型是什么这个类型断言都会失
	// 败。我们几乎不需要对一个更少限制性的接口类型(更少的方法集合)做断言,因为它表现
	// 的就像赋值操作一样,除了对于nil接口值的情况。
	w = rw                // io.ReadWriter is assignable to io.Writer
	w = rw.(io.Writer)    // fails only if rw == nil
	fmt.Printf("%T\n", w) // *os.File

	// 如果类型断言出现在一个预期有两个结果的赋值操作中,例如如下的定义,这个操
	// 作不会在失败的时候发生panic但是代替地返回一个额外的第二个结果,这个结果是一个标识
	// 成功的布尔值:
	var ww io.Writer = os.Stdout
	f1, ok := ww.(*os.File)       // success: ok, f == os.Stdout
	fmt.Printf("%v %v\n", f1, ok) // &{0xc0000b0060} true
	b1, ok := ww.(*bytes.Buffer)  // failure: !ok, b == nil
	fmt.Printf("%v %v\n", b1, ok) // <nil> false
	// 第二个结果常规地赋值给一个命名为ok的变量。如果这个操作失败了,那么ok就是false值,
	// 第一个结果等于被断言类型的零值,在这个例子中就是一个nil的*bytes.Buffer类型。

	// 这个ok结果经常立即用于决定程序下面做什么。if语句的扩展格式让这个变的很简洁:
	// if f, ok := w.(*os.File); ok {}

	// 当类型断言的操作对象是一个变量,你有时会看见原来的变量名重用而不是声明一个新的本
	// 地变量,这个重用的变量会覆盖原来的值:
	// if w, ok := w.(*os.File); ok {}
}
