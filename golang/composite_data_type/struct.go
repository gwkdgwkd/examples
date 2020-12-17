package main

import (
	"fmt"
	"time"
)

type Employee struct {
	ID int
	// 通常一行对应一个结构体成员,成员的名字在前类型在后,不过如果相邻的成员类型如果相
	// 同的话可以被合并到一行
	Name, Address string
	DoB           time.Time
	Position      string
	Salary        int
	ManagerID     int
}

var dilbert Employee

func main() {
	// 结构体是一种聚合的数据类型,是由零个或多个任意类型的值聚合成的实体。每个值称为结
	// 构体的成员。
	// 可以作为一个整体单元被复制,作为函数的参数或返回值,或者是被存储到数组中。

	// dilbert结构体变量的成员可以通过点操作符访问
	dilbert.Salary -= 5000
	// 对成员取地址,然后通过指针访问
	position := &dilbert.Position
	*position = "Senior " + *position
	var employeeOfTheMonth *Employee = &dilbert
	employeeOfTheMonth.Position += " (proactive team"
	(*employeeOfTheMonth).Position += " player)"

	fmt.Printf("%v\n", dilbert) // {0   0001-01-01 00:00:00 +0000 UTC Senior  (proactive team player) -5000 0}

	// 结构体成员的输入顺序也有重要的意义,顺序不同是不同的结构体。

	// 如果结构体成员名字是以大写字母开头的,那么该成员就是导出的;这是Go语言导出规则决
	// 定的。一个结构体可能同时包含导出和未导出的成员。

	// 一个命名为S的结构体类型将不能再包含S类型的成员:因为一个聚合的值不能包含它自身。
	// (该限制同样适应于数组)但是S类型的结构体可以包含*S指针类型的成员,这可以让我
	// 们创建递归的数据结构,比如链表和树结构等。

	// 结构体类型的零值是每个成员都是零值。通常会将零值作为最合理的默认值。

	// 如果结构体没有任何成员的话就是空结构体,写作struct{}。它的大小为0,也不包含任何信
	// 息,但是有时候依然是有价值的。有些Go语言程序员用map来模拟set数据结构时,用它来代
	// 替map中布尔类型的value,只是强调key的重要性,但是因为节约的空间有限,而且语法比较
	// 复杂,所以我们通常会避免这样的用法。
	// seen := make(map[string]struct{})
	// if _, ok := seen[s]; !ok {
	// 	seen[s] = struct{}{}
	// }

	// 结构体值也可以用结构体字面值表示,结构体字面值可以指定每个成员的值。
	// 有两种形式的结构体字面值语法:
	// 第一种写法,要求以结构体成员定义的顺序为每个结构体成员指定一个字面值。
	// 它要求写代码和读代码的人要记住结构体的每个成员的类型和顺序,不过结构
	// 体成员有细微的调整就可能导致上述代码不能编译。一般只在定义结构体的包
	// 内部使用,或者是在较小的结构体中使用,这些结构体的成员排列比较规则
	p := Point{1, 2}
	fmt.Printf("%#v\n", p) // main.Point{X:1, Y:2}
	// 第二种写法(更常用),以成员名字和相应的值来初始化,可以包含部分或全部的成员
	// 在这种形式的结构体字面值写法中,如果成员被忽略的话将默认用零值。因为,提供了成员
	// 的名字,所有成员出现的顺序并不重要。
	q := Point{X: 5}
	fmt.Printf("%#v\n", q) // main.Point{X:5, Y:0}
	// 两种不同形式的写法不能混合使用。而且,你不能企图在外部包中用第一种顺序赋值的技巧
	// 来偷偷地初始化结构体中未导出的成员。
	// package	p
	// type T struct{a, b int} // a and b are not exported
	// package	q
	// import "p"
	// var	_ =	p.T{a: 1, b: 2}	// compile error: can't	reference a, b
	// var	_ =	p.T{1, 2} // compile error:	can't reference	a, b

	fmt.Println(Scale(Point{1, 2}, 5)) // {5 10}

	// 如果考虑效率的话,较大的结构体通常会用指针的方式传入和返回

	// 如果要在函数内部修改结构体成员的话,用指针传入是必须的;因为在Go语言中,所有的函
	// 数参数都是值拷贝传入的,函数参数将不再是函数调用时的原始变量。

	// 结构体通常通过指针处理,可以用下面的写法来创建并初始化一个结构体变量,并返回
	// 结构体的地址:
	pp := &Point{1, 2} // 可以直接在表达式中使用,比如一个函数调用。
	// 等价于：
	pq := new(Point)
	*pq = Point{1, 2}
	fmt.Printf("%#v, %#v\n", pp, pq) // &main.Point{X:1, Y:2}, &main.Point{X:1, Y:2}

	// 如果结构体的全部成员都是可以比较的,那么结构体也是可以比较的,那样的话两个结构体
	// 将可以使用==或!=运算符进行比较。相等比较运算符==将比较两个结构体的每个成员。
	p1 := Point{1, 2}
	q1 := Point{2, 1}
	fmt.Println(p1.X == q1.X && p1.Y == q1.Y) // false
	fmt.Println(p1 == q1)                     // false

	// 可比较的结构体类型和其他可比较的类型一样,可以用于map的key类型。
	type address struct {
		hostname string
		port     int
	}
	hits := make(map[address]int)
	hits[address{"golang.org", 443}]++
	fmt.Println(hits[address{"golang.org", 443}]) // 1

	// 只声明一个成员对应的数据类型而不指名成员的名字;这类成员就叫匿名成员。
	// 匿名成员的数据类型必须是命名的类型或指向一个命名的类型的指针。
	type Circle struct {
		Point
		Radius int
	}
	type Wheel struct {
		Circle
		Spokes int
	}

	// 得意于匿名嵌入的特性,可以直接访问叶子属性而不需要给出完整的路径
	var w Wheel
	w.X = 8      //	w.Circle.Point.X = 8
	w.Y = 8      //	w.Circle.Point.Y = 8
	w.Radius = 5 //	w.Circle.Radius	= 5
	w.Spokes = 20
	fmt.Printf("%#v\n", w) // main.Wheel{Circle:main.Circle{Point:main.Point{X:8, Y:8}, Radius:5}, Spokes:20}
	// 显式形式访问这些叶子成员的语法依然有效,因此匿名成员并不是真的无法访问了。
	// 其中匿名成员Circle和Point都有自己的名字——就是命名的类型名字——但是
	// 这些名字在点操作符中是可选的。我们在访问子成员的时候可以忽略任何匿名成员部分。

	// 结构体字面值并没有简短表示匿名成员的语法
	// w = Wheel{8, 8, 5, 20}                       // compile error: unknown fields
	// w = Wheel{X: 8, Y: 8, Radius: 5, Spokes: 20} // compile error: unknown fields

	// 结构体字面值必须遵循形状类型声明时的结构,所以我们只能用下面的两种语法,它们彼此是等价的:
	w = Wheel{Circle{Point{8, 8}, 5}, 20}
	w = Wheel{
		Circle: Circle{
			Point:  Point{X: 8, Y: 8},
			Radius: 5,
		},
		Spokes: 20, // NOTE: trailing comma	necessary here (and at Radius)
	}

	// 因为匿名成员也有一个隐式的名字,因此不能同时包含两个类型相同的匿名成员,这会导致
	// 名字冲突。同时,因为成员的名字是由其类型隐式地决定的,所有匿名成员也有可见性的规则约束。
	// 即使它们不导出(小写)，依然可以用简短形式访问匿名成员嵌套的成员，但是在包外部,因为没有
	// 导出不能访问它们的成员,因此简短的匿名成员访问语法也是禁止的。

	// 匿名成员特性只是对访问嵌套成员的点运算符提供了简短的语法糖
	// 匿名成员并不要求是结构体类型;其实任何命名的类型都可以作为结构体的匿名成员。
	// 为什么要嵌入一个没有任何子成员类型的匿名成员类型呢?
	// 答案是匿名类型的方法集。简短的点运算符语法可以用于选择匿名成员嵌套的成员,也可以
	// 用于访问它们的方法。实际上,外层的结构体不仅仅是获得了匿名成员类型的所有成员,而
	// 且也获得了该类型导出的全部的方法。这个机制可以用于将一个有简单行为的对象组合成有
	// 复杂行为的对象。组合是Go语言中面向对象编程的核心。
}

type Point struct{ X, Y int }

// 结构体可以作为函数的参数和返回值。
func Scale(p Point, factor int) Point {
	return Point{p.X * factor, p.Y * factor}
}

// 使用一个二叉树来实现一个插入排序:
type tree struct {
	value       int
	left, right *tree
}

func Sort(values []int) {
	var root *tree
	for _, v := range values {
		root = add(root, v)
	}
	appendValues(values[:0], root)
}

func appendValues(values []int, t *tree) []int {
	if t != nil {
		values = appendValues(values, t.left)
		values = append(values, t.value)
		values = appendValues(values, t.right)
	}
	return values
}
func add(t *tree, value int) *tree {
	if t == nil {
		t = new(tree)
		t.value = value
		return t
	}
	if value < t.value {
		t.left = add(t.left, value)
	} else {
		t.right = add(t.right, value)
	}
	return t
}
