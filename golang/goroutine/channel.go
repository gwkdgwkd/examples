package main

import "fmt"

// 如果说goroutine是Go语言程序的并发体的话,那么channels则是它们之间的通信机制。一个
// channel是一个通信机制,它可以让一个goroutine通过它给另一个goroutine发送值信息。每个
// channel都有一个特殊的类型,也就是channels可发送数据的类型。一个可以发送int类型数据
// 的channel一般写为chan int。

func main() {
	// 使用内置的make函数,我们可以创建一个channel:
	ch := make(chan int)
	fmt.Printf("%v %T\n", ch, ch) // 0xc0000240c0 chan int

	// 使用内置的close函数就可以关闭一个channel:
	close(ch)

	// 和map类似,channel也对应一个make创建的底层数据结构的引用。当我们复制一个channel
	// 或用于函数参数传递时,我们只是拷贝了一个channel引用,因此调用者和被调用者将引用同
	// 一个channel对象。和其它的引用类型一样,channel的零值也是nil。

	// 两个相同类型的channel可以使用==运算符比较。如果两个channel引用的是相同的对象,那
	// 么比较的结果为真。一个channel也可以和nil进行比较。

	// 一个channel有发送和接受两个主要操作,都是通信行为。一个发送语句将一个值从一个
	// goroutine通过channel发送到另一个执行接收操作的goroutine。发送和接收两个操作都使
	// 用<-运算符。在发送语句中,<-运算符分割channel和要发送的值。在接收语句中,<-运
	// 算符写在channel对象之前。一个不使用接收结果的接收操作也是合法的。

	// 以最简单方式调用make函数创建的是一个无缓存的channel,但是我们也可以指定第二个整
	// 型参数,对应channel的容量。如果channel的容量大于零,那么该channel就是带缓存的channel。
	ch = make(chan int)    // unbuffered channel
	ch = make(chan int, 0) // unbuffered channel
	ch = make(chan int, 3) // buffered channel with capacity 3

	// 一个基于无缓存Channels的发送操作将导致发送者goroutine阻塞,直到另一个goroutine在相
	// 同的Channels上执行接收操作,当发送的值通过Channels成功传输之后,两个goroutine可以
	// 继续执行后面的语句。反之,如果接收操作先发生,那么接收者goroutine也将阻塞,直到有
	// 另一个goroutine在相同的Channels上执行发送操作。
	// 基于无缓存Channels的发送和接收操作将导致两个goroutine做一次同步操作。因为这个原
	// 因,无缓存Channels有时候也被称为同步Channels。当通过一个无缓存Channels发送数据
	// 时,接收者收到数据发生在唤醒发送者goroutine之前(happens before,这是Go语言
	// 并发内存模型的一个关键术语!)。

	// 说x事件和y事件是并发的。这并不是意味着x事件和y事件就一定是同时发生的,我们只是不能确
	// 定这两个事件发生的先后顺序。

	// 基于channels发送消息有两个重要方面。首先每个消息都有一个值,但是有时候通讯的事实
	// 和发生的时刻也同样重要。当我们更希望强调通讯发生的时刻时,我们将它称为消息事件。
	// 有些消息事件并不携带额外的信息,它仅仅是用作两个goroutine之间的同步,这时候我们可
	// 以用struct{}空结构体作为channels元素的类型,虽然也可以使用bool或int类型实现同样的
	// 功能, done<-1语句也比done<-struct{}{}更短。

	// Channels也可以用于将多个goroutine连接在一起,一个Channel的输出作为下一个Channel的
	// 输入。这种串联的Channels就是所谓的管道(pipeline)。
	// naturals := make(chan int)
	// squares := make(chan int)
	// // Counter
	// go func() {
	// 	for x := 0; ; x++ {
	// 		naturals <- x
	// 	}
	// }()
	// // Squarer
	// go func() {
	// 	for {
	// 		x := <-naturals
	// 		squares <- x * x
	// 	}
	// }()
	// // Printer (in main goroutine)
	// for {
	// 	fmt.Println(<-squares)
	// }
}
