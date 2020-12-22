package main

import (
	"fmt"
	"image/color"
	"math"
	"sync"
)

// 从90年代早期开始,面向对象编程(OOP)就成为了称霸工程界和教育界的编程范式,所以之
// 后几乎所有大规模被应用的语言都包含了对OOP的支持,go语言也不例外。
// 尽管没有被大众所接受的明确的OOP的定义,从我们的理解来讲,一个对象其实也就是一个
// 简单的值或者一个变量,在这个对象中会包含一些方法,而一个方法则是一个一个和特殊类
// 型关联的函数。一个面向对象的程序会用方法来表达其属性和对应的操作,这样使用这个对
// 象的用户就不需要直接去操作对象,而是借助方法来做这些事情。

// 在函数声明时,在其名字之前放上一个变量,即是一个方法。
type Point struct{ X, Y float64 }

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X-p.X, q.Y-p.Y)
}

// Point类型的方法
func (p Point) Distance(q Point) float64 {
	// 参数p,叫做方法的接收器(receiver),早期的面向对象语言留下的遗产将调用一个方法称为“向一个对象发送消息”。
	// 在Go语言中,并不会像其它语言那样用this或者self作为接收器;可以任意的选择接
	// 收器的名字。由于接收器的名字经常会被使用到,所以保持其在方法间传递时的一致性和简
	// 短性是不错的主意。建议使用其类型的第一个字母,这里使用了Point的首字母p。
	return math.Hypot(q.X-p.X, q.Y-p.Y)
}

// 基于指针对象的方法
// 一般会约定如果Point这个类有一个指针作为接收器的方法,那么所有Point
// 的方法都必须有一个指针接收器,即使是那些并不需要这个指针接收器的函数。
func (p *Point) ScaleBy(factor float64) {
	p.X *= factor
	p.Y *= factor
}

// 只有类型(Point)和指向他们的指针(*Point),才是可能会出现在接收器声明里的两种接收
// 器。此外,为了避免歧义,在声明方法时,如果一个类型名本身是一个指针的话,是不允许其出现在接收器中的
// type P *int
// func (P) f() { /*	...	*/ } //	compile	error:invalid receiver type P (P is a pointer type)

// 在能够给任意类型定义方法这一点上,Go和很多其它的面向对象的语言不太一样。因此
// 在Go语言里,我们为一些简单的数值、字符串、slice、map来定义一些附加行为很方便。
// 可以给同一个包内的任意命名类型定义方法,只要这个命名类型的底层类型(底层类型是指
// []Point这个slice,Path就是命名类型)不是指针或者interface。
type Path []Point

func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

// 在声明一个method的receiver该是指针还是非指针类型时,你需要考虑两方面的内部,第
// 一方面是这个对象本身是不是特别大,如果声明为非指针变量时,调用会产生一次拷
// 贝;第二方面是如果你用指针类型作为receiver,那么你一定要注意,这种指针类型指向
// 的始终是一块内存地址,就算你对其进行了拷贝。

// 就像一些函数允许nil指针作为参数一样,方法理论上也可以用nil指针作为其接收器,尤其当
// nil对于对象来说是合法的零值时,比如map或者slice。
// 当你定义一个允许nil作为接收器值的方法的类型时,在类型前面的注释中指出nil变量代表的
// 意义是很有必要的

// Point类的方法也被引入了ColoredPoint。
type ColoredPoint struct {
	Point
	Color color.RGBA
}

// 内嵌字段会指导编译器去生成额外的包装方法来委托已经声明好的方法,和下面的形式是等价的:
// func (p ColoredPoint) Distance(q Point) float64 {
// 	return p.Point.Distance(q)
// }
// func (p *ColoredPoint) ScaleBy(factor float64) {
// 	p.Point.ScaleBy(factor)
// }

// 在类型中内嵌的匿名字段也可能是一个命名类型的指针,这种情况下字段和方法会被间接地
// 引入到当前的类型中
type ColoredPoint1 struct {
	*Point
	Color color.RGBA
}

// 一个struct类型也可能会有多个匿名字段。
// 这种类型的值便会拥有Point和RGBA类型的所有方法,以及直接定义在ColoredPoint中的
// 方法。当编译器解析一个选择器到方法时,比如p.ScaleBy,它会首先去找直接定义在这个类
// 型里的ScaleBy方法,然后找被ColoredPoint的内嵌字段们引入的方法,然后去找Point和
// RGBA的内嵌字段引入的方法,然后一直递归向下找。如果选择器有二义性的话编译器会报
// 错,比如你在同一级里有两个同名的方法。
type ColoredPoint2 struct {
	Point
	color.RGBA
}

