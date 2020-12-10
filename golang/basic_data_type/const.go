package main

import (
	"fmt"
)

func main() {
	// 常量表达式的值在编译期计算,而不是在运行期。每种常量的潜在类型都是基础类型:
	// boolean、string或数字。

	// 一个常量的声明语句定义了常量的名字,和变量的声明语法类似,常量的值不可修改,这样
	// 可以防止在运行期被意外或恶意的修改。和变量声明一样,可以批量声明多个常量;
	// 这比较适合声明一组相关的常量:
	const pi1 = 3.14159 //	approximately; math.Pi is a	better approximation
	const (
		e  = 2.71828182845904523536028747135266249775724709369995957496696763
		pi = 3.14159265358979323846264338327950288419716939937510582097494459
	)
	fmt.Println(pi1, e, pi) // 3.14159 2.718281828459045 3.141592653589793

	// 所有常量的运算都可以在编译期完成,这样可以减少运行时的工作,也方便其他编译优化。
	// 当操作数是常量时,一些运行时的错误也可以在编译时被发现,例如整数除零、字符串索引
	// 越界、任何导致无效浮点数的操作等。

	// 常量间的所有算术运算、逻辑运算和比较运算的结果也是常量,对常量的类型转换操作或以
	// 下函数调用都是返回常量结果:len、cap、real、imag、complex和unsafe.Sizeof。
	// 因为它们的值是在编译期就确定的,因此常量可以是构成类型的一部分,如用于数组的长度。

	// 一个常量的声明也可以包含一个类型和一个值,但是如果没有显式指明类型,那么将从右边
	// 的表达式推断类型。

	// 批量声明的常量,除了第一个外其它的常量右边的初始化表达式都可以省略,如果省
	// 略初始化表达式则表示使用前面常量的初始化表达式写法,对应的常量类型也一样的。
	const (
		a = 1
		b
		c = 2
		d
	)
	fmt.Println(a, b, c, d) // 1 1 2 2

	// 常量声明可以使用iota常量生成器初始化,它用于生成一组以相似规则初始化的常量,但是不
	// 用每行都写一遍初始化表达式。在一个const声明语句中,在第一个声明的常量所在的行,
	// iota将会被置为0,然后在每一个有常量声明的行加一。
	type Weekday int
	const (
		Sunday Weekday = iota
		Monday
		Tuesday
		Wednesday
		Thursday
		Friday
		Saturday
	)
	// 0 1 2 3 4 5 6
	fmt.Println(Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday)
	type Flags uint
	const (
		FlagUp           Flags = 1 << iota // 00000001
		FlagBroadcast                      // 00000010
		FlagLoopback                       // 00000100
		FlagPointToPoint                   // 00001000
		FlagMulticast                      // 00010000
	)
	fmt.Printf("%08b,%08b,%08b,%08b,%08b\n", FlagUp, FlagBroadcast, FlagLoopback, FlagPointToPoint, FlagMulticast)
	const (
		_   = 1 << (10 * iota)
		KiB //	1024
		MiB //	1048576
		GiB //	1073741824
		TiB //	1099511627776						(exceeds 1 << 32)
		PiB //	1125899906842624
		EiB //	1152921504606846976
		ZiB //	1180591620717411303424				(exceeds 1 << 64)
		YiB //	1208925819614629174706176
	)

	// Go语言的常量有个不同寻常之处。虽然一个常量可以有任意有一个确定的基础类型,例如int
	// 或float64,或者是类似time.Duration这样命名的基础类型,但是许多常量并没有一个明确的
	// 基础类型。编译器为这些没有明确的基础类型的数字常量提供比基础类型更高精度的算术运
	// 算;你可以认为至少有256bit的运算精度。这里有六种未明确类型的常量类型,分别是无类型
	// 的布尔型、无类型的整数、无类型的字符、无类型的浮点数、无类型的复数、无类型的字符串。

	// 通过延迟明确常量的具体类型,无类型的常量不仅可以提供更高的运算精度,而且可以直接
	// 用于更多的表达式而不需要显式的类型转换。例如,例子中的ZiB和YiB的值已经超出任何Go
	// 语言中整数类型能表达的范围,但是它们依然是合法的常量,而且可以像下面常量表达式依
	// 然有效(YiB/ZiB是在编译期计算出来的,并且结果常量是1024,是Go语言int变量能有效表示的):
	fmt.Println(YiB / ZiB) // 1024

	// math.Pi无类型的浮点数常量,可以直接用于任意需要浮点数或复数的地方:
	// var x float32 = math.Pi
	// var y float64 = math.Pi
	// var z complex128 = math.Pi
	// 如果math.Pi被确定为特定类型,比如float64,那么结果精度可能会不一样,同时对于需要
	// float32或complex128类型值的地方则会强制需要一个明确的类型转换:
	// const Pi64 float64 = math.Pi
	// var x1 float32 = float32(Pi64)
	// var y1 float64 = Pi64
	// var z1 complex128 = complex128(Pi64)

	// 对于常量面值,不同的写法可能会对应不同的类型。例如0、0.0、0i和\u0000虽然有着相同
	// 的常量值,但是它们分别对应无类型的整数、无类型的浮点数、无类型的复数和无类型的字
	// 符等不同的常量类型。同样,true和false也是无类型的布尔类型,字符串面值常量是无类型的
	// 字符串类型。
	const c1 = 0
	const c2 = 0.0
	const c3 = 0i
	const c4 = '\u0000'
	const c5 = true
	const c6 = "string"
	fmt.Printf("%v[%[1]T]\n", c1) // 0[int]
	fmt.Printf("%v[%[1]T]\n", c2) // 0[float64]
	fmt.Printf("%v[%[1]T]\n", c3) // (0+0i)[complex128]
	fmt.Printf("%v[%[1]T]\n", c4) // 0[int32]
	fmt.Printf("%v[%[1]T]\n", c5) // true[bool]
	fmt.Printf("%v[%[1]T]\n", c6) // string[string]

	// 除法运算符/会根据操作数的类型生成对应类型的结果。因此,不同写法的常量除法
	// 表达式可能对应不同的结果:
	var f float64 = 212
	fmt.Println((f - 32) * 5 / 9)     // 100;	(f-32)*5 is	a float64
	fmt.Println(5 / 9 * (f - 32))     // 0;		5/9	is an untyped integer,0
	fmt.Println(5.0 / 9.0 * (f - 32)) // 100;	5.0/9.0	is an untyped float

	// 只有常量可以是无类型的。当一个无类型的常量被赋值给一个变量的时候,或者出现在有明确
	// 类型的变量声明的右边,无类型的常量将会被隐式转换为对应的类型,如果转换合法的话。
	// var f float64 = 3 + 0i //	untyped	complex	->	float64
	// f = 2                  //	untyped	integer	->	float64
	// f = 1e123              //	untyped	floating-point	->	float64
	// f = 'a'                //	untyped	rune	->	float64

	// 无论是隐式或显式转换,将一种类型转换为另一种类型都要求目标可以表示原始值。对于浮
	// 点数和复数,可能会有舍入处理:
	// const (
	// 	deadbeef = 0xdeadbeef        //	untyped	int	with	value	3735928559
	// 	a        = uint32(deadbeef)  // uint32	with value 3735928559
	// 	b        = float32(deadbeef) // float32 with value	3735928576 (rounded	up)
	// 	c        = float64(deadbeef) // float64 with value	3735928559 (exact)
	// 	d        = int32(deadbeef)   // compile error:	constant overflows int32
	// 	e        = float64(1e309)    // compile error:	constant overflows float64
	// 	f        = uint(-1)          // compile error:	constant underflows	uint
	// )

	// 对于一个没有显式类型的变量声明(包括简短变量声明),常量的形式将隐式决定变量的默认类型:
	// i := 0      // untyped integer; implicit int(0)
	// r := '\000' // untyped rune; implicit rune('\000')
	// f := 0.0    // untyped floating-point; implicit float64(0.0)
	// c := 0i     // untyped complex; implicit complex128(0i)

	// 无类型整数常量转换为int,它的内存大小是不确定的,但是无类型浮点数
	// 和复数常量则转换为内存大小明确的float64和complex128。如果不知道浮点数类型的内存大
	// 小是很难写出正确的数值算法的,因此Go语言不存在整型类似的不确定内存大小的浮点数和复数类型。

	// 如果要给变量一个不同的类型,我们必须显式地将无类型的常量转化为所需的类型,或给声
	// 明的变量指定明确的类型:
	// var i = int8(0)
	// var i int8 = 0

	// 当尝试将这些无类型的常量转为一个接口值时,这些默认类型将显得尤为重要,
	// 因为要靠它们明确接口对应的动态类型。
	fmt.Printf("%T\n", 0)      // int
	fmt.Printf("%T\n", 0.0)    // float64
	fmt.Printf("%T\n", 0i)     // complex128
	fmt.Printf("%T\n", '\000') // int32(rune)
}
