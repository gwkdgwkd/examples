package main

import (
	"fmt"
	"math"
)

func main() {
	// Go语言提供了两种精度的浮点数,float32和float64。它们的算术规范由IEEE754浮点数国际
	// 标准定义,该浮点数规范被所有现代的CPU支持。

	// 这些浮点数类型的取值范围可以从很微小到很巨大。浮点数的范围极限值可以在math包找
	// 到。常量math.MaxFloat32表示float32能表示的最大数值,大约是3.4e38;对应的
	// math.MaxFloat64常量大约是1.8e308。它们分别能表示的最小值近似为1.4e-45和4.9e-324。
	fmt.Println(math.MaxFloat32) // 3.4028234663852886e+38
	fmt.Println(math.MaxFloat64) // 1.7976931348623157e+308

	// 一个float32类型的浮点数可以提供大约6个十进制数的精度,而float64则可以提供约15个十进
	// 制数的精度;通常应该优先使用float64类型,因为float32类型的累计计算误差很容易扩散,
	// 并且float32能精确表示的正整数并不是很大(因为float32的有效bit位只有23个,其它
	// 的bit位用于指数和符号;当整数大于23bit能表达的范围时,float32的表示将出现误差)
	var f float32 = 16777216 //	1 << 24
	fmt.Println(f == f+1)    //	true

	const e = 2.71828                            // 浮点数的字面值可以直接写小数部分
	e1, e2 := .707, 1.                           // 小数点前面或后面的数字都可能被省略
	fmt.Printf("%v, %v, %v(%[3]T)\n", e, e1, e2) // 2.71828, 0.707, 1(float64)

	// 很小或很大的数最好用科学计数法书写,通过e或E来指定指数部分
	const A, P = 6.02214129e23, 6.62606957e-34
	fmt.Printf("%v, %v\n", A, P) // 6.02214129e+23, 6.62606957e-34
	// %g参数打印浮点数,将采用更紧凑的表示形式打印,并提供足够的精度。
	// %g,%e,%f三个打印形式都可以指定打印的宽度和控制打印精度。
	fmt.Printf("%g, %g\n", A, P) // 6.02214129e+23, 6.62606957e-34
	fmt.Printf("%e, %e\n", A, P) // 6.022141e+23, 6.626070e-34
	fmt.Printf("%f, %f\n", A, P) // 602214128999999968641024.000000, 0.000000

	// math包中除了提供大量常用的数学函数外,还提供了IEEE754浮点数标准中定义的特殊值的
	// 创建和测试:正无穷大和负无穷大,分别用于表示太大溢出的数字和除零的结果;还有NaN
	// 非数,一般用于表示无效的除法操作结果0/0或Sqrt(-1).
	var z float64
	fmt.Println(z, -z, 1/z, -1/z, z/z) // 0 -0 +Inf -Inf NaN

	// 函数math.IsNaN用于测试一个数是否是非数NaN,math.NaN则返回非数对应的值。虽然可以
	// 用math.NaN来表示一个非法的结果,但是测试一个结果是否是非数NaN则是充满风险的,因
	// 为NaN和任何数都是不相等的(译注:在浮点数中,NaN、正无穷大和负无穷大都不是唯一
	// 的,每个都有非常多种的bit模式表示):
	nan := math.NaN()
	fmt.Println(nan == nan, nan < nan, nan > nan) // false false false
}

// 如果一个函数返回的浮点数结果可能失败,最好的做法是用单独的标志报告失败,像这样:
// func compute() (value float64, ok bool) {
// 	//	...
// 	if failed {
// 		return 0, false
// 	}
// 	return result, true
// }
