package main

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"time"
)

// import "io"
// import "bufio"

// 函数声明包括函数名、形式参数列表、返回值列表(可省略)以及函数体。
// func name(parameter-list) (result-list)	{
// 	body
// }
// 形式参数列表描述了函数的参数名以及参数类型。这些参数作为局部变量,其值由参数调用
// 者提供。返回值列表描述了函数返回值的变量名以及类型。如果函数返回一个无名变量或者
// 没有返回值,返回值列表的括号是可以省略的。如果一个函数声明不包括返回值列表,那么
// 函数体执行完毕后,不会返回任何值。

// 返回值也可以像形式参数一样被命名。在这种情况下,每个返回值被声明成一个局部变量,并根据该返回
// 值的类型,将其初始化为0。
// 如果一个函数在声明时,包含返回值列表,该函数必须以return语句结尾,除非函数明显无法运行到结尾处。
// 例如函数在结尾时调用了panic异常或函数中存在无限循环。

// 4种方法声明有2个int型参数和1个int型返回值的函数：
func add(x int, y int) int   { return x + y }
func sub(x, y int) (z int)   { z = x - y; return }
func first(x int, _ int) int { return x } // 强调某个参数未被使用
func zero(int, int) int      { return 0 }

// 函数的类型被称为函数的标识符。如果两个函数形式参数列表和返回值列表中的变量类型一
// 一对应,那么这两个函数被认为有相同的类型和标识符。形参和返回值的变量名不影响函数
// 标识符也不影响它们是否可以以省略参数类型的形式表示。

// 每一次函数调用都必须按照声明顺序为所有参数提供实参(参数值)。在函数调用时,Go语
// 言没有默认参数值,也没有任何方法可以通过参数名指定形参,因此形参和返回值的变量名
// 对于函数调用者而言没有意义。

// 在函数体中,函数的形参作为局部变量,被初始化为调用者提供的值。函数的形参和有名返
// 回值作为函数最外层的局部变量,被存储在相同的词法块中。

// 实参通过值的方式传递,因此函数的形参是实参的拷贝。对形参进行修改不会影响实参。但
// 是,如果实参包括引用类型,如指针,slice(切片)、map、function、channel等类型,实参可
// 能会由于函数的间接引用被修改。

// 遇到没有函数体的函数声明,这表示该函数不是以Go实现的。这样的声明定义了函数标识符。
// func Sin(x float64) float

// 函数可以是递归的,这意味着函数可以直接或间接的调用自身。

// 在Go中,一个函数可以返回多个值,许多标准库中的函数返回2个值,一个是期望得到的返回值,另一个是函数出错时的错误信息。
func test1() (int, int) { return 1, 2 }

// 一个函数内部可以将另一个有多返回值的函数作为返回值
func test2() (int, int) { return test1() }

// 虽然良好的命名很重要,但你也不必为每一个返回值都取一个适当的名字。
// 如果一个函数将所有的返回值都显示的变量名,那么该函数的return语句可以省略操作数。
func test3() (x int, y int) { return } // 这称之为bare return

// 第二种处理错误
// func WaitForServer(url string) error {
// 	const timeout =	1 *	time.Minute
// 	deadline := time.Now().Add(timeout)
// 	for	tries := 0;	time.Now().Before(deadline); tries++ {
// 		if	err	==	nil	{
// 			return	nil	//	success
// 		}
// 		log.Printf("server	not	responding	(%s);retrying...",	err)
// 		time.Sleep(time.Second	<<	uint(tries))	//	exponential	back-off
// 	}
// 	return	fmt.Errorf("server %s failed to respond after %s", url, timeout)
// }

func test4() (int, error) { return 5, fmt.Errorf("hello error") }

// 函数被看作第一类值(first-class values):函数像其他值一样,拥有类型,可以被
// 赋值给其他变量,传递给函数,从函数返回。对函数值(function value)的调用类似函数调用。
func square(n int) int     { return n * n }
func negative(n int) int   { return -n }
func product(m, n int) int { return m * n }