// 方法只能在命名类型(像Point)或者指向类型的指针上定义,但是多亏了内嵌,有些时候
// 给匿名struct类型来定义方法也有了手段。
var cache = struct {
	sync.Mutex
	mapping map[string]string
}{
	mapping: make(map[string]string),
}

func Lookup(key string) string {
	cache.Lock()
	v := cache.mapping[key]
	cache.Unlock()
	return v
}

func main() {
	// 在方法调用过程中,接收器参数一般会在方法名之前出现。这和方法声明是一样的,都是接
	// 收器参数在方法名字之前。
	p := Point{1, 2}
	q := Point{4, 6}
	// 包级别的函数geometry.Distance
	fmt.Println(Distance(p, q)) // 5,function call
	// 调用的是Point类下声明的Point.Distance方法
	fmt.Println(p.Distance(q)) // 5,method call
	// p.Distance的表达式叫做选择器,因为他会选择合适的对应p这个对象的Distance方法来
	// 执行。选择器也会被用来选择一个struct类型的字段,比如p.X。由于方法和字段都是在同一
	// 命名空间,所以如果我们在这里声明一个X方法的话,编译器会报错,因为在调用p.X时会有歧义

	// 因为每种类型都有其方法的命名空间,不同的Distance调用指向了不同类型里的Distance方法。
	perim := Path{
		{1, 1},
		{5, 1},
		{5, 4},
		{1, 1},
	}
	fmt.Println(perim.Distance()) // 12
	// 编译器会根据方法的名字以及接收器来决定具体调用的是哪一个函数。

	// 对于一个给定的类型,其内部的方法都必须有唯一的方法名,但是不同的类型却可以有同样
	// 的方法名,比如我们这里Point和Path就都有Distance这个名字的方法;所以没有必要非
	// 在方法名之前加类型名来消除歧义,比如PathDistance。这里已经看到了方法比之函数
	// 的一些好处:方法名可以简短。当在包外调用的时候这种好处就会被放大,因为可以使用这个短
	// 名字,而可以省略掉包的名字

	// 调用指针类型方法,只要提供一个Point类型的指针即可:
	r := &Point{1, 2}
	r.ScaleBy(2)
	fmt.Println(*r) // {2 4}
	p1 := Point{1, 2}
	pptr := &p1
	pptr.ScaleBy(2)
	fmt.Println(p1) // {2 4}
	p2 := Point{1, 2}
	(&p2).ScaleBy(2)
	fmt.Println(p2) // {2 4}
	// 如果接收器p是一个Point类型的变量,并且其方法需要一个Point指针作为接收器,可以：
	p3 := Point{1, 2}
	p3.ScaleBy(2)
	fmt.Println(p3) // {2 4}
	// 编译器会隐式地帮我们用&p去调用ScaleBy这个方法。这种简写方法只适用于“变量”,包括
	// struct里的字段比如p.X,以及array和slice内的元素比如perim[0]。
	// 不能通过一个无法取到地址的接收器来调用指针方法,比如临时变量的内存地址就无法获取得到:
	// Point{1, 2}.ScaleBy(2) // compile error: cannot take the address of Point literal

	// 可以用一个*Point这样的接收器来调用Point的方法,因为我们可以通过地址来找到
	// 这个变量,只要用解引用符号*来取到该变量即可。编译器在这里也会给我们隐式地插
	// 入*这个操作符,所以下面这两种写法等价的:
	pptr.Distance(q)
	(*pptr).Distance(q)

	// 不管你的method的receiver是指针类型还是非指针类型,都是可以通过指针/非指针类型
	// 进行调用的,编译器会帮你做类型转换。

	red := color.RGBA{255, 0, 0, 255}
	blue := color.RGBA{0, 0, 255, 255}
	var dd = ColoredPoint{Point{1, 1}, red}
	var gg = ColoredPoint{Point{5, 4}, blue}
	fmt.Println(dd.Distance(gg.Point)) // 5
	dd.ScaleBy(2)
	gg.ScaleBy(2)
	fmt.Println(dd.Distance(gg.Point)) // 10
	// gg.Distance(dd) // compile error: cannot use dd (type ColoredPoint) as type Point in argument to gg.Point.Distance

	nn := ColoredPoint1{&Point{1, 1}, red}
	mm := ColoredPoint1{&Point{5, 4}, blue}
	fmt.Println(nn.Distance(*mm.Point)) // 5
	mm.Point = nn.Point                 // mm和nn共享指针
	nn.ScaleBy(2)
	fmt.Println(*nn.Point, *mm.Point) // {2 2} {2 2}
}
