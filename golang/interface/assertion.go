package main

import (
	"bytes"
	"errors"
	"fmt"
	"io"
	"os"
	"syscall"
)

// os包中文件操作返回的错误集合。I/O可以因为任何数量的原因失败,但是有三种经常
// 的错误必须进行不同的处理:文件已经存在(对于创建操作),找不到文件(对于读取操
// 作),和权限拒绝。os包中提供了这三个帮助函数来对给定的错误值表示的失败进行分类:
// func IsExist(err error) bool
// func IsNotExist(err error) bool
// func IsPermission(err error) bool

// 处理I/O错误的逻辑可能一个和另一个平台非常的不同,所以这种方案并不健壮并且对相
// 同的失败可能会报出各种不同的错误消息。在测试的过程中,通过检查错误消息的子字符串
// 来保证特定的函数以期望的方式失败是非常有用的,但对于线上的代码是不够的。
// func IsNotExist(err error) bool {
// 	return strings.Contains(err.Error(), "file does not exist")
// }

// 一个更可靠的方式是使用一个专门的类型来描述结构化的错误。os包中定义了一个PathError
// 类型来描述在文件路径操作中涉及到的失败,像Open或者Delete操作,并且定义了一个叫
// LinkError的变体来描述涉及到两个文件路径的操作,像Symlink和Rename。
type PathError struct {
	Op   string
	Path string
	Err  error
}

func (e *PathError) Error() string {
	return e.Op + "	" + e.Path + ":	" + e.Err.Error()
}

var ErrNotExist = errors.New("file does not	exist")

func IsNotExist(err error) bool {
	if pe, ok := err.(*PathError); ok {
		err = pe.Err
	}
	return err == syscall.ENOENT || err == ErrNotExist
}

// 通过类型断言查询接口
// Write方法需要传入一个byte切片而我们希望写入的值是一个字符串,所以我们需要使用
// []byte(...)进行转换。这个转换分配内存并且做一个拷贝,但是这个拷贝在转换后几乎立马就
// 被丢弃掉。假装这是一个web服务器的核心部分并且的性能分析表示这个内存分配
// 使服务器的速度变慢。这里可以避免掉内存分配么?
// func writeHeader(w io.Writer, contentType string) error {
// 	if _, err := w.Write([]byte("Content-Type: ")); err != nil {
// 		return err
// 	}
// 	if _, err := w.Write([]byte(contentType)); err != nil {
// 		return err
// 	}
// }
// io.Writer接口告诉我们关于w持有的具体类型的唯一东西:就是可以向它写入字节切片。
// 根据net/http包知道，w变量持有的动态类型也有一个允许字符串高效写入的WriteString方法;
// 这个方法会避免去分配一个临时的拷贝。(许多满足io.Writer接口的重要类型同时也有WriteString方法,包
// 括*bytes.Buffer,*os.File和*bufio.Writer)
// 不能对任意io.Writer类型的变量w,假设它也拥有WriteString方法。但是我们可以定义一
// 个只有这个方法的新接口并且使用类型断言来检测是否w的动态类型满足这个新接口:
// func writeString(w io.Writer, s string) (n int, err error) {
// 	type stringWriter interface {
// 		WriteString(string) (n int, err error)
// 	}
// 	if sw, ok := w.(stringWriter); ok {
// 		return sw.WriteString(s) // avoid a copy
// 	}
// 	return w.Write([]byte(s)) // allocate temporary	copy
// }
// func writeHeader(w io.Writer, contentType string) error {
// 	if _, err := writeString(w, "Content-Type: "); err != nil {
// 		return err
// 	}
// 	if _, err := writeString(w, contentType); err != nil {
// 		return err
// 	}
// }
// 奇之处在于没有定义了WriteString方法的标准接口和没有指定它是一个需要行为
// 的标准接口。而且一个具体类型只会通过它的方法决定它是否满足stringWriter接口,而不是
// 任何它和这个接口类型表明的关系。它的意思就是上面的技术依赖于一个假设;这个假设就
// 是,如果一个类型满足下面的这个接口,然后WriteString(s)就方法必须和Write([]byte(s))有相
// 同的效果。

// fmt.Fprintf函数怎么从其它所有值中区分满足error或者fmt.Stringer接口的值?在
// fmt.Fprintf内部,有一个将单个操作对象转换成一个字符串的步骤
// func formatOneValue(x interface{}) string {
// 	if err, ok := x.(error); ok {
// 		return err.Error()
// 	}
// 	if str, ok := x.(Stringer); ok {
// 		return str.String()
// 	}
// }

// 类型分支
// 接口被以两种不同的方式使用。在第一个方式中,以io.Reader,io.Writer,fmt.Stringer,
// sort.Interface,http.Handler,和error为典型,一个接口的方法表达了实现这个接口的具体类
// 型间的相似性,但是隐藏了代表的细节和这些具体类型本身的操作。重点在于方法上,而不
// 是具体的类型上。
// 第二个方式利用一个接口值可以持有各种具体类型值的能力并且将这个接口认为是这些类型
// 的union(联合)。类型断言用来动态地区别这些类型并且对每一种情况都不一样。在这个方
// 式中,重点在于具体的类型满足这个接口,而不是在于接口的方法(如果它确实有一些的
// 话),并且没有任何的信息隐藏。我们将以这种方式使用的接口描述为discriminated
// unions(可辨识联合)。
// func sqlQuote(x interface{}) string {
// switch x.(type) {
// case nil: //	...
// case int, uint: //	...
// case bool: //	...
// case string: //	...
// default: //	...
// }
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

	_, err := os.Open("/no/such/file")
	fmt.Println(err)         //	open /no/such/file: no such file or directory
	fmt.Printf("%#v\n", err) // &os.PathError{Op:"open", Path:"/no/such/file", Err:0x2}

	fmt.Println(os.IsNotExist(err)) // true
	// 如果错误消息结合成一个更大的字符串,当然PathError的结构就不再为人所知,例如通过一
	// 个对fmt.Errorf函数的调用。区别错误通常必须在失败操作后,错误传回调用者前进行。

}
