package main

import (
	"fmt"
	"reflect"
	"strings"
	"unsafe"
)

// Go语言的设计包含了诸多安全策略,限制了可能导致程序运行出错的用法。编译时类型检查
// 可以发现大多数类型不匹配的操作,例如两个字符串做减法的错误。字符串、map、slice和
// chan等所有的内置类型,都有严格的类型转换规则。
// 对于无法静态检测到的错误,例如数组访问越界或使用空指针,运行时动态检测可以保证程
// 序在遇到问题的时候立即终止并打印相关的错误信息。自动内存管理(垃圾内存自动回收)
// 可以消除大部分野指针和内存泄漏相关的问题。
// Go语言的实现刻意隐藏了很多底层细节。我们无法知道一个结构体真实的内存布局,也无法
// 获取一个运行时函数对应的机器码,也无法知道当前的goroutine是运行在哪个操作系统线程
// 之上。事实上,Go语言的调度器会自己决定是否需要将某个goroutine从一个操作系统线程转
// 移到另一个操作系统线程。一个指向变量的指针也并没有展示变量真实的地址。因为垃圾回
// 收器可能会根据需要移动变量的内存位置,当然变量对应的地址也会被自动更新。
// 总的来说,Go语言的这些特性使得Go程序相比较低级的C语言来说更容易预测和理解,程序
// 也不容易崩溃。通过隐藏底层的实现细节,也使得Go语言编写的程序具有高度的可移植性,
// 因为语言的语义在很大程度上是独立于任何编译器实现、操作系统和CPU系统结构的(当然
// 也不是完全绝对独立:例如int等类型就依赖于CPU机器字的大小,某些表达式求值的具体顺
// 序,还有编译器实现的一些额外的限制等)。

// unsafe包属于黑魔法,虽然功能很强大,但是也容易误伤到自己。如果没有处理好细节,它们
// 可能导致各种不可预测的并且隐晦的错误,甚至连有经验的C语言程序员也无法理解这些错误。
// 使用unsafe包的同时也放弃了Go语言保证与未来版本的兼容性的承诺,因为它必然会有意无意
// 中使用很多非公开的实现细节,而这些实现的细节在未来的Go语言中很可能会被改变。

// unsafe包是一个采用特殊方式实现的包。虽然它可以和普通包一样的导入和使
// 用,但它实际上是由编译器实现的。它提供了一些访问语言内部特性的方法,特别是内存布
// 局相关的细节。
// unsafe包被广泛地用于比较低级的包,例如runtime、os、syscall还有net包等,因为它们
// 需要和操作系统密切配合,但是对于普通的程序一般是不需要使用unsafe包的。

// Go语言的规范并没有要求一个字段的声明顺序和内存中的顺序是一致的,所以理论上一个编
// 译器可以随意地重新排列每个字段的内存位置,虽然在写作本书的时候编译器还没有这么
// 做。下面的三个结构体虽然有着相同的字段,但是第一种写法比另外的两个需要多50%的内存。
// struct {bool;float64;int16}	// 3word(32) 4word(64)
// struct{	float64;int16;bool}	// 2words 3words
// struct{	bool;int16;float64	} // 2words	3words

// 当调用一个库函数,并且返回的是uintptr类型地址时(普通方法实现的函数尽量不要返
// 回该类型。下面例子是reflect包的函数,reflect包和unsafe包一样都是采用特殊技术实现的,
// 编译器可能给它们开了后门),比如下面反射包中的相关函数,返回的结果应该立即转换为
// unsafe.Pointer以确保指针指向的是相同的变量。
// func (Value) Pointer() uintptr
// func (Value) UnsafeAddr() uintptr
// func (Value) InterfaceData() [2]uintptr

func test() {}

func Float64bits(f float64) uint64 { return *(*uint64)(unsafe.Pointer(&f)) }

