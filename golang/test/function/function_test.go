package word

import (
	"math/rand"
	"testing" // 每个测试函数必须导入testing包。
	"time"
)

// 测试函数有如下的签名:
// func TestName(t *testing.T) {
// }

// 测试函数的名字必须以Test开头,可选的后缀名必须以大写字母开头:
// func TestSin(t *testing.T) { /*	...	*/ }
// func TestCos(t *testing.T) { /*	...	*/ }
// func TestLog(t *testing.T) { /*	...	*/ }
// 其中t参数用于报告测试失败和附加的日志信息。

// 失败测试的输出并不包括调用t.Errorf时刻的堆栈调用信息。和其他编程语言或测试框架的
// assert断言不同,t.Errorf调用也没有引起panic异常或停止测试的执行。即使表格中前面的数
// 据导致了测试的失败,表格后面的测试数据依然会运行测试,因此在一个测试中我们可能了
// 解多个失败的信息。
// 如果我们真的需要停止测试,或许是因为初始化失败或可能是早先的错误导致了后续错误等
// 原因,我们可以使用t.Fatal或t.Fatalf停止当前测试函数。它们必须在和测试函数同一个
// goroutine内调用。

// 考虑下这两个包:net/url包,提供了URL解析的功能;net/http包,提供了web服务和HTTP客
// 户端的功能。如我们所料,上层的net/http包依赖下层的net/url包。然后,net/url包中的一个测
// 试是演示不同URL和HTTP客户端的交互行为。也就是说,一个下层包的测试代码导入了上层的包。
// 这样的行为在net/url包的测试代码中会导致包的循环依赖,Go语言规范是禁止包的循环依赖的。
// 不过我们可以通过外部测试包的方式解决循环依赖的问题,也就是在net/url包所在的目录声明
// 一个独立的url_test测试包。
// 通过避免循环的导入依赖,外部测试包可以更灵活地编写测试,特别是集成测试(需要测试
// 多个组件之间的交互),可以像普通应用程序那样自由地导入其他包。
// 可以用go list命令查看包对应目录中哪些Go源文件是产品代码,哪些是包内测试,还有
// 哪些是外部测试包。我们以fmt包作为一个例子:GoFiles表示产品代码对应的Go源文件列
// 表;也就是go build命令要编译的部分。
// go list -f={{.GoFiles}} fmt // [doc.go errors.go format.go print.go scan.go]
// go list -f={{.TestGoFiles}} fmt // [export_test.go]
// go list -f={{.XTestGoFiles}} fmt // [errors_test.go example_test.go fmt_test.go
// gostringer_example_test.go scan_test.go stringer_example_test.go stringer_test.go]

// 许多Go语言新人会惊异于Go语言极简的测试框架。很多其它语言的测试框架都提供了识别测
// 试函数的机制(通常使用反射或元数据),通过设置一些“setup”和“teardown”的钩子函数来执
// 行测试用例运行的初始化和之后的清理操作,同时测试工具箱还提供了很多类似assert断言、
// 值比较函数、格式化输出错误信息和停止一个失败的测试等辅助函数(通常使用异常机
// 制)。虽然这些机制可以使得测试非常简洁,但是测试输出的日志却会像火星文一般难以理
// 解。此外,虽然测试最终也会输出PASS或FAIL的报告,但是它们提供的信息格式却非常不利
// 于代码维护者快速定位问题,因为失败信息的具体含义非常隐晦,比如“assert:0==1”或成页的海量跟踪日志。
// Go语言的测试风格则形成鲜明对比。它期望测试者自己完成大部分的工作,定义函数避免重
// 复,就像普通编程那样。编写测试并不是一个机械的填空过程;一个测试也有自己的接口,
// 尽管它的维护者也是测试仅有的一个用户。一个好的测试不应该引发其他无关的错误信息,
// 它只要清晰简洁地描述问题的症状即可,有时候可能还需要一些上下文信息。在理想情况
// 下,维护者可以在不看代码的情况下就能根据错误信息定位错误产生的原因。一个好的测试
// 不应该在遇到一点小错误时就立刻退出测试,它应该尝试报告更多的相关的错误信息,因为
// 我们可能从多个失败测试的模式中发现错误产生的规律。
// 避免脆弱测试代码的方法是只检测你真正关心的属性。保持测试代码的简洁和内部结构的稳
// 定。特别是对断言部分要有所选择。不要对字符串进行全字匹配,而是针对那些在项目的发
// 展中是比较稳定不变的子串。很多时候值得花力气来编写一个从复杂输出中提取用于断言的
// 必要信息的函数,虽然这可能会带来很多前期的工作,但是它可以帮助迅速及时修复因为项
// 目演化而导致的不合逻辑的失败测试。

