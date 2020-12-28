package main

import (
	"fmt"
	"sync"
)

// 在一个线性(就是说只有一个goroutine的)的程序中,程序的执行顺序只由程序的逻辑来决定。
// 例如,有一段语句序列,第一个在第二个之前(废话),以此类推。在有两个或更多
// goroutine的程序中,每一个goroutine内的语句也是按照既定的顺序去执行的,但是一般情况
// 下没法去知道分别位于两个goroutine的事件x和y的执行顺序,x是在y之前还是之后还是
// 同时发生是没法判断的。当能够没有办法自信地确认一个事件是在另一个事件的前面或
// 者后面发生的话,就说明x和y这两个事件是并发的。

// 一个函数在线性程序中可以正确地工作。如果在并发的情况下,这个函数依然可
// 以正确地工作的话,那么我们就说这个函数是并发安全的,并发安全的函数不需要额外的同
// 步工作。我们可以把这个概念概括为一个特定类型的一些方法和操作函数,如果这个类型是
// 并发安全的话,那么所有它的访问方法和操作就都是并发安全的。

// 导出包级别的函数一般情况下都是并发安全的。由于package级的变量没法被限制在单
// 一的gorouine,所以修改这些变量“必须”使用互斥条件。

// 一个函数在并发调用时没法工作的原因太多了,比如死锁(deadlock)、活锁(livelock)和饿死
// (resource starvation)。

// 竞争条件指的是程序在多个goroutine交叉执行操作时,没有给出正确的结果。竞争条件是很
// 恶劣的一种场景,因为这种问题会一直潜伏在你的程序里,然后在非常少见的时候蹦出来,
// 或许只是会在很大的负载时才会发生,又或许是会在使用了某一个编译器、某一种平台或者
// 某一种架构的时候才会出现。这些使得竞争条件带来的问题非常难以复现而且难以分析诊断。

// 有三种方式可以避免数据竞争:
// 第一种方法是不要去写变量。
// 第二种避免数据竞争的方法是,避免从多个goroutine访问变量。
// 由于其它的goroutine不能够直接访问变量,它们只能使用一个channel来发送给指定的
// goroutine请求来查询更新变量。这也就是Go的口头禅“不要使用共享数据来通信;使用通信来
// 共享数据”。一个提供对一个指定的变量通过channel来请求的goroutine叫做这个变量的监控
// (monitor)goroutine。
// 第三种避免数据竞争的方法是允许很多goroutine去访问变量,但是在同一个时刻最多只有一
// 个goroutine在访问。这种方式被称为“互斥”。

// 每次一个goroutine访问bank变量时,它都会调用mutex的Lock方法来获取一个互斥锁。如果其它
// 的goroutine已经获得了这个锁的话,这个操作会被阻塞直到其它goroutine调用了Unlock使该锁
// 变回可用状态。mutex会保护共享变量。惯例来说,被mutex所保护的变量是在mutex变量声明之后立
// 刻声明的。
// 在Lock和Unlock之间的代码段中的内容goroutine可以随便读取或者修改,这个代码段叫做临
// 界区。goroutine在结束后释放锁是必要的,无论以哪条路径通过函数都需要释放,即使是在
// 错误路径中,也要记得释放。
// var (
// 	mu      sync.Mutex // guards	balance
// 	balance int
// )
// func Deposit(amount int) {
// 	mu.Lock()
// 	balance = balance + amount
// 	mu.Unlock()
// }
// 关于Go的互斥量不能重入这一点我们有很充分的理由。互斥量的目的是为了确保共享变量在
// 程序执行时的关键点上能够保证不变性。不变性的其中之一是“没有goroutine访问共享变量”。
// 但实际上对于mutex保护的变量来说,不变性还包括其它方面。当一个goroutine获得了一个互
// 斥锁时,它会断定这种不变性能够被保持。其获取并保持锁期间,可能会去更新共享变量,
// 这样不变性只是短暂地被破坏。然而当其释放锁之后,它必须保证不变性已经恢复原样。尽
// 管一个可以重入的mutex也可以保证没有其它的goroutine在访问共享变量,但这种方式没法保
// 证这些变量额外的不变性。

