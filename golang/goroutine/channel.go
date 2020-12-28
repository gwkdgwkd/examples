package main

import "fmt"

// 如果说goroutine是Go语言程序的并发体的话,那么channels则是它们之间的通信机制。一个
// channel是一个通信机制,它可以让一个goroutine通过它给另一个goroutine发送值信息。每个
// channel都有一个特殊的类型,也就是channels可发送数据的类型。一个可以发送int类型数据
// 的channel一般写为chan int。

func counter(out chan<- int) {
	for x := 0; x < 100; x++ {
		out <- x
	}
	close(out)
}
func squarer(out chan<- int, in <-chan int) {
	for v := range in {
		out <- v * v
	}
	close(out)
}
func printer(in <-chan int) {
	for v := range in {
		fmt.Println(v)
	}
}

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
	naturals := make(chan int)
	squares := make(chan int)
	// Counter
	go func() {
		for x := 0; x < 100; x++ {
			naturals <- x
		}
		close(naturals)
	}()
	// Squarer
	go func() {
		for x := range naturals {
			squares <- x * x
		}
		close(squares)
	}()
	// Printer (in main goroutine)
	for x := range squares {
		fmt.Println(x)
	}
	// 如果发送者知道,没有更多的值需要发送到channel的话,那么让接收者也能及时知道没有多
	// 余的值可接收将是有用的,因为接收者可以停止不必要的接收等待。这可以通过内置的close
	// 函数来关闭channel实现。
	// 当一个channel被关闭后,再向该channel发送数据将导致panic异常。当一个被关闭的channel
	// 中已经发送的数据都被成功接收后,后续的接收操作将不再阻塞,它们会立即返回一个零值。
	// 没有办法直接测试一个channel是否被关闭,但是接收操作有一个变体形式:它多接收一个结
	// 果,多接收的第二个结果是一个布尔值ok,ture表示成功从channels接收到值,false表示
	// channels已经被关闭并且里面没有值可接收。
	// x, ok := <-naturals
	// if !ok {
	// 	break // channel was closed	and	drained
	// }
	// 其实并不需要关闭每一个channel。只要当需要告诉接收者goroutine,所有的数据已经全部
	// 发送时才需要关闭channel。不管一个channel是否被关闭,当它没有被引用时将会被Go语言
	// 的垃圾自动回收器回收。(不要将关闭一个打开文件的操作和关闭一个channel操作混淆。对
	// 于每个打开的文件,都需要在不使用的使用调用对应的Close方法来关闭文件。)
	// 试图重复关闭一个channel将导致panic异常,试图关闭一个nil值的channel也将导致panic异常。
	// close(naturals) // panic: close of closed channel
	// var n chan int
	// close(n) // panic: close of closed channel

	// Go语言的类型系统提供了单方向的channel类型,分别用于
	// 只发送或只接收的channel。类型chan<-int表示一个只发送int的channel,只能发送不能接
	// 收。相反,类型<-chan int表示一个只接收int的channel,只能接收不能发送。(箭头<-和
	// 关键字chan的相对位置表明了channel的方向。)这种限制将在编译期检测。
	// 因为关闭操作只用于断言不再向channel发送新的数据,所以只有在发送者所在的goroutine才
	// 会调用close函数,因此对一个只接收的channel调用close将是一个编译错误。
	naturals1 := make(chan int)
	squares1 := make(chan int)
	go counter(naturals1)
	go squarer(squares1, naturals1)
	printer(squares1)
	// 调用counter(naturals)将导致将chan int类型的naturals隐式地转换为chan<-int类型只发
	// 送型的channel。调用printer(squares)也会导致相似的隐式转换,这一次是转换为<-chan
	// int	类型只接收型的channel。任何双向channel向单向channel变量的赋值操作都将导致该隐
	// 式转换。这里并没有反向转换的语法:也就是不能将一个类似chan<-int类型的单向型的
	// channel转换为chan int类型的双向型的channel。

	// 带缓存的Channel内部持有一个元素队列。队列的最大容量是在调用make函数创建channel时
	// 通过第二个参数指定的。
	// 向缓存Channel的发送操作就是向内部缓存队列的尾部插入元素,接收操作则是从队列的头部
	// 删除元素。如果内部缓存队列是满的,那么发送操作将阻塞直到因另一个goroutine执行接收
	// 操作而释放了新的队列空间。相反,如果channel是空的,接收操作将阻塞直到有另一个
	// goroutine执行发送操作而向队列插入元素。
	// 如果缓存不满，发送不阻塞；如果缓存不空，那么接收不阻塞。通过这种方式,channel的缓存
	// 队列解耦了接收和发送的goroutine。

	// 在某些特殊情况下,程序可能需要知道channel内部缓存的容量,可以用内置的cap函数获取:
	fmt.Println(cap(ch)) // 3
	// 同样,对于内置的len函数,如果传入的是channel,那么将返回channel内部缓存队列中有效
	// 元素的个数。因为在并发程序中该信息会随着接收操作而失效,但是它对某些故障诊断和性
	// 能优化会有帮助。
	fmt.Println(len(ch)) // 0

	// 如果使用了无缓存的channel,那么两个慢的goroutines将会因为没有人接收而被永远卡
	// 住。这种情况,称为goroutines泄漏,这将是一个BUG。和垃圾变量不同,泄漏的goroutines
	// 并不会被自动回收,因此确保每个不再需要的goroutine能正常退出是重要的。
	// 关于无缓存或带缓存channels之间的选择,或者是带缓存channels的容量大小的选择,都可
	// 能影响程序的正确性。无缓存channel更强地保证了每个发送操作与相应的同步接收操作;但
	// 是对于带缓存channel,这些操作是解耦的。同样,即使我们知道将要发送到一个channel的
	// 信息的数量上限,创建一个对应容量大小的带缓存channel也是不现实的,因为这要求在执行
	// 任何接收操作之前缓存所有已经发送的值。如果未能分配足够的缓冲将导致程序死锁。
	// Channel的缓存也可能影响程序的性能。

	// 无法做到从每一个channel中接收信息,如果我们这么做的话,如果第一个
	// channel中没有事件发过来那么程序就会立刻被阻塞,这样我们就无法收到第二个channel中
	// 发过来的事件。这时候需要多路复用(multiplex)这些操作了
	// select {
	// case <-ch1:
	// case x := <-ch2:
	// case ch3 <- y:
	// default:
	// }
	// 和switch语句稍微有点相似,也会有几个case和最后的default选择支。每一个case代表
	// 一个通信操作(在某个channel上进行发送或者接收)并且会包含一些语句组成的一个语句块。
	// 一个接收表达式可能只包含接收表达式自身(不把接收到的值赋值给变量什么的),就像上面的
	// 第一个case,或者包含在一个简短的变量声明中,像第二个case里一样;第二种形式让你能够
	// 引用接收到的值。
	// select会等待case中有能够执行的case时去执行。当条件满足时,select才会去通信并执行
	// case之后的语句;这时候其它通信是不会执行的。一个没有任何case的select语句写作
	// select{},会永远地等待下去。

	// channel的零值是nil。也许会让你觉得比较奇怪,nil的channel有时候也是有一些用处的。因
	// 为对一个nil的channel发送和接收操作会永远阻塞,在select语句中操作nil的channel永远都不
	// 会被select到。这使得可以用nil来激活或者禁用case,来达成处理其它输入或输出事件时超时和取消的逻辑。

	// Go语言并没有提供在一个goroutine中终止另一个goroutine的方法,由于这样会导致goroutine
	// 之间的共享变量落在未定义的状态上。
	// 可以向channel里发送了一个简单的值,在goroutine中会把这个值理解为自己的退出
	// 信号。但是如果想要退出两个或者任意多个goroutine怎么办呢?
	// 一种可能的手段是向abort的channel里发送和goroutine数目一样多的事件来退出它们。如果
	// 这些goroutine中已经有一些自己退出了,那么会导致我们的channel里的事件数比goroutine还
	// 多,这样导致我们的发送直接被阻塞。另一方面,如果这些goroutine又生成了其它的
	// goroutine,我们的channel里的数目又太少了,所以有些goroutine可能会无法接收到退出消
	// 息。一般情况下我们是很难知道在某一个时刻具体有多少个goroutine在运行着的。另外,当
	// 一个goroutine从abort	channel中接收到一个值的时候,他会消费掉这个值,这样其它的
	// goroutine就没法看到这条信息。为了能够达到我们退出goroutine的目的,我们需要更靠谱的
	// 策略,来通过一个channel把消息广播出去,这样goroutine们能够看到这条事件消息,并且在
	// 事件完成之后,可以知道这件事已经发生过了。
	// 广播机制:不要向channel发送值,而是用关闭一个channel来进行广播。
	// 创建一个退出的channel,这个channel不会向其中发送任何值,但其所在的闭包内要写明程序需要退出。
}
