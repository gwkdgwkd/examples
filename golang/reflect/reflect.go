package main

import (
	"fmt"
	"io"
	"os"
	"reflect"
	"strconv"
	"strings"
	"time"
)

// Go语言提供了一种机制,能够在运行时更新变量和检查它们的值、调用它们的方法和它们支
// 持的内在操作,而不需要在编译时就知道这些变量的具体类型。这种机制被称为反射。反射
// 也可以让我们将类型本身作为第一类的值类型处理。

// 反射是由reflect包提供的。它定义了两个重要的类型,Type和Value.一个Type表示一个Go
// 类型.它是一个接口,有许多方法来区分类型以及检查它们的组成部分,例如一个结构体的成员
// 或一个函数的参数等. 唯一能反映reflect.Type实现的是接口的类型描述信息,也正是这
// 个实体标识了接口值的动态类型.
// reflect包中另一个重要的类型是Value. 一个reflect.Value可以装载任意类型的值.

// 应该尽量避免在一个包的API中暴露涉及反射的接口。

// 反射是一个强大并富有表达力的工具,但是它应该被小心地使用,原因有三:
// 第一个原因是,基于反射的代码是比较脆弱的。对于每一个会导致编译器报告类型错误的问
// 题,在反射中都有与之相对应的误用问题,不同的是编译器会在构建时马上报告错误,而反
// 射则是在真正运行到的时候才会抛出panic异常,可能是写完代码很久之后了,而且程序也可
// 能运行了很长的时间。
// 避免这种因反射而导致的脆弱性的问题的最好方法是将所有的反射相关的使用控制在包的内
// 部,如果可能的话避免在包的API中直接暴露reflect.Value类型,这样可以限制一些非法输
// 入。如果无法做到这一点,在每个有风险的操作前指向额外的类型检查。以标准库中的代码
// 为例,当fmt.Printf收到一个非法的操作数是,它并不会抛出panic异常,而是打印相关的错误
// 信息。
// 反射同样降低了程序的安全性,还影响了自动化重构和分析工具的准确性,因为它们无法识
// 别运行时才能确认的类型信息。
// 避免使用反射的第二个原因是,即使对应类型提供了相同文档,但是反射的操作不能做静态
// 类型检查,而且大量反射的代码通常难以理解。总是需要小心翼翼地为每个导出的类型和其
// 它接受interface{}或reflect.Value类型参数的函数维护说明文档。
// 第三个原因,基于反射的代码通常比正常的代码运行速度慢一到两个数量级。对于一个典型
// 的项目,大部分函数的性能和程序的整体性能关系不大,所以使用反射可能会使程序更加清
// 晰。测试是一个特别适合使用反射的场景,因为每个测试的数据集都很小。但是对于性能关
// 键路径的函数,最好避免使用反射。

// 首先用switch类型分支来测试输入参数是否实现了String方法,如果是的话就调用该方
// 法。然后继续增加类型测试分支,检查这个值的动态类型是否是string、int、bool等基础类
// 型,并在每种情况下执行相应的格式化操作。
// 如何处理其它类似[]float64、map[string][]string等类型呢?我们当然可以添加更多的
// 测试分支,但是这些组合类型的数目基本是无穷的。还有如何处理类似url.Values这样的具名
// 类型呢?即使类型分支可以识别出底层的基础类型是map[string][]string,但是它并不匹配
// url.Values类型,因为它们是两种不同的类型,而且switch类型分支也不可能包含每个类似
// url.Values的类型,这会导致对这些库的依赖。
// 没有办法来检查未知类型的表示方式,我们被卡住了。这就是我们为何需要反射的原因。
func Sprint(x interface{}) string {
	type stringer interface {
		String() string
	}
	switch x := x.(type) {
	case stringer:
		return x.String()
	case string:
		return x
	case int:
		return strconv.Itoa(x)
	//	...similar cases for int16,	uint32,	and	so on...
	case bool:
		if x {
			return "true"
		}
		return "false"
	default:
		//	array, chan, func, map,	pointer, slice,	struct
		return "???"
	}
}

// 使用reflect.Value的Kind方法来替代之前的类型switch.虽然还是有无穷多的类型,但是
// 它们的kinds类型却是有限的:Bool,String和所有数字类型的基础类型;Array和Struct对应
// 的聚合类型;Chan,Func,Ptr,Slice,和Map对应的引用类型;interface类型;还有表示空值的
// Invalid类型.(空的reflect.Value的kind即为Invalid.)
// Any formats any value as	a string.
func Any(value interface{}) string {
	return formatAtom(reflect.ValueOf(value))
}

