package main

import (
	"fmt"
	"strconv"
	"unicode/utf8"
)

func main() {
	// 一个字符串是一个不可改变的字节序列。字符串可以包含任意的数据,包括byte值0,但是通
	// 常是用来包含人类可读的文本。文本字符串通常被解释为采用UTF8编码的Unicode码点
	// (rune)序列

	// 内置的len函数可以返回一个字符串中的字节数目(不是rune字符数目),索引操作s[i]返回第i
	// 个字节的字节值,i必须满足0≤i<len(s)条件约束。
	s := "hello,world"
	fmt.Println(len(s))     // 11
	fmt.Println(s[0], s[7]) // 104 111

	// 试图访问超出字符串索引范围的字节将会导致panic异常
	// fmt.Println(s[len(s)]) // panic:index out of range

	// 第i个字节并不一定是字符串的第i个字符,因为对于非ASCII字符的UTF8编码会要两个或多个字节。

	// 子字符串操作s[i:j]基于原始的s字符串的第i个字节开始到第j个字节(并不包含j本身)生成一
	// 个新字符串。生成的新字符串将包含j-i个字节。
	// 如果索引超出字符串范围或者j小于i的话将导致panic异常。
	fmt.Println(s[0:5]) // hello

	// 不管i还是j都可能被忽略,当它们被忽略时将采用0作为开始位置,采用len(s)作为结束的位置。
	fmt.Println(s[:5]) // hello
	fmt.Println(s[6:]) // world
	fmt.Println(s[:])  // hello,world

	// +操作符将两个字符串链接构造一个新字符串
	fmt.Println("goodbye" + s[5:]) // goodbye,world

	// 字符串可以用==和<进行比较;比较通过逐个字节比较完成的,因此比较的结果是字符串自然编码的顺序。

	// 字符串的值是不可变的:一个字符串包含的字节序列永远不会被改变
	// 因为字符串是不可修改的,因此尝试修改字符串内部数据的操作也是被禁止的
	// s[0] = 'L' // compile error:	cannot assign to s[0]
	// 可以给一个字符串变量分配一个新字符串值,这并不会导致原始的字符串值被改变,但是变量s将因为+=
	// 语句持有一个新的字符串值,但是t依然是包含原先的字符串值。
	s1 := "left foot"
	t := s1
	s1 += ",right foot"
	fmt.Println(s1) // left foot,right foot
	fmt.Println(t)  // left foot

	// 不变性意味如果两个字符串共享相同的底层数据的话也是安全的,这使得复制任何长度的字
	// 符串代价是低廉的。同样,一个字符串s和对应的子字符串切片s[7:]的操作也可以安全地共享
	// 相同的内存,因此字符串切片操作代价也是低廉的。在这两种情况下都没有必要分配新的内存。

	// 字符串值也可以用字符串面值方式编写,只要将一系列字节序列包含在双引号即可。
	// Go语言源文件总是用UTF8编码,并且Go语言的文本字符串也以UTF8编码的方式处理,
	// 因此我们可以将Unicode码点也写到字符串面值中。

	// 在一个双引号包含的字符串面值中,可以用以反斜杠\开头的转义序列插入任意的数据。下
	// 面的换行、回车和制表符等是常见的ASCII控制代码的转义方式:
	// \a						响铃
	// \b						退格
	// \f						换页
	// \n						换行
	// \r						回车
	// \t						制表符
	// \v						垂直制表符
	// \'						单引号	(只用在	'\''	形式的rune符号面值中)
	// \"						双引号	(只用在	"..."	形式的字符串面值中)
	// \\						反斜杠

	// 可以通过十六进制或八进制转义在字符串面值包含任意的字节。一个十六进制的转义形式
	// 是\xhh,其中两个h表示十六进制数字(大写或小写都可以)。一个八进制转义形式
	// 是\ooo,包含三个八进制的o数字(0到7),但是不能超过\377(译注:对应一个字节的
	// 范围,十进制为255)。每一个单一的字节表达一个特定的值。
	fmt.Println("nihao, \x61 \142") // nihao, a b

	// 一个原生的字符串面值形式是`...`,使用反引号代替双引号。在原生的字符串面值中,没有转
	// 义操作;全部的内容都是字面的意思,包含退格和换行,因此一个程序中的原生字符串面值
	// 可能跨越多行(在原生字符串面值内部是无法直接写`字符的,可以用八进制或十六进
	// 制转义或+"`"链接字符串常量完成)。唯一的特殊处理是会删除回车以保证在所有平台上的值
	// 都是一样的,包括那些把回车也放入文本文件的系统(Windows系统会把回车和换行
	// 一起放入文本文件中)。
	// 原生字符串面值用于编写正则表达式会很方便,因为正则表达式往往会包含很多反斜杠。原
	// 生字符串面值同时被广泛应用于HTML模板、JSON面值、命令行提示信息以及那些需要扩展
	// 到多行的场景。

	// Unicode收集了这个世界上所有的符号系统,包括重音符号和其它变音符号,制表符和回车符,
	// 还有很多神秘的符号,每个符号都分配一个唯一的Unicode码点,Unicode码点对应Go语言中
	// 的rune整数类型(rune是int32等价类型)。
	// 在第八版本的Unicode标准收集了超过120,000个字符,涵盖超过100多种语言。

	// UTF8是一个将Unicode码点编码为字节序列的变长编码。UTF8编码使用1到4个字节
	// 来表示每个Unicode码点,ASCII部分字符只使用1个字节,常用字符部分使用2或3个字节表
	// 示。每个符号编码后第一个字节的高端bit位用于表示总共有多少编码个字节。如果第一个字
	// 节的高端bit为0,则表示对应7bit的ASCII字符,ASCII字符每个字符依然是一个字节,和传统
	// 的ASCII编码兼容。如果第一个字节的高端bit是110,则说明需要2个字节;后续的每个高端bit
	// 都以10开头。更大的Unicode码点也是采用类似的策略处理。
	// 0xxxxxxx								runes 0-127     (ASCII)
	// 110xxxxx 10xxxxxx						128-2047		(values	<128 unused)
	// 1110xxxx 10xxxxxx 10xxxxxx				2048-65535		(values	<2048 unused)
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx		65536-0x10ffff	(other values unused)
	// 变长的编码无法直接通过索引来访问第n个字符,但是UTF8编码获得了很多额外的优点。首
	// 先UTF8编码比较紧凑,完全兼容ASCII码,并且可以自动同步:它可以通过向前回朔最多2个
	// 字节就能确定当前字符编码的开始字节的位置。它也是一个前缀编码,所以当从左向右解码
	// 时不会有任何歧义也并不需要向前查看(像GBK之类的编码,如果不知道起点位置则
	// 可能会出现歧义)。没有任何字符的编码是其它字符编码的子串,或是其它编码序列的字
	// 串,因此搜索一个字符时只要搜索它的字节编码序列即可,不用担心前后的上下文会对搜索
	// 结果产生干扰。同时UTF8编码的顺序和Unicode码点的顺序一致,因此可以直接排序UTF8编
	// 码序列。同时因为没有嵌入的NUL(0)字节,可以很好地兼容那些使用NUL作为字符串结尾的
	// 编程语言。

	// Go语言的源文件采用UTF8编码,并且Go语言处理UTF8编码的文本也很出色。unicode包提
	// 供了诸多处理rune字符相关功能的函数(比如区分字母和数组,或者是字母的大写和小写转
	// 换等),unicode/utf8包则提供了用于rune字符序列的UTF8编码和解码的功能。

	// Go语言字符串面值中的Unicode转义字符让我们可以通过Unicode码点输入特殊的字符。有两种形式:
	// \uhhhh对应16bit的码点值,\Uhhhhhhhh对应32bit的码点值,其中h是一个十六进制数字;一般很少需要
	// 使用32bit的形式。每一个对应码点的UTF8编码。下面的字母串面值都表示相同的值:
	fmt.Println("世界")                                     // 世界
	fmt.Println("\xe4\xb8\x96\xe7\x95\x8c")               // 世界
	fmt.Println("\u4e16\u754c")                           // 世界
	fmt.Println("\U00004e16\U0000754c")                   // 世界
	fmt.Printf("%c,%c,%c\n", '世', '\u4e16', '\U00004e16') // 世,世,世

	// 对于小于256码点值可以写在一个十六进制转义字节中,例如\x41对应字符'A',但是对于更
	// 大的码点则必须使用\u或\U转义形式。因此,\xe4\xb8\x96并不是一个合法的rune字符,
	// 虽然这三个字节对应一个有效的UTF8编码的码点。

	// 统计混合字符串的数量
	s2 := "Hello,世界"
	fmt.Println(len(s2))                    //	12
	fmt.Println(utf8.RuneCountInString(s2)) //	8
	n := 0
	for _, _ = range s2 {
		n++
	}
	fmt.Println(n) // 8
	n1 := 0
	for range s {
		n1++
	}
	fmt.Println(n) // 8

	for i := 0; i < len(s2); {
		// DecodeRuneInString函数都返回一个r和长度,r对应字符本身,长度对应r采用
		// UTF8编码后的编码字节数目。长度可以用于更新第i个字符在字符串中的字节索引位置。
		r, size := utf8.DecodeRuneInString(s2[i:])
		fmt.Printf("%c(%d)|", r, i) // H(0)|e(1)|l(2)|l(3)|o(4)|,(5)|世(6)|界(9)|
		i += size
	}
	fmt.Println()

	// range循环在处理字符串的时候,会自动隐式解码UTF8字符串。需要注意的是对于非
	// ASCII,索引更新的步长将超过1个字节。
	// 0       'H'     72
	// 1       'e'     101
	// 2       'l'     108
	// 3       'l'     108
	// 4       'o'     111
	// 5       ','     44
	// 6       '世'    19990
	// 9       '界'    30028
	for i, r := range "Hello,世界" {
		fmt.Printf("%d\t%q\t%d\n", i, r, r)
	}

	// 每一个UTF8字符解码,不管是显式地调用utf8.DecodeRuneInString解码或是在range循环中
	// 隐式地解码,如果遇到一个错误的UTF8编码输入,将生成一个特别的Unicode字符\uFFFD,
	// 在印刷中这个符号通常是一个黑色六角或钻石形状,里面包含一个白色的问号"�"。
	// 如果对应码点的字符是无效的,则用\uFFFD无效字符作为替换。
	fmt.Printf("%c\n", '\uFFFD') // �

	// UTF8字符串作为交换格式是非常方便的,但是在程序内部采用rune序列可能更方便,因为
	// rune大小一致,支持数组索引和方便切割。
	// 将[]rune类型转换应用到UTF8编码的字符串,将返回字符串编码的Unicode码点序列:
	s3 := "プログラム"
	fmt.Printf("% x\n", s3) // e3 83 97 e3 83 ad e3 82 b0 e3 83 a9 e3 83 a0
	r := []rune(s3)
	fmt.Printf("%x\n", r) // [30d7 30ed 30b0 30e9 30e0]
	// 将一个[]rune类型的Unicode字符slice或数组转为string,则对它们进行UTF8编码
	fmt.Println(string(r)) // プログラム

	// 标准库中有四个包对字符串处理尤为重要:bytes、strings、strconv和unicode包。strings包
	// 提供了许多如字符串的查询、替换、比较、截断、拆分和合并等功能。
	// bytes包也提供了很多类似功能的函数,但是针对和字符串有着相同结构的[]byte类型。因为字
	// 符串是只读的,因此逐步构建字符串会导致很多分配和复制。在这种情况下,使用
	// bytes.Buffer类型将会更有效。
	// strconv包提供了布尔型、整型数、浮点数和对应字符串的相互转换,还提供了双引号转义相
	// 关的转换。

	// unicode包提供了IsDigit、IsLetter、IsUpper和IsLower等类似功能,它们用于给字符分类。每
	// 个函数有一个单一的rune类型的参数,然后返回一个布尔值。而像ToUpper和ToLower之类的
	// 转换函数将用于rune字符的大小写转换。所有的这些函数都是遵循Unicode标准定义的字母、
	// 数字等分类规范。strings包也有类似的函数,它们是ToUpper和ToLower,将原始字符串的每
	// 个字符都做相应的转换,然后返回新的字符串。

	// 一个字符串是包含的只读字节数组,一旦创建,是不可变的。相比之下,一个字节slice的元
	// 素则可以自由地修改。字符串和字节slice之间可以相互转换:
	s4 := "abc"
	// 从概念上讲,一个[]byte(s)转换是分配了一个新的字节数组用于保存字符串数据的拷贝,然后
	// 引用这个底层的字节数组。编译器的优化可以避免在一些场景下分配和复制字符串数据,但
	// 总的来说需要确保在变量b被修改的情况下,原始的s字符串也不会改变。
	b := []byte(s4)
	// 将一个字节slice转到字符串的string(b)操作则是构造一个字符串拷贝,以确保字符串是只读的。
	s5 := string(b)
	fmt.Println(s5) // abc

	// 为了避免转换中不必要的内存分配,bytes包和strings同时提供了许多实用函数。
	// strings包中的六个函数:
	// func	Contains(s, substr string) bool
	// func	Count(s,sep string)	int
	// func	Fields(s string) []string
	// func	HasPrefix(s, prefix string)	bool
	// func	Index(s, sep string) int
	// func	Join(a []string, sep string) string
	// bytes包中也对应的六个函数:
	// func	Contains(b,	subslice []byte) bool
	// func	Count(s, sep []byte)	int
	// func	Fields(s []byte) [][]byte
	// func	HasPrefix(s, prefix	[]byte) bool
	// func	Index(s, sep []byte)	int
	// func	Join(s [][]byte, sep []byte) []byte
	// 它们之间唯一的区别是字符串类型参数被替换成了字节slice类型的参数。

	// bytes包还提供了Buffer类型用于字节slice的缓存。一个Buffer开始是空的,但是随着string、
	// byte或[]byte等类型数据的写入可以动态增长,一个bytes.Buffer变量并不需要初始化,因为零
	// 值也是有效的。
	// 当向bytes.Buffer添加任意字符的UTF8编码时,最好使用bytes.Buffer的WriteRune方法,但是
	// WriteByte方法对于写入类似'['和']'等ASCII字符则会更加有效。

	// 除了字符串、字符、字节之间的转换,字符串和数值之间的转换也比较常见。由strconv包提
	// 供这类转换功能。

	// 将一个整数转为字符串,一种方法是用fmt.Sprintf返回一个格式化的字符串;另一个方法是用
	// strconv.Itoa(“整数到ASCII”):
	x := 123
	y := fmt.Sprintf("%d", x)
	fmt.Println(y, strconv.Itoa(x)) // 123 123

	// FormatInt和FormatUint函数可以用不同的进制来格式化数字
	fmt.Println(strconv.FormatInt(int64(x), 2)) // 1111011

	// fmt.Printf函数的%b、%d、%o和%x等参数提供功能往往比strconv包的Format函数方便很
	// 多,特别是在需要包含附加额外信息的时候。

	// 如果要将一个字符串解析为整数,可以使用strconv包的Atoi或ParseInt函数,还有用于解析无
	// 符号整数的ParseUint函数。
	x1, _ := strconv.Atoi("123")
	y1, _ := strconv.ParseInt("123", 10, 64)
	fmt.Println(x1, y1) // 123 123

	// 有时候也会使用fmt.Scanf来解析输入的字符串和数字,特别是当字符串和数字混合在一行的
	// 时候,它可以灵活处理不完整或不规则的输入。
}

// 对于UTF8编码后文本的处理和原始的字节处理逻辑是一样的。但是对应很多其它编码则并不是这样的:
// 测试一个字符串是否是另一个字符串的前缀
func HasPrefix(s, prefix string) bool {
	return len(s) >= len(prefix) && s[:len(prefix)] == prefix
}

// 测试一个字符串是否是另一个字符串的后缀
func HasSuffix(s, suffix string) bool {
	return len(s) >= len(suffix) && s[len(s)-len(suffix):] == suffix
}

// 包含子串测试
func Contains(s, substr string) bool {
	for i := 0; i < len(s); i++ {
		if HasPrefix(s[i:], substr) {
			return true
		}
	}
	return false
}