// 拥有函数名的函数只能在包级语法块中被声明,通过函数字面量(function literal),我们可
// 绕过这一限制,在任何表达式中表示一个函数值。函数字面量的语法和函数声明相似,区别
// 在于func关键字后没有函数名。函数值字面量是一种表达式,它的值被成为匿名函数
// (anonymous function)。函数字面量允许我们在使用函数时,再定义它。
func squares() func() int {
	var x int
	return func() int {
		x++
		return x * x
	}
}

// 当匿名函数需要被递归调用时,我们必须首先声明一个变量(visitAll),再将匿名函数赋值给这个变量。
// 如果不分成两部,函数字面量无法与visitAll绑定,也无法递归调用该匿名函数。

// 参数数量可变的函数称为为可变参数函数。在声明可变参数函数时,需要在参数列表的最后一个参数类型之前
// 加上省略符号“...”,这表示该函数会接收任意数量的该类型参数。
func sum(vals ...int) int { // vals被看作是类型为[]int的切片
	total := 0
	for _, val := range vals {
		total += val
	}
	return total
}

// 虽然在可变参数函数内部,...int型参数的行为看起来很像切片类型,但实际上,可变参数函
// 数和以切片作为参数的函数是不同的。
func ff(...int) {}
func gg([]int)  {}

// interfac{}表示函数的最后一个参数可以接收任意类型
func errorf(linenum int, format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, "Line %d: ", linenum)
	fmt.Fprintf(os.Stderr, format, args...)
	fmt.Fprintln(os.Stderr)
}

// 只需要在调用普通函数或方法前加上关键字defer,就完成了defer所需要的语法。当defer语
// 句被执行时,跟在defer后面的函数会被延迟执行。直到包含该defer语句的函数执行完毕时,
// defer后的函数才会被执行,不论包含defer语句的函数是通过return正常结束,还是由于panic
// 导致的异常结束。你可以在一个函数中执行多条defer语句,它们的执行顺序与声明顺序相反。
// defer语句经常被用于处理成对的操作,如打开、关闭、连接、断开连接、加锁、释放锁。通
// 过defer机制,不论函数逻辑多复杂,都能保证在任何执行路径下,资源被释放。释放资源的
// defer应该直接跟在请求资源的语句后。

// 调试复杂程序时,defer机制也常被用于记录何时进入和退出函数。
func bigSlowOperation() {
	// 不要忘记defer语句后的圆括号,否则本该在进入时执行的操作会在退出时执行,而本该在退出时执行的,永远不会被执行。
	defer trace("bigSlowOperation")()
	// do something
	time.Sleep(2 * time.Second)
}
func trace(msg string) func() {
	start := time.Now()
	log.Printf("enter %s", msg)
	return func() {
		log.Printf("exit %s (%s)", msg, time.Since(start))
	}
}

// defer语句中的函数会在return语句更新返回值变量后再执行,又因为在函数中定义
// 的匿名函数可以访问该函数包括返回值变量在内的所有变量,所以,对匿名函数采用defer机
// 制,可以使其观察函数的返回值。
func double(x int) (result int) {
	defer func() { fmt.Printf("double(%d) = %d\n", x, result) }()
	return x + x
}

// 被延迟执行的匿名函数甚至可以修改函数返回给调用者的返回值:
func triple(x int) (result int) {
	defer func() { result += x }()
	return double(x)
}

// 在循环体中的defer语句需要特别注意,因为只有在函数执行完毕后,这些被延迟的函数才会执行。
// 一种解决方法是将循环体中的defer语句移至另外一个函数。在每次循环时,调用这个函数。

// Go的类型系统会在编译时捕获很多错误,但有些错误只能在运行时检查,如数组访问越界、
// 空指针引用等。这些运行时错误会引起painc异常。
// 一般而言,当panic异常发生时,程序会中断运行,并立即执行在该goroutine)中被延迟的函数(defer机制)。
// 随后,程序崩溃并输出日志信息。日志信息包括panic value和函数调用的堆栈跟踪信息。panic value
// 通常是某种错误信息。对于每个goroutine,日志信息中都会有与之相对的,发生panic时的函数调用堆栈跟踪信
// 息。通常,我们不需要再次运行程序去定位问题,日志信息已经提供了足够的诊断依据。
// 不是所有的panic异常都来自运行时,直接调用内置的panic函数也会引发panic异常;panic函
// 数接受任何值作为参数。当某些不应该发生的场景发生时,就应该调用panic。
func fff(x int) {
	fmt.Printf("f(%d)\n", x+0/x) //	panics if x == 0
	defer fmt.Printf("defer %d\n", x)
	fff(x - 1)
}