// formatAtom formats a value without inspecting its internal structure.
func formatAtom(v reflect.Value) string {
	switch v.Kind() {
	case reflect.Invalid:
		return "invalid"
	case reflect.Int, reflect.Int8, reflect.Int16,
		reflect.Int32, reflect.Int64:
		return strconv.FormatInt(v.Int(), 10)
	case reflect.Uint, reflect.Uint8, reflect.Uint16,
		reflect.Uint32, reflect.Uint64, reflect.Uintptr:
		return strconv.FormatUint(v.Uint(), 10)
	//	...floating-point and complex cases	omitted	for	brevity...
	case reflect.Bool:
		return strconv.FormatBool(v.Bool())
	case reflect.String:
		return strconv.Quote(v.String())
	case reflect.Chan, reflect.Func, reflect.Ptr, reflect.Slice, reflect.Map:
		return v.Type().String() + "	0x" +
			strconv.FormatUint(uint64(v.Pointer()), 16)
	default: //	reflect.Array,	reflect.Struct,	reflect.Interface
		return v.Type().String() + "	value"
	}
}

// 一个递归的值打印器
func Display(name string, x interface{}) {
	fmt.Printf("Display %s (%T):\n", name, x)
	display(name, reflect.ValueOf(x))
}

func display(path string, v reflect.Value) {
	switch v.Kind() {
	case reflect.Invalid:
		fmt.Printf("%s = invalid\n", path)
	case reflect.Slice, reflect.Array:
		for i := 0; i < v.Len(); i++ {
			display(fmt.Sprintf("%s[%d]", path, i), v.Index(i))
		}
	case reflect.Struct:
		for i := 0; i < v.NumField(); i++ {
			fieldPath := fmt.Sprintf("%s.%s", path, v.Type().Field(i).Name)
			display(fieldPath, v.Field(i))
		}
	case reflect.Map:
		for _, key := range v.MapKeys() {
			display(fmt.Sprintf("%s[%s]", path,
				formatAtom(key)), v.MapIndex(key))
		}
	case reflect.Ptr:
		if v.IsNil() {
			fmt.Printf("%s = nil\n", path)
		} else {
			display(fmt.Sprintf("(*%s)", path), v.Elem())
		}
	case reflect.Interface:
		if v.IsNil() {
			fmt.Printf("%s = nil\n", path)
		} else {
			fmt.Printf("%s.type = %s\n", path, v.Elem().Type())
			display(path+".value", v.Elem())
		}
	default: //	basic types, channels, funcs
		fmt.Printf("%s = %s\n", path, formatAtom(v))
	}
}

func Print(x interface{}) {
	v := reflect.ValueOf(x)
	t := v.Type()
	fmt.Printf("type %s\n", t)
	for i := 0; i < v.NumMethod(); i++ {
		methType := v.Method(i).Type()
		fmt.Printf("func(%s) %s%s\n", t, t.Method(i).Name,
			strings.TrimPrefix(methType.String(), "func"))
	}
}

type Movie struct {
	Title, Subtitle string
	Year            int
	Color           bool
	Actor           map[string]string
	Oscars          []string
	Sequel          *string
}