// 测试能证明缺陷存在,而无法证明没有缺陷。再多的测试也不能证明一个程序没有BUG。在最好的情况
// 下,测试可以增强我们的信心:代码在很多重要场景下是可以正常工作的。
// 语句的覆盖率是指在测试中至少被运行一次的代码占总代码数的比例。
// -coverprofile标志参数通过在测试代码中插入生成钩子来统计覆盖率数据。也就是说,在运行每个测
// 试前,它将待测代码拷贝一份并做修改,在每个词法块都会设置一个布尔标志变量。当被修
// 改后的被测试代码运行退出时,将统计日志数据写入c.out文件,并打印一部分执行的语句的
// 一个总结。(如果你需要的是摘要,使用go test -cover)
// 如果使用了-covermode=count标志参数,那么将在每个代码块插入一个计数器而不是布尔标
// 志量。在统计结果中记录了每个块的执行次数,这可以用于衡量哪些是被频繁执行的热点代码。
// go test -coverprofile=c.out
// go tool cover -html=c.out // 生成一个HTML报告,然后在浏览器中打开
// go tool命令运行Go工具链的底层可执行程序。这些底层可执行程序放在
// $GOROOT/pkg/tool/${GOOS}_${GOARCH}目录。因为有go uild命令的原因,很少直接调用这些底层工具。
// 实现100%的测试覆盖率听起来很美,但是在具体实践中通常是不可行的,也不是值得推荐的
// 做法。因为那只能说明代码被执行过而已,并不意味着代码就是没有BUG的;因为对于逻辑
// 复杂的语句需要针对不同的输入执行多次。有一些语句,例如上面的panic语句则永远都不会
// 被执行到。另外,还有一些隐晦的错误在现实中很少遇到也很难编写对应的测试代码。测试
// 从本质上来说是一个比较务实的工作,编写测试代码和编写应用代码的成本对比是需要考虑
// 的。测试覆盖率工具可以帮助我们快速识别测试薄弱的地方,但是设计好的测试用例和编写
// 应用代码一样需要严密的思考。

// 基准测试是测量一个程序在固定工作负载下的性能。在Go语言中,基准测试函数和普通测试
// 函数写法类似,但是以Benchmark为前缀名,并且带有一个*testing.B类型的参数;*testing.B
// 参数除了提供和*testing.T类似的方法,还有额外一些和性能测量相关的方
// 法。它还提供了一个整数N,用于指定操作执行的循环次数。

// func TestPalindrome(t *testing.T) {
// 	if !IsPalindrome("detartrated") {
// 		t.Error(`IsPalindrome("detartrated") = false`)
// 	}
// 	if !IsPalindrome("kayak") {
// 		t.Error(`IsPalindrome("kayak") = false`)
// 	}
// }
// func TestNonPalindrome(t *testing.T) {
// 	if IsPalindrome("palindrome") {
// 		t.Error(`IsPalindrome("palindrome") = true`)
// 	}
// }
// func TestFrenchPalindrome(t *testing.T) {
// 	if !IsPalindrome("été") {
// 		t.Error(`IsPalindrome("été") = false`)
// 	}
// }
// func TestCanalPalindrome(t *testing.T) {
// 	input := "A man, a plan, a canal: Panama"
// 	if !IsPalindrome(input) {
// 		t.Errorf(`IsPalindrome(%q) = false`, input)
// 	}
// }

// TestIsPalindrome测试仅仅使用导出的IsPalindrome函数,因此这是一个黑盒测试。
// 表格驱动的测试便于构造基于精心挑选的测试数据的测试用例。
func TestIsPalindrome(t *testing.T) {
	var tests = []struct {
		input string
		want  bool
	}{
		{"", true},
		{"a", true},
		{"aa", true},
		{"ab", false},
		{"kayak", true},
		{"detartrated", true},
		{"A	man, a plan, a canal: Panama", true},
		{"Evil I did dwell;	lewd did I live.", true},
		{"Able was I ere I saw Elba", true},
		{"été", true},
		{"Et se resservir, ivresse reste.", true},
		{"palindrome", false}, // non-palindrome
		{"desserts", false},   // semi-palindrome
	}
	for _, test := range tests {
		if got := IsPalindrome(test.input); got != test.want {
			t.Errorf("IsPalindrome(%q) = %v", test.input, got)
		}
	}
}

// 另一种测试思路是随机测试,也就是通过构造更广泛的随机输入来测试探索函数的行为。
// 那么对于一个随机的输入,我们如何能知道希望的输出结果呢?这里有两种处理策略。第一
// 个是编写另一个对照函数,使用简单和清晰的算法,虽然效率较低但是行为和要测试的函数
// 是一致的,然后针对相同的随机输入检查两者的输出结果。第二种是生成的随机输入的数据
// 遵循特定的模式,这样我们就可以知道期望的输出的模式。
func randomPalindrome(rng *rand.Rand) string {
	n := rng.Intn(25) // random length up to 24
	runes := make([]rune, n)
	for i := 0; i < (n+1)/2; i++ {
		r := rune(rng.Intn(0x1000)) // random rune up to '\u0999'
		runes[i] = r
		runes[n-1-i] = r
	}
	return string(runes)
}
func TestRandomPalindromes(t *testing.T) {
	//	Initialize a pseudo-random number generator.
	seed := time.Now().UTC().UnixNano()
	t.Logf("Random seed: %d", seed)
	rng := rand.New(rand.NewSource(seed))
	for i := 0; i < 1000; i++ {
		p := randomPalindrome(rng)
		// t.Log(p)
		if !IsPalindrome(p) {
			t.Errorf("IsPalindrome(%q) = false", p)
		}
	}
}

func BenchmarkIsPalindrome(b *testing.B) {
	for i := 0; i < b.N; i++ {
		IsPalindrome("A	man, a plan, a canal: Panama")
	}
}