// 为了方便诊断问题,runtime包允许程序员输出堆栈信
// 在Go的panic机制中,延迟函数的调用在释放堆栈信息之前。
func printStack() {
	var buf [4096]byte
	n := runtime.Stack(buf[:], false)
	os.Stdout.Write(buf[:n])
}

// 通常来说,不应该对panic异常做任何处理,但有时,也许我们可以从异常中恢复,至少可以在程序崩溃前,做一些操作。
// 不加区分的恢复所有的panic异常,不是可取的做法;因为在panic之后,无法保证包级变量的状态仍然和我们预期一致。
// 有些情况下,我们无法恢复。某些致命错误会导致Go在运行时终止程序,如内存不足。
// 使用panic和recover编写一个不包含return语句但能返回一个非零值的函数:
func returnN() (result int) {
	defer func() {
		if p := recover(); p != nil {
			result = p.(int)
		}
	}()
	panic(3)
}

func main() {
	fmt.Printf("%T\n", add)   // func(int, int) int
	fmt.Printf("%T\n", sub)   // func(int, int) int
	fmt.Printf("%T\n", first) // func(int, int) int
	fmt.Printf("%T\n", zero)  // func(int, int) int

	// 调用多返回值函数时,返回给调用者的是一组值,调用者必须显式的将这些值分配给变量:
	r1, r2 := test1()
	// 如果某个值不被使用,可以将其分配给blank identifier
	r3, _ := test1()
	fmt.Println(r1, r2, r3) // 1 2 1

	// 当你调用接受多参数的函数时,可以将一个返回多参数的函数作为该函数的参数:
	fmt.Println(test2()) // 1 2

	fmt.Println(test3()) // 0 0

	// 内置的error是接口类型。error类型可能是nil或者non-nil。nil意味着函数运行成功,
	// non-nil表示失败。对于non-nil的error类型,我们可以通过调用error的Error函数或
	// 者输出函数获得字符串类型的错误信息。
	// fmt.Println(err)
	// fmt.Printf("%v", err)
	// 函数返回non-nil的error时,其他的返回值是未定义的(undefined),这些未定义的返回
	// 值应该被忽略。然而,有少部分函数在发生错误时,仍然会返回一些有用的返回值。比如,
	// 当读取文件发生错误时,Read函数会返回可以读取的字节数以及错误信息。对于这种情况,
	// 正确的处理方式应该是先处理这些不完整的数据,再处理错误。

	// 常用的五种错误处理方式
	// 1、最常用的方式是传播错误。这意味着函数中某个子程序的失败,会变成该函数的失败:
	// resp, err := http.Get(url)
	// if err != nil{
	// 	return nil, err
	//  // return nil,fmt.Errorf("parsing :	%v", url,err)
	// }
	// fmt.Errorf函数使用fmt.Sprintf格式化错误信息并返回。我们使用该函数前缀添加额外
	// 的上下文信息到原始错误信息。
	// 2、如果错误的发生是偶然性的,或由不可预知的问题导致的。一个明智的选择是重新尝试失
	// 败的操作。在重试时,我们需要限制重试的时间间隔或重试的次数,防止无限制的重试。
	// 3、输出错误信息并结束程序。需要注意的是,这种策略只应在main中执行。对库函数而言,
	// 应仅向上传播错误,除非该错误意味着程序内部包含不一致性,即遇到了bug,才能在库函数中结束程序。
	// if _, err := test4(); err !=	nil	{
	// 	fmt.Fprintf(os.Stderr, "Site is down: %v\n", err)
	// 	os.Exit(1)
	// }
	// 调用log.Fatalf可以更简洁的代码达到与上文相同的效果。log中的所有函数,都默认会在错误
	// 信息之前输出时间信息。
	// if _ ,err := test4(); err != nil	{
	//  log.SetPrefix("wait: ")
	//  log.SetFlags(0)
	// 	log.Fatalf("Site is down: %v\n", err)
	// }
	// 4、有时只需要输出错误信息就足够了,不需要中断程序的运行。可以通过log包提供函数：
	// if err := Ping(); err != nil {
	// 	log.Printf("ping failed: %v; networking	disabled",err)
	// }
	// if err := Ping(); err != nil {
	// 	fmt.Fprintf(os.Stderr,	"ping failed: %v; networking disabled\n", err)
	// }
	// log包中的所有函数会为没有换行符的字符串增加换行符。
	// 5、最后一种策略:我们可以直接忽略掉错误：
	// os.RemoveAll(dir) // 尽管os.RemoveAll会失败,这是因为操作系统会定期的清理临时目录。

	// 在Go中,错误处理有一套独特的编码风格。检查某个子函数是否失败后,我们通常将处理失
	// 败的逻辑代码放在处理成功的代码之前。如果某个错误会导致函数返回,那么成功时的逻辑
	// 代码不应放在else语句块中,而应直接放在函数体中。Go中大部分函数的代码结构几乎相
	// 同,首先是一系列的初始检查,防止错误发生,之后是函数的实际逻辑。

	// in := bufio.NewReader(os.Stdin)
	// for	{
	// 	r, _, err:=in.ReadRune()
	// 	if err == io.EOF{ // ctrl + D
	// 	    fmt.Printf("read EOF:%v\n", err)
	// 		break //finished reading
	// 	}
	// 	if err != nil{
	// 	    fmt.Printf("read failed:%v", err)
	// 	    return
	// 	}
	// 	//	...use	r...
	// 	fmt.Printf("read :%v\n", r)
	// }

	f := square
	fmt.Println(f(3)) // 9
	f = negative
	fmt.Println(f(3))     // -3
	fmt.Printf("%T\n", f) // func(int) int
	// f = product // cannot use product (type func(int, int) int) as type func(int) int in assignment

	// 函数类型的零值是nil。调用值为nil的函数值会引起panic错误:
	// var f1 func(int) int
	// f1(3) // panic
	if f != nil { // 函数值可以与nil比较, 但是函数值之间是不可比较的,也不能用函数值作为map的key。
		f(3)
	}

	// 函数值不仅仅是一串代码,还记录了状态。在squares中定义的匿名内
	// 部函数可以访问和更新squares中的局部变量,这意味着匿名函数和squares中,存在变量引
	// 用。这就是函数值属于引用类型和函数值不可比较的原因。Go使用闭包(closures)技术实
	// 现函数值,Go程序员也把函数值叫做闭包。
	f2 := squares()
	fmt.Println(f2()) // 1
	fmt.Println(f2()) // 4
	fmt.Println(f2()) // 9
	fmt.Println(f2()) // 16
	// 通过这个例子看到变量的生命周期不由它的作用域决定:squares返回后,变量x仍然隐式的存在于f中。

	fmt.Println(sum())           // 0
	fmt.Println(sum(3))          // 3
	fmt.Println(sum(1, 2, 3, 4)) // 10

	fmt.Printf("%T\n", ff) // func(...int)
	fmt.Printf("%T\n", gg) // func([]int)

	linenum, name := 12, "count"
	errorf(linenum, "undefined: %s", name) // Line 12: undefined: count

	bigSlowOperation()
	// 2020/12/21 23:44:30 enter bigSlowOperation
	// 2020/12/21 23:44:40 exit bigSlowOperation (10.000289891s)

	_ = double(4)          // double(4) = 8
	fmt.Println(triple(4)) // 12

	fmt.Println(returnN())

	defer printStack()
	fff(3)
	// f(3)
	// f(2)
	// f(1)
	// defer 1
	// defer 2
	// defer 3
	// panic: runtime error: integer divide by zero
}