func main() {
	// unsafe.Sizeof函数返回操作数在内存中的字节大小,参数可以是任意类型的表达式,但是它
	// 并不会对表达式进行求值。一个Sizeof函数调用是一个对应uintptr类型的常量表达式,因此返
	// 回的结果可以用作数组类型的长度大小,或者用作计算其他的常量。
	fmt.Println(unsafe.Sizeof(int8(0)))       // 1
	fmt.Println(unsafe.Sizeof(uint8(0)))      // 1
	fmt.Println(unsafe.Sizeof(int16(0)))      // 2
	fmt.Println(unsafe.Sizeof(uint16(0)))     // 2
	fmt.Println(unsafe.Sizeof(int32(0)))      // 4
	fmt.Println(unsafe.Sizeof(uint32(0)))     // 4
	fmt.Println(unsafe.Sizeof(int(0)))        // 8
	fmt.Println(unsafe.Sizeof(uint(0)))       // 8
	fmt.Println(unsafe.Sizeof(int64(0)))      // 8
	fmt.Println(unsafe.Sizeof(uint64(0)))     // 8
	fmt.Println(unsafe.Sizeof(float32(0)))    // 4
	fmt.Println(unsafe.Sizeof(float64(0)))    // 8
	fmt.Println(unsafe.Sizeof(complex(0, 0))) // 16
	fmt.Println(unsafe.Sizeof(complex64(0)))  // 8
	fmt.Println(unsafe.Sizeof(complex128(0))) // 16
	fmt.Println(unsafe.Sizeof(true))          // 1
	fmt.Println(unsafe.Sizeof(uintptr(0)))    // 8
	a := 0
	fmt.Println(unsafe.Sizeof(&a)) // 8
	p := &a
	fmt.Println(unsafe.Sizeof(p))                   // 8
	fmt.Println(unsafe.Sizeof("nihao"))             // 16
	fmt.Println(unsafe.Sizeof(test))                // 8
	fmt.Println(unsafe.Sizeof([]int{1, 2}))         // 24
	fmt.Println(unsafe.Sizeof(map[string]string{})) // 8
	fmt.Println(unsafe.Sizeof(make(chan int)))      // 8
	var x interface{}
	fmt.Println(unsafe.Sizeof(x)) // 16

	// Sizeof函数返回的大小只包括数据结构中固定的部分,例如字符串对应结构体中的指针和字符
	// 串长度部分,但是并不包含指针指向的字符串的内容。Go语言中非聚合类型通常有一个固定
	// 的大小,尽管在不同工具链下生成的实际大小可能会有所不同。考虑到可移植性,引用类型
	// 或包含引用类型的大小在32位平台上是4个字节,在64位平台上是8个字节。

	// 计算机在加载和保存数据时,如果内存地址合理地对齐的将会更有效率。例如2字节大小的
	// int16类型的变量地址应该是偶数,一个4字节大小的rune类型变量的地址应该是4的倍数,一
	// 个8字节大小的float64、uint64或64-bit指针类型变量的地址应该是8字节对齐的。但是对于再
	// 大的地址对齐倍数则是不需要的,即使是complex128等较大的数据类型最多也只是8字节对齐。
	// 由于地址对齐这个因素,一个聚合类型(结构体或数组)的大小至少是所有字段或元素大小
	// 的总和,或者更大因为可能存在内存空洞。内存空洞是编译器自动添加的没有被使用的内存
	// 空间,用于保证后面每个字段或元素的地址相对于结构或数组的开始地址能够合理地对齐
	// (内存空洞可能会存在一些随机数据,可能会对用unsafe包直接操作内存的处理产生影响)。

	// unsafe.Alignof函数返回对应参数的类型需要对齐的倍数.	和Sizeof类似,	Alignof也是返回
	// 一个常量表达式,	对应一个常量.通常情况下布尔和数字类型需要对齐到它们本身的大小(最多8个字节),
	// 其它的类型对齐到机器字大小.
	// unsafe.Offsetof函数的参数必须是一个字段x.f,然后返回f字段相对于x起始地址的偏移量,包括可能的空洞.
	var tt struct {
		a bool
		b int16
		c []int
	}
	fmt.Printf("%v, %v\n", unsafe.Sizeof(tt), unsafe.Alignof(tt))                                // 32, 8
	fmt.Printf("%v, %v, %v\n", unsafe.Sizeof(tt.a), unsafe.Alignof(tt.a), unsafe.Offsetof(tt.a)) // 1, 1, 0
	fmt.Printf("%v, %v, %v\n", unsafe.Sizeof(tt.b), unsafe.Alignof(tt.b), unsafe.Offsetof(tt.b)) // 2, 2, 2
	fmt.Printf("%v, %v, %v\n", unsafe.Sizeof(tt.c), unsafe.Alignof(tt.c), unsafe.Offsetof(tt.c)) // 24, 8, 8

	// 虽然这几个函数在不安全的unsafe包,但Sizeof、Alignof、Offsetof调用并不是真的不安全,特别在需要
	// 优化内存空间时它们返回的结果对于理解原生的内存布局很有帮助。

	// 大多数指针类型会写成*T,表示是“一个指向T类型变量的指针”。unsafe.Pointer是特别定义
	// 的一种指针类型(类似C语言中的void*类型的指针),它可以包含任意类型变量的地
	// 址。当然,我们不可以直接通过*p来获取unsafe.Pointer指针指向的真实变量的值,因为我
	// 们并不知道变量的具体类型。和普通指针一样,unsafe.Pointer指针也是可以比较的,并且支
	// 持和nil常量比较判断是否为空指针。

	// 一个普通的*T类型指针可以被转化为unsafe.Pointer类型指针,并且一个unsafe.Pointer类型
	// 指针也可以被转回普通的指针,被转回普通的指针类型并不需要和原始的*T类型相同。通过
	// 将*float64类型指针转化为*uint64类型指针,我们可以查看一个浮点数变量的位模式。
	fmt.Printf("%#016x\n", Float64bits(1.0)) // 0x3ff0000000000000
	// 通过转为新类型指针,我们可以更新浮点数的位模式。通过位模式操作浮点数是可以的,但
	// 是更重要的意义是指针转换语法让我们可以在不破坏类型系统的前提下向内存写入任意的值。

	// 一个unsafe.Pointer指针也可以被转化为uintptr类型,然后保存到指针型数值变量中(
	// 这只是和当前指针相同的一个数字值,并不是一个指针),然后用以做必要的指针数值运
	// 算。这种转换虽然也是可逆的,但是将uintptr转为unsafe.Pointer指针可能会破坏类
	// 型系统,因为并不是所有的数字都是有效的内存地址。

	// 许多将unsafe.Pointer指针转为原生数字,然后再转回为unsafe.Pointer类型指针的操作也是不安全的。
	// 将变量x的地址加上b字段地址偏移量转化为*int16类型指针,然后通过该指针更新x.b:
	var y struct {
		a bool
		b int16
		c []int
	}
	// 和 pb := &x.b等价
	pb := (*int16)(unsafe.Pointer(uintptr(unsafe.Pointer(&y)) + unsafe.Offsetof(y.b)))
	*pb = 42
	fmt.Println(y.b) //	42
	// 上面的写法尽管很繁琐,但在这里并不是一件坏事,因为这些功能应该很谨慎地使用。不要
	// 试图引入一个uintptr类型的临时变量,因为它可能会破坏代码的安全性,下面是错误的
	// tmp := uintptr(unsafe.Pointer(&y)) + unsafe.Offsetof(y.b)
	// pb := (*int16)(unsafe.Pointer(tmp))
	// *pb = 42
	// pT := uintptr(unsafe.Pointer(new(int))) // 错误
	// 这里并没有指针引用new新创建的变量,因此该语句执行完成之后,垃圾收集器有权马上回
	// 收其内存空间,所以返回的pT将是无效的地址。

	// 虽然目前的Go语言实现还没有使用移动GC(未来可能实现),但这不该是编写错误代
	// 码侥幸的理由:当前的Go语言实现已经有移动变量的场景。goroutine的栈是
	// 根据需要动态增长的。当发生栈动态增长的时候,原来栈中的所有变量可能需要被移动到新
	// 的更大的栈中,所以我们并不能确保变量的地址在整个使用周期内是不变的。

	// 强烈建议将所有包含变量地址的uintptr类型变量当作BUG处理,同时减少不必要的
	// unsafe.Pointer类型到uintptr类型的转换。

	// 来自reflect包的DeepEqual函数可以对两个值进行深度相等判断。DeepEqual函数使用内建的
	// ==比较操作符对基础类型进行相等判断,对于复合类型则递归该变量的每个基础类型然后做
	// 类似的比较判断。因为它可以工作在任意的类型上,甚至对于一些不支持==操作运算符的类
	// 型也可以工作,因此在一些测试代码中广泛地使用该函数。比如下面的代码是用DeepEqual
	// 函数比较两个字符串数组是否相等。
	got := strings.Split("a:b:c", ":")
	want := []string{"a", "b", "c"}
	if reflect.DeepEqual(got, want) {
		fmt.Println("xiang deng") // bu xiang deng
	}

	// 尽管DeepEqual函数很方便,而且可以支持任意的数据类型,但是它也有不足之处。例如,
	// 它将一个nil值的map和非nil值但是空的map视作不相等,同样nil值的slice和非nil但是空的
	// slice也视作不相等。
	var aa, bb []string = nil, []string{}
	fmt.Println(reflect.DeepEqual(aa, bb)) // false
	var cc, dd map[string]int = nil, make(map[string]int)
	fmt.Println(reflect.DeepEqual(cc, dd)) // false

	fmt.Println(Equal([]int{1, 2, 3}, []int{1, 2, 3}))        // true
	fmt.Println(Equal([]string{"foo"}, []string{"bar"}))      // false
	fmt.Println(Equal([]string(nil), []string{}))             // true
	fmt.Println(Equal(map[string]int(nil), map[string]int{})) // true

	ak, bk, ck := &link{value: "a"}, &link{value: "b"}, &link{value: "c"}
	ak.tail, bk.tail, ck.tail = bk, ak, ck
	fmt.Println(Equal(ak, ak)) // true
	fmt.Println(Equal(bk, bk)) // true
	fmt.Println(Equal(ck, ck)) // true
	fmt.Println(Equal(ak, bk)) // false ???
	fmt.Println(Equal(ak, ck)) // false ???
}