// 允许多个只读操作并行执行,但写操作会完全互斥。这种锁叫作“多读单写”锁
// (multiple readers,single writer lock),Go语言提供的这样的锁是sync.RWMutex:
// var mu sync.RWMutex
// var balance int
// func Balance() int {
// 	mu.RLock() // readers lock
// 	defer mu.RUnlock()
// 	return balance
// }
// RLock只能在临界区共享变量没有任何写入操作时可用。
// RWMutex只有当获得锁的大部分goroutine都是读操作,而锁在竞争条件下,也就是说,
// goroutine们必须等待才能获取到锁的时候,RWMutex才是最能带来好处的。RWMutex需要更
// 复杂的内部记录,所以会让它比一般的无竞争锁的mutex慢一些。

// sync包提供了一个专门的方案来解决这种一次性初始化的问题:sync.Once。概念上来讲,
// 一次性的初始化需要一个互斥量mutex和一个boolean变量来记录初始化是不是已经完成了;
// 互斥量用来保护boolean变量和客户端数据结构。Do这个唯一的方法需要接收初始化函数作为其参数。
// var	loadIconsOnce sync.Once
// var	icons map[string]image.Image
// func Icon(name string) image.Image	{
// 	loadIconsOnce.Do(loadIcons)
// 	return icons[name]
// }
// 每一次对Do(loadIcons)的调用都会锁定mutex,并会检查boolean变量。在第一次调用时,变
// 量的值是false,Do会调用loadIcons并会将boolean设置为true。随后的调用什么都不会做,
// 但是mutex同步会保证loadIcons对内存(这里其实就是指icons变量)产生的效果能够对所有
// goroutine可见。用这种方式来使用sync.Once的话,我们能够避免在变量被构建完成之前和其
// 它goroutine共享该变量。

// 竞争条件检测:
// 即使我们小心到不能再小心,但在并发程序中犯错还是太容易了。幸运的是,Go的runtime和
// 工具链为我们装备了一个复杂但好用的动态分析工具,竞争检查器(the race detector)。
// 只要在go build,go run或者go test命令后面加上-race的flag,就会使编译器创建一个你的应
// 用的“修改”版或者一个附带了能够记录所有运行期对共享变量访问工具的test,并且会记录下
// 每一个读或者写共享变量的goroutine的身份信息。另外,修改版的程序会记录下所有的同步
// 事件,比如go语句,channel操作,以及对(*sync.Mutex).Lock, (*sync.WaitGroup).Wait等
// 等的调用。
// 竞争检查器会检查这些事件,会寻找在哪一个goroutine中出现了这样的case,例如其读或者
// 写了一个共享变量,这个共享变量是被另一个goroutine在没有进行干预同步操作便直接写入
// 的。这种情况也就表明了是对一个共享变量的并发访问,即数据竞争。这个工具会打印一份
// 报告,内容包含变量身份,读取和写入的goroutine中活跃的函数的调用栈。这些信息在定位
// 问题时通常很有用。
// 竞争检查器会报告所有的已经发生的数据竞争。然而,它只能检测到运行时的竞争条件;并
// 不能证明之后不会发生数据竞争。所以为了使结果尽量正确,请保证你的测试并发地覆盖到
// 了你到包。
// 由于需要额外的记录,因此构建时加了竞争检测的程序跑起来会慢一些,且需要更大的内
// 存,即使是这样,这些代价对于很多生产环境的工作来说还是可以接受的。对于一些偶发的
// 竞争条件来说,让竞争检查器来干活可以节省无数日夜的debugging。(多少服务端C和
// C艹程序员为此竞折腰)

var waitGroutp = sync.WaitGroup{}

func main() {
	var x, y int
	go func() {
		x = 1                   // A1
		fmt.Print("y:", y, " ") // A2
		waitGroutp.Done()
	}()
	waitGroutp.Add(1)
	go func() {
		y = 1                   // B1
		fmt.Print("x:", x, " ") // B2
		waitGroutp.Done()
	}()

	waitGroutp.Add(1)

	waitGroutp.Wait()
	// 可能：
	// y:0	x:1
	// x:0	y:1
	// x:1	y:1
	// y:1	x:1
	// x:0	y:0
	// y:0	x:0
	// 所有并发的问题都可以用一致的、简单的既定的模式来规避。所以可能的话,将变量限定在
	// goroutine内部;如果是多个goroutine都需要访问的变量,使用互斥条件来访问。

}
