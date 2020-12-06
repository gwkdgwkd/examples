package main

import "fmt"

// var h = 5 // 可以定义全局变量
// h := 5 // 不可以，只能用在局部变量

func main()  {
	// 变量名称的命名：
	// Go语言变量名由字母、数字、下划线组成，其中首个字符不能为数字。
	// Go语言中关键字和保留字都不能用作变量名。
	// 变量名称严格区分大小写
	// 标识符一定要见名思意：变量名建议用名称，方法名建议用动词
	// 变量命名一般采用驼峰式（maxAge,小;MaxAge,大)，特有名词根据是否私有全大写或小写

	// 1.var声明变量:
	// var 变量名称 类型
	var username string // 没有初始化，值为空
	var i int // 没有初始化，值为0
	var f float64 // 没有初始化，值为0
	var b bool // 没有初始化化，值位false
	fmt.Println(username, i, f, b) //  0 0 false

	// var 1a string // 错误
	// var if = "李四" // 错误
	// var m_a = "李四" // 正确，但不推荐

	// 2.变量的定义以及初始化
	// var a1 string
	// a1 = "张三" // 第一种方式
	// var a2 string = "张三" // 第二种方式
	// var a3 = "张三" // 第三种方式，类型推导

	// 3.一次定义多个变量
	// 第一种方式：var 变量名称，变量名称 类型
	// var b1, b2 string // 同一种类型
	/*
	第二种方式：类型可以不同
    var （
		变量名称 类型
	    变量名称 类型  
	）
	var (
		c1 int
		c2 float64
	)
	var (
		c1 int = 5
		c2 float64 = 12.34
	)
	var (
		c1 = 5
		c2 = 12.34
	)
	*/

	// 4.短变量声明法，只能用于声明局部变量，不能用于全局变量的声明
	d := 5
	fmt.Printf("%v,类型%T\n", d, d)	// 5,类型int
	e, f, g := 5, 15.66, "nihao"
	fmt.Printf("%v(%T), %v(%T), %v(%T)\n", e, e, f, f, g, g) // 5(int), 15.66(float64), nihao(string)

	// 5.匿名变量，使用一个下划线_表示。使用多重赋值时，如果想忽略某个值，可以使用匿名变量。

	// 6.常量，定义常量时必须赋值;定义完后也不能改变其值。
	// const pi = 3.14159 // 定义时必须初始化
	// pi = 6.5 // 不能改变
	const ( // 多个常量也可以一起声明
	    // D // 第一个必须初始化
		A = "A"
		B = "B"
		C // 如果忽略力值表示和上面一行的值相同
		E
	)
	fmt.Println(A, B ,C, E) // A B B

	// 7.iota
	const a = iota // iota是0
	fmt.Println(a) // 0
	const (
		n1 = iota
		n2 // 第一个值是iota，后面的值如果不赋值，那么累加
		n3
		_ // 跳过一个值
		n4
		n5 = 100 // 声明中间插队
		n6 = iota
		n7
	)
	fmt.Println(n1, n2, n3, n4, n5, n6, n7) // 0 1 2 4 100 6 7

	// 8.多个iota定义在一行
	const ( 
		m1, m2 = iota + 1, iota + 2
		m3, m4
		m5, m6
	)
	fmt.Println(m1, m2, m3, m4, m5, m6) // 1 2 2 3 3 4
}