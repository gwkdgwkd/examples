package main

import "fmt"

// Go语言的数值类型包括几种不同大小的整数、浮点数和复数。每种数值类型都决定了对应的
// 大小范围和是否支持正负符号。

func main() {
	// Go语言同时提供了有符号和无符号类型的整数运算。这里有int8、int16、int32和int64四种截
	// 然不同大小的有符号整数类型,分别对应8、16、32、64bit大小的有符号整数,与此对应的
	// 是uint8、uint16、uint32和uint64四种无符号整数类型。

	// 还有两种一般对应特定CPU平台机器字大小的有符号和无符号整数int和uint;其中int是应
	// 用最广泛的数值类型。这两种类型都有同样的大小,32或64bit,但是我们不能对此做任何的
	// 假设;因为不同的编译器即使在相同的硬件平台上可能产生不同的大小。

	// Unicode字符rune类型是和int32等价的类型,通常用于表示一个Unicode码点。这两个名称可
	// 以互换使用。同样byte也是uint8类型的等价类型,byte类型一般用于强调数值是一个原始的
	// 数据而不是一个小的整数。

	// 一种无符号的整数类型uintptr,没有指定具体的bit大小但是足以容纳指针。uintptr
	// 类型只有在底层编程时才需要,特别是Go语言和C语言函数库或操作系统接口相交互的地方。

	// int、uint和uintptr是不同类型的兄弟类型。其中int和int32也是不同的类型,即使int的
	// 大小也是32bit,在需要将int当作int32类型的地方需要一个显式的类型转换操作,反之亦然。

	// 有符号整数采用2的补码形式表示,也就是最高bit位用来表示符号位,一个n-bit的有符号
	// 数的值域是从$-2^{n-1}$到$2^{n-1}-1$。无符号整数的所有bit位都用于表示非负数,值域是0
	// 到$2^n-1$。例如,int8类型整数的值域是从-128到127,而uint8类型整数的值域是从0到255。

	// 从上到下，从左到右，优先级由高到低。使用括号可以明确优先顺序,使用括号也可以用于提升优先级。
	// *		/		%		<<		>>		&		&^
	// +		-		|		^
	// ==		!=		<		<=		>		>=
	// &&
	// ||

	// 算术运算符+、-、*和/可以适用于整数、浮点数和复数,但是取模运算符%仅用于整
	// 数间的运算。对于不同编程语言,%取模运算的行为可能并不相同。在Go语言中,%取模运
	// 算符的符号和被取模数的符号总是一致的,因此-5%3和-5%-3结果都是-2。除法运算
	// 符/的行为则依赖于操作数是否为全为整数,比如5.0/4.0的结果是1.25,但是5/4的结果是
	// 1,因为整数除法会向着0方向截断余数。

	// 如果一个算术运算的结果,不管是有符号或者是无符号的,如果需要更多的bit位才能正确表
	// 示的话,就说明计算结果是溢出了。超出的高位的bit位部分将被丢弃。如果原始的数值是有
	// 符号类型,而且最左边的bit为是1的话,那么最终结果可能是负的,例如int8的例子:
	var u uint8 = 255
	fmt.Println(u, u+1, u*u) //	255 0 1
	var i int8 = 127
	fmt.Println(i, i+1, i*i) //	127 128 1

	// 两个相同的整数类型可以使用下面的二元比较运算符进行比较;比较表达式的结果是布尔类型。
	// ==	等于
	// !=	不等于
	// <	小于
	// <=	小于等于
	// >	大于
	// >=	大于等于
	// 布尔型、数字类型和字符串等基本类型都是可比较的,也就是说两个相同类型的值
	// 可以用==和!=进行比较。此外,整数、浮点数和字符串可以根据比较结果排序。许多其它类
	// 型的值可能是不可比较的,因此也就可能是不可排序的。

	// 一元的加法和减法运算符:
	// +	一元加法(无效果)
	// -	负数
	// 对于整数,+x是0+x的简写,-x则是0-x的简写;对于浮点数和复数,+x就是x,-x则是x的负数。

	// Go语言还提供了以下的bit位操作运算符,前面4个操作运算符并不区分是有符号还是无符号数:
	// &	位运算	AND
	// |	位运算	OR
	// ^	位运算	XOR
	// &^	位清空	(AND NOT)
	// <<	左移
	// >>	右移
	// 位操作运算符^作为二元运算符时是按位异或(XOR),当用作一元运算符时表示按位取
	// 反;也就是说,它返回一个每个bit位都取反的数。位操作运算符&^用于按位置零(AND
	// NOT):如果对应y中bit位为1的话,表达式z=x&^y结果z的对应的bit位为0,否则z对应的
	// bit位等于x相应的bit位的值。
	// 在x<<n和x>>n移位运算中,决定了移位操作bit数部分必须是无符号数;被操作的x数可以
	// 是有符号或无符号数。算术上,一个x<<n左移运算等价于乘以$2^n$,一个x>>n右移运算
	// 等价于除以$2^n$。
	// 左移运算用零填充右边空缺的bit位,无符号数的右移运算也是用0填充左边空缺的bit位,但是
	// 有符号数的右移运算会用符号位的值填充左边空缺的bit位。因为这个原因,最好用无符号运
	// 算,这样你可以将整数完全当作一个bit位模式处理。

	// 尽管Go语言提供了无符号数和运算,即使数值本身不可能出现负数我们还是倾向于使用有符
	// 号的int类型,就像数组的长度那样,虽然使用uint无符号类型似乎是一个更合理的选择。事实
	// 上,内置的len函数返回一个有符号的int,我们可以像下面例子那样处理逆序循环。
	medals := []string{"gold", "silver", "bronze"}
	for i := len(medals) - 1; i >= 0; i-- {
		// for i := uint(2); i >= 0; i-- { // panic: runtime error: index out of range [18446744073709551615] with length 3
		// 	fmt.Println(i) // 18446744073709551615
		fmt.Println(medals[i]) //"bronze","silver","gold"
	}
	// 另一个选择对于上面的例子来说将是灾难性的。如果len函数返回一个无符号数,那么i也将是
	// 无符号的uint类型,然后条件i>=0则永远为真。在三次迭代之后,也就是i==0时,i--语
	// 句将不会产生-1,而是变成一个uint类型的最大值(可能是$2^64-1$),然后medals[i]表达式
	// 将发生运行时panic异常,也就是试图访问一个slice范围以外的元素。
	// 出于这个原因,无符号数往往只有在位运算或其它特殊的运算场景才会使用,就像bit集合、
	// 分析二进制文件格式或者是哈希和加密操作等。它们通常并不用于仅仅是表达非负数量的场合。

	// 需要一个显式的转换将一个值从一种类型转化位另一种类型,并且算术和逻辑运
	// 算的二元操作中必须是相同的类型。虽然这偶尔会导致需要很长的表达式,但是它消除了所
	// 有和类型相关的问题,而且也使得程序容易理解。
	// 对于每种类型T,如果转换允许的话,类型转换操作T(x)将x转换为T类型。许多
	// 整数之间的相互转换并不会改变数值;它们只是告诉编译器如何解释这个值。但是对于将一
	// 个大尺寸的整数类型转为一个小尺寸的整数类型,或者是将一个浮点数转为整数,可能会改
	// 变数值或丢失精度:
	f := 3.141
	i1 := int(f)       // 浮点数到整数的转换将丢失任何小数部分,然后向数轴零方向截断。
	fmt.Println(f, i1) // 3.141 3
	f1 := 1e100        // a float64
	i2 := int(f1)      // 应该避免对可能会超出目标类型表示范围的数值类型转换,因为截断的行为可能依赖于具体的实现
	fmt.Println(i2)    // -9223372036854775808

	// 任何大小的整数字面值都可以用以0开始的八进制格式书写,例如0666;或用以0x或0X开头
	// 的十六进制格式书写,例如0xdeadbeef。
}
