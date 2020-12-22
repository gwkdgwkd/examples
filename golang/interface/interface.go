package main

import "fmt"
import "io"
import "bytes"
import "flag"
// import "time"

// 一个具体的类型可以准确的描述它所代表的值,并且展示出对类型本身的一些操作方式:
// 就像数字类型的算术操作,切片类型的取下标、添加元素和范围获取操作。具体的类型
// 还可以通过它的内置方法提供额外的行为操作。总的来说,当你拿到一个具体的类型时
// 你就知道它的本身是什么和你可以用它来做什么。
// 在Go语言中还存在着另外一种类型:接口类型。接口类型是一种抽象的类型。它不会暴露出
// 它所代表的对象的内部值的结构和这个对象支持的基础操作的集合;它们只会展示出它们自
// 己的方法。也就是说当你有看到一个接口类型的值时,你不知道它是什么,唯一知道的就是
// 可以通过它的方法来做什么。

// 接口类型具体描述了一系列方法的集合,一个实现了这些方法的具体类型是这个接口类型的实例。

// io.Writer类型是用的最广泛的接口之一,因为它提供了所有的类型写入bytes的抽象,包括文
// 件类型,内存缓冲区,网络链接,HTTP客户端,压缩工具,哈希等等。io包中定义了很多其
// 它有用的接口类型。Reader可以代表任意可以读取bytes的类型,Closer可以是任意可以关闭
// 的值,例如一个文件或是网络链接。
// package	io
// type Reader interface {
// 	Read(p []byte) (n int, err error)
// }
// type Closer	interface {
// 	Close()	error
// }

// 有些新的接口类型通过组合已经有的接口来定义:
// 接口内嵌
type ReadWriter1 interface {
	io.Reader
	io.Writer
}
// 不使用内嵌来声明io.Writer接口
type ReadWriter2 interface {
	Read(p []byte) (n int, err error)
	Write(p	[]byte)	(n int, err error)
}
// 使用种混合的风格:
type ReadWriter3 interface {
	Read(p []byte) (n int, err error)
	io.Writer
}
// ReadWriter三种定义方式相同。方法的顺序变化也没有影响,唯一重要的就是这个集合里面的方法。
type ReadWriteCloser interface {
	io.Reader
	io.Writer
	io.Closer
}

// 一个类型如果拥有一个接口需要的所有方法,那么这个类型就实现了这个接口。

// 因为ReadWriter和ReadWriteCloser包含所有Writer的方法,所以任何实现了ReadWriter和
// ReadWriteCloser的类型必定也实现了Writer接口

// 对于每一个命名过的具体类型T;它一些方法的接收者是类型T本身然而另一些则是一个*T的指针。
// 在T类型的参数上调用一个*T的方法是合法的,只要这个参数是一个变量;编译器隐式的获取了它的地址。
// 但这仅仅是一个语法糖:T类型的值不拥有所有*T指针的方法,那这样它就可能只实现更少的接口。
type IntSet	struct {}
func (*IntSet) String()	string {
	return "test"
}

// 非空的接口类型比如io.Writer经常被指针类型实现,尤其当一个或多个接口方法像Write方法
// 那样隐式的给接收者带来变化的时候。一个结构体的指针是非常常见的承载方法的类型。
// 但是并不意味着只有指针类型满足接口类型,甚至连一些有设置方法的接口类型也可能会被
// Go语言中其它的引用类型实现。

// var period = flag.Duration("period", 1*time.Second, "sleep period")