func main() {
	// 函数 reflect.TypeOf接受任意的interface{}类型,并以reflect.Type形式返回其动态类型:
	t := reflect.TypeOf(3)  // a reflect.Type
	fmt.Println(t.String()) // int
	fmt.Println(t)          // int

	// 因为reflect.TypeOf返回的是一个动态类型的接口值,	它总是返回具体的类型.因此,下面的代
	// 码将打印"*os.File"而不是"io.Writer".
	var w io.Writer = os.Stdout
	fmt.Println(reflect.TypeOf(w)) // *os.File

	// reflect.Type接口是满足fmt.Stringer接口的.因为打印一个接口的动态类型对于
	// 调试和日志是有帮助的,fmt.Printf提供了一个缩写%T参数,内部使用reflect.TypeOf来输出:
	fmt.Printf("%T\n", 3) // int

	// reflect.ValueOf接受任意的interface{}类型,并返回一个装载着其动态值的reflect.Value.和
	// reflect.TypeOf类似, reflect.ValueOf返回的结果也是具体的类型,但是reflect.Value也可以持有一个接口值.
	v := reflect.ValueOf(3) // a reflect.Value
	fmt.Println(v)          // 3
	fmt.Printf("%v\n", v)   // 3
	fmt.Println(v.String()) // <int Value>
	// 和reflect.Type类似, reflect.Value也满足fmt.Stringer接口, 但是除非Value持有的是字符串,
	// 否则String方法只返回其类型.而使用fmt包的%v标志参数会对reflect.Values特殊处理.

	// reflect.ValueOf的逆操作是reflect.Value.Interface方法.它返回一个interface{}类型,装载着
	// 与reflect.Value相同的具体值:
	x := v.Interface()    // an interface{}
	i := x.(int)          // an int
	fmt.Printf("%d\n", i) // 3
	// reflect.Value和interface{}都能装载任意的值.所不同的是,一个空的接口隐藏了值内部的表示
	// 方式和所有方法, 因此只有我们知道具体的动态类型才能使用类型断言来访问内部的值.

	var xx int64 = 1
	var d time.Duration = 1 * time.Nanosecond
	fmt.Println(Any(xx))                 //	1
	fmt.Println(Any(d))                  //	1
	fmt.Println(Any([]int64{xx}))        //	[]int64 0xc0000ac050
	fmt.Println(Any([]time.Duration{d})) //	[]time.Duration	0xc0000ac058

	strangelove := Movie{
		Title:    "Dr. Strangelove",
		Subtitle: "How I Learned to Stop Worrying and Love the Bomb",
		Year:     1964,
		Color:    false,
		Actor: map[string]string{
			"Dr.Strangelove":           "Peter Sellers",
			"Grp.Capt.Lionel Mandrake": "Peter Sellers",
			"Pres.Merkin Muffley":      "Peter Sellers",
			"Gen.Buck Turgidson":       "George C.Scott",
			"Brig.Gen.Jack D.Ripper":   "Sterling Hayden",
			`Maj.T.J."King"Kong`:       "Slim Pickens",
		},
		Oscars: []string{
			"Best Actor	(Nomin.)",
			"Best Adapted Screenplay (Nomin.)",
			"Best Director (Nomin.)",
			"Best Picture (Nomin.)",
		},
	}
	Display("strangelove", strangelove)
	// Display strangelove (main.Movie):
	// strangelove.Title = "Dr. Strangelove"
	// strangelove.Subtitle = "How I Learned to Stop Worrying and Love the Bomb"
	// strangelove.Year = 1964
	// strangelove.Color = false
	// strangelove.Actor["Dr.Strangelove"] = "Peter Sellers"
	// strangelove.Actor["Grp.Capt.Lionel Mandrake"] = "Peter Sellers"
	// strangelove.Actor["Pres.Merkin Muffley"] = "Peter Sellers"
	// strangelove.Actor["Gen.Buck Turgidson"] = "George C.Scott"
	// strangelove.Actor["Brig.Gen.Jack D.Ripper"] = "Sterling Hayden"
	// strangelove.Actor["Maj.T.J.\"King\"Kong"] = "Slim Pickens"
	// strangelove.Oscars[0] = "Best Actor\t(Nomin.)"
	// strangelove.Oscars[1] = "Best Adapted Screenplay (Nomin.)"
	// strangelove.Oscars[2] = "Best Director (Nomin.)"
	// strangelove.Oscars[3] = "Best Picture (Nomin.)"
	// strangelove.Sequel = nil
	// 也可以使用Display函数来显示标准库中类型的内部结构,例如*os.File类型:
	Display("os.Stderr", os.Stderr)
	// Display os.Stderr (*os.File):
	// (*(*os.Stderr).file).pfd.fdmu.state = 0
	// (*(*os.Stderr).file).pfd.fdmu.rsema = 0
	// (*(*os.Stderr).file).pfd.fdmu.wsema = 0
	// (*(*os.Stderr).file).pfd.Sysfd = 2
	// (*(*os.Stderr).file).pfd.pd.runtimeCtx = 0
	// (*(*os.Stderr).file).pfd.iovecs = nil
	// (*(*os.Stderr).file).pfd.csema = 0
	// (*(*os.Stderr).file).pfd.isBlocking = 1
	// (*(*os.Stderr).file).pfd.IsStream = true
	// (*(*os.Stderr).file).pfd.ZeroReadIsEOF = true
	// (*(*os.Stderr).file).pfd.isFile = true
	// (*(*os.Stderr).file).name = "/dev/stderr"
	// (*(*os.Stderr).file).dirinfo = nil
	// (*(*os.Stderr).file).nonblock = false
	// (*(*os.Stderr).file).stdoutOrErr = true
	// (*(*os.Stderr).file).appendMode = false
	// 反射能够访问到结构体中未导出的成员。
	Display("rV", reflect.ValueOf(os.Stderr))
	// Display rV (reflect.Value):
	// (*rV.typ).size = 8
	// (*rV.typ).ptrdata = 8
	// (*rV.typ).hash = 871609668
	// (*rV.typ).tflag = 1
	// (*rV.typ).align = 8
	// (*rV.typ).fieldAlign = 8
	// (*rV.typ).kind = 54
	// (*(*rV.typ).alg).hash = func(unsafe.Pointer, uintptr) uintptr 0x453590
	// (*(*rV.typ).alg).equal = func(unsafe.Pointer, unsafe.Pointer) bool 0x402d80
	// (*(*rV.typ).gcdata) = 1
	// (*rV.typ).str = 8446
	// (*rV.typ).ptrToThis = 0
	// rV.ptr = unsafe.Pointer value
	// rV.flag = 22

	var ii interface{} = 3
	Display("ii", ii) // 返回一个Int类型的值
	// Display ii (int):
	// ii = 3
	Display("&ii", &ii) // 返回一个指向i的指针
	// Display &ii (*interface {}):
	// (*&ii).type = int
	// (*&ii).value = 3

	// 许多Go语言程序都包含了一些循环的数据。让Display支持这类带环的数据结构需要些技巧,
	// 需要额外记录迄今访问的路径;相应会带来成本。通用的解决方案是采用unsafe的语言特性
	// 对于目前的实现,如果遇到对象图中含有回环,Display将会陷入死循环,例如下面这个首尾相连的链表:
	type Cycle struct {
		Value int
		Tail  *Cycle
	}
	var c Cycle
	c = Cycle{42, &c}
	// Display("c", c) // 循环打印

	// Go语言中类似x、x.f[1]和*p形式的表达式都可以表示变量,但是其它如x	+1和f(2)
	// 则不是变量。一个变量就是一个可寻址的内存空间,里面存储了一个值,并且存储的值可以
	// 通过内存地址来更新。
	// 对于reflect.Values也有类似的区别。有一些reflect.Values是可取地址的;其它一些则不可
	// 以。考虑以下的声明语句:
	xxx := 2
	a := reflect.ValueOf(2)     // a对应的变量不可取地址。因为a中的值仅仅是整数2的拷贝副本。
	b := reflect.ValueOf(xxx)   // 不可取地址。
	cc := reflect.ValueOf(&xxx) // 不可取地址, 只是一个指针&x的拷贝
	dd := cc.Elem()             // 解引用方式生成的,指向另一个变量,因此是可取地址的。
	// 所有通过reflect.ValueOf(x)返回的reflect.Value都是不可取地址的。
	// 可以通过调用reflect.ValueOf(&x).Elem(),来获取任意变量x对应的可取地址的Value。
	// 可以通过调用reflect.Value的CanAddr方法来判断其是否可以被取地址:
	fmt.Println(a.CanAddr())  // false
	fmt.Println(b.CanAddr())  // false
	fmt.Println(cc.CanAddr()) // false
	fmt.Println(dd.CanAddr()) // true
	// 每当我们通过指针间接地获取的reflect.Value都是可取地址的,即使开始的是一个不可取地址
	// 的Value。在反射机制中,所有关于是否支持取地址的规则都是类似的。例如,slice的索引表
	// 达式e[i]将隐式地包含一个指针,它就是可取地址的,即使开始的e表达式不支持也没有关系。
	// 以此类推,reflect.ValueOf(e).Index(i)对于的值也是可取地址的,即使原始的
	// reflect.ValueOf(e)不支持也没有关系。

	// 要从变量对应的可取地址的reflect.Value来访问变量需要三个步骤。第一步是调用Addr()方
	// 法,它返回一个Value,里面保存了指向变量的指针。然后是在Value上调用Interface()方法,
	// 也就是返回一个interface{},里面包含指向变量的指针。最后,如果我们知道变量的类型,我
	// 们可以使用类型的断言机制将得到的interface{}类型的接口强制转为普通的类型指针。这样我
	// 们就可以通过这个普通指针来更新变量了:
	m := 2
	n := reflect.ValueOf(&m).Elem()   // d refers to the variable x
	pm := n.Addr().Interface().(*int) // pm := &m
	*pm = 3                           // x = 3
	fmt.Println(m)                    // 3
	// 或者,不使用指针,而是通过调用可取地址的reflect.Value的reflect.Value.Set方法来更新对于的值:
	n.Set(reflect.ValueOf(4))
	fmt.Println(m) // 4
	// Set方法将在运行时执行和编译时进行类似的可赋值性约束的检查。以上代码,变量和值都是
	// int类型,但是如果变量是int64类型,那么程序将抛出一个panic异常,所以关键问题是要确保
	// 改类型的变量可以接受对应的值:
	// n.Set(reflect.ValueOf(int64(5))) //	panic: reflect.Set: value of type int64 is not assignable to type int
	// 同样,对一个不可取地址的reflect.Value调用Set方法也会导致panic异常:
	// xy := 2
	// bg := reflect.ValueOf(xy)
	// bg.Set(reflect.ValueOf(3)) // panic: reflect.Set: value of type int64 is not assignable to type int

	// 有很多用于基本数据类型的Set方法:SetInt、SetUint、SetString和SetFloat等。
	uu := reflect.ValueOf(&m).Elem()
	uu.SetInt(3)
	fmt.Println(uu) // 3
	// SetInt,只要变量是某种类型的有符号整数就可以工作,即使是一些命名的类型、甚至只要底层数据类型是有符
	// 号整数就可以,而且如果对于变量类型值太大的话会被自动截断。
	// 对于一个引用interface{}类型的reflect.Value调用SetInt会导致panic异常,即使那个interface{}变
	// 量对于整数类型也不行。

	// 反射可以越过Go语言的导出规则的限制读取结构体中未导出的成员,比如在类Unix系统上os.File结构体中的
	// fd int成员。然而,利用反射机制并不能修改这些未导出的成员:
	// stdout := reflect.ValueOf(os.Stdout).Elem() // *os.Stdout, an os.File var
	// fmt.Println(stdout.Type())                  // os.File
	// fd := stdout.FieldByName("fd") // panic: reflect: call of reflect.Value.Int on zero Value ?
	// fmt.Println(fd.Int()) // 1
	// fd.SetInt(2)          // panic:	unexported field
	// 一个可取地址的reflect.Value会记录一个结构体成员是否是未导出成员,如果是的话则拒绝修改操作。

	// reflect.Type的Field方法将返回一个reflect.StructField,里面含有每个成员的名字、类型和可选的成员标签等
	// 信息。其中成员标签信息对应reflect.StructTag类型的字符串,并且提供了Get方法用于解析和根据特定key提取的子.
	type resume struct {
		Name string `json:"name" doc:"我的名字"`
	}
	var stru resume
	z := reflect.TypeOf(&stru).Elem()
	doc := make(map[string]string)
	for i := 0; i < z.NumField(); i++ {
		doc[z.Field(i).Tag.Get("json")] = z.Field(i).Tag.Get("doc")
	}
	fmt.Printf("name字段为：%s\n", doc["name"]) // name字段为：我的名字

	// reflect.Type和reflect.Value都提供了一个Method方法。每次t.Method(i)调用将一个
	// reflect.Method的实例,对应一个用于描述一个方法的名称和类型的结构体。每次v.Method(i)
	// 方法调用都返回一个reflect.Value以表示对应的值,也就是一个方法是帮到它的接收
	// 者的。使用reflect.Value.Call方法,将可以调用一个Func类型的Value
	Print(time.Hour)
	// type time.Duration
	// func(time.Duration) Hours() float64
	// func(time.Duration) Microseconds() int64
	// func(time.Duration) Milliseconds() int64
	// func(time.Duration) Minutes() float64
	// func(time.Duration) Nanoseconds() int64
	// func(time.Duration) Round(time.Duration) time.Duration
	// func(time.Duration) Seconds() float64
	// func(time.Duration) String() string
	// func(time.Duration) Truncate(time.Duration) time.Duration
	Print(new(strings.Replacer))
	// type *strings.Replacer
	// func(*strings.Replacer) Replace(string) string
	// func(*strings.Replacer) WriteString(io.Writer, string) (int, error)
}