type link struct {
	value string
	tail  *link
}

func equal(x, y reflect.Value, seen map[comparison]bool) bool {
	if !x.IsValid() || !y.IsValid() {
		return x.IsValid() == y.IsValid()
	}
	if x.Type() != y.Type() {
		return false
	}

	if x.CanAddr() && y.CanAddr() {
		xptr := unsafe.Pointer(x.UnsafeAddr())
		yptr := unsafe.Pointer(y.UnsafeAddr())
		if xptr == yptr {
			return true // identical references
		}
		c := comparison{xptr, yptr, x.Type()}
		if seen[c] {
			return true // already seen
		}
		seen[c] = true
	}

	switch x.Kind() {
	case reflect.Bool:
		return x.Bool() == y.Bool()
	case reflect.String:
		return x.String() == y.String()
	case reflect.Chan, reflect.UnsafePointer, reflect.Func:
		return x.Pointer() == y.Pointer()
	case reflect.Ptr, reflect.Interface:
		return equal(x.Elem(), y.Elem(), seen)
	case reflect.Array, reflect.Slice:
		if x.Len() != y.Len() {
			return false
		}
		for i := 0; i < x.Len(); i++ {
			if !equal(x.Index(i), y.Index(i), seen) {
				return false
			}
		}
		return true
	}
	// panic("unreachable") // ???
	return true // ???
}

func Equal(x, y interface{}) bool {
	seen := make(map[comparison]bool)
	return equal(reflect.ValueOf(x), reflect.ValueOf(y), seen)
}

type comparison struct {
	x, y unsafe.Pointer
	t    reflect.Type
}