type Celsius float64 //	摄氏温度
type Fahrenheit	float64	// 华氏温度
func CToF(c	Celsius) Fahrenheit	{ return Fahrenheit(c*9/5 +	32) }
func FToC(f	Fahrenheit)	Celsius	{ return Celsius((f-32)*5/9) }
func (c	Celsius) String() string { return fmt.Sprintf("%g°C", c) }
type Value interface {
	String() string
	Set(string)	error
}
type celsiusFlag struct{ Celsius }
func (f *celsiusFlag) Set(s	string)	error {
	var	unit string
	var	value float64
	fmt.Sscanf(s, "%f%s", &value, &unit) //	no error check needed
	switch unit	{
	case "C", "°C":
		f.Celsius = Celsius(value)
		return nil
	case "F", "°F":
		f.Celsius = FToC(Fahrenheit(value))
		return nil
	}
	return fmt.Errorf("invalid temperature %q",	s)
}
func CelsiusFlag(name string, value	Celsius, usage string) *Celsius	{
	f := celsiusFlag{value}
	flag.CommandLine.Var(&f, name, usage)
	return &f.Celsius
}

func main() {
	// 接口指定的规则非常简单:表达一个类型属于某个接口只要这个类型实现这个接口
	// var w io.Writer
	// w = os.Stdout // OK: *os.File has Write method
	// w = new(bytes.Buffer) // OK: *bytes.Buffer has Write method
	// w = time.Second // compile error: time.Duration	lacks Write	method
	// var rwc io.ReadWriteCloser
	// rwc = os.Stdout // OK: *os.File	has	Read, Write, Close methods
	// rwc = new(bytes.Buffer) // compile error: *bytes.Buffer	lacks Close method
	// 这个规则甚至适用于等式右边本身也是一个接口类型
	// w = rwc // OK: io.ReadWriteCloser has Write method
	// rwc = w // compile error: io.Writer lacks Close method

	// fmt.Println(IntSet{}.String()) // compile error: String requires *IntSet receiver
	var s IntSet
	fmt.Println(s.String()) // test
	fmt.Println((&s).String()) // test
	var d *IntSet
	d = &s
	fmt.Println(d.String()) // test

	// 只有*IntSet类型有String方法,所以也只有*IntSet类型实现了fmt.Stringer接口:
	var _ fmt.Stringer = &s	// OK
	// var _ fmt.Stringer = s // compile error: IntSet lacks String method

	// 接口类型封装和隐藏具体类型和它的值。即使具体类型有其它的方法也只有接口类型暴露出来的方法会被调用到
	// 一个有更多方法的接口类型,比如io.ReadWriter,和少一些方法的接口类型,例如io.Reader,
	// 进行对比;更多方法的接口类型会告诉我们更多关于它的值持有的信息,并且对实现它的类型要求更加严格。
	// interface{}类型,没有任何方法,interface{}被称为空接口类型是不可或缺的。因为空接口类型
	// 对实现它的类型没有要求,所以我们可以将任意一个值赋给空接口类型。
	var	any	interface{}
	any	= true
	any	= 12.34
	any	= "hello"
	any	= map[string]int{"one":	1}
	any	= new(bytes.Buffer)
	fmt.Printf("%#T\n", any)
	// 对于创建的一个interface{}值持有一个boolean,float,string,map,pointer,或者任意其它
	// 的类型;我们当然不能直接对它持有的值做操作,因为interface{}没有任何方法。

	// 在编译期断言一个*bytes.Buffer的值实现了io.Writer接口类型:
	// var w io.Writer = new(bytes.Buffer)
	// 因为任意*bytes.Buffer的值,甚至包括nil通过(*bytes.Buffer)(nil)进行显示的转换都实现
	// 了这个接口,所以我们不必分配一个新的变量。并且因为我们绝不会引用变量w,我们可以使用空标识符来进行代替。
	var	_ io.Writer = (*bytes.Buffer)(nil)

	// 默认情况下,休眠周期是一秒,但是可以通过-period这个命令行标记来控制。
	// flag.Parse()
	// fmt.Printf("Sleeping for %v...", *period)
	// time.Sleep(*period)
	// fmt.Println()

	var	temp = CelsiusFlag("temp", 20.0, "the temperature")
	flag.Parse()
	fmt.Println(*temp)
}