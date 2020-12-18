package main

import (
	"fmt"
	"log"
	"os"
	"text/template"
)

// text/template和html/template等模板包提供了一个将变量值填充到一个文本或HTML格式的模板的机制。
// template包实现了数据驱动的用于生成文本输出的模板。
// 其实简单来说就是将一组文本嵌入另一组文本模版中，返回一个你期望的文本。

// 一个模板是一个字符串或一个文件,里面包含了一个或多个由双花括号包含的 {{action}}对
// 象。大部分的字符串只是按面值打印,但是对于actions部分将触发其它的行为。每个actions
// 都包含了一个用模板语言书写的表达式,一个action虽然简短但是可以输出复杂的打印值,模
// 板语言包含通过选择结构体的成员、调用函数或方法、表达式控制流if-else语句和range循环
// 语句,还有其它实例化模板等诸多特性。
func main() {
	// 用作模板的输入文本必须是utf-8编码的文本。"Action",即数据运算和控制单位由"{{"和"}}"界定（即{{Action}}）；
	// 在Action之外的所有文本都不做修改的拷贝到输出中。Action内部不能有换行，但注释可以有换行。

	// 调用的是变量时
	str := "world"
	tmpl1, err := template.New("test").Parse("hello, {{.}}\n") //建立一个名字为test的模版"hello, {{.}}"
	if err != nil {
		panic(err)
	}
	//将str的值合成到tmpl模版的{{.}}中，并将合成得到的文本输入到os.Stdout
	err = tmpl1.Execute(os.Stdout, str) // hello, world
	if err != nil {
		panic(err)
	}

	// 传入的是struct对象的值
	sweaters := Inventory{"wool", 17}
	tmpl, err := template.New("test").Parse("{{.Count}} of {{.Material}}\n") //{{.Count}}获取的是struct对象中的Count字段的值
	if err != nil {
		panic(err)
	}
	err = tmpl.Execute(os.Stdout, sweaters) // 17 of wool
	if err != nil {
		panic(err)
	}

	// 自定义的变量
	mine := &MyMethod{Say: "hello", Name: "student"}
	//先对变量$str1，$str2，$str3赋值，一个是直接将字符串值赋值，另两个是调用函数，将返回值赋值，然后再将变量值输出
	tmpl2, err := template.New("test").Parse("{{$str1 := .Say}}{{$str2 := .SayHello}}{{$str3 := .SayYouName .Name}}{{$str1}} {{$str2}}\n{{$str3}}\n")
	if err != nil {
		panic(err)
	}
	err = tmpl2.Execute(os.Stdout, mine)
	if err != nil {
		panic(err)
	}
	// hello world
	// my name is : student

	// 执行模板时，函数从两个函数字典中查找：首先是模板函数字典，然后是全局函数字典。
	// 一般不在模板内定义函数，而是使用Funcs方法添加函数到模板里。
	// 方法必须有一到两个返回值，如果是两个，那么第二个一定是error接口类型
	// 模版内定义函数:
	mine1 := &MyMethod{Name: "boss"}
	tmpl3, err := template.New("test").Parse("{{.SayHello}}\n{{.SayYouName .Name}}\n")
	if err != nil {
		panic(err)
	}
	err = tmpl3.Execute(os.Stdout, mine1) // my name is : boss
	if err != nil {
		panic(err)
	}
	// 使用Funcs方法添加函数到模板:
	funcMap := template.FuncMap{
		//在FuncMap中声明相应要使用的函数，然后就能够在template字符串中使用该函数
		"SayHello":   SayHello1,
		"SayYouName": SayYouName1,
	}
	name := "boss"
	tmpl4, err := template.New("test").Funcs(funcMap).Parse("{{SayHello}}\n{{SayYouName .}}\n")
	if err != nil {
		panic(err)
	}
	err = tmpl4.Execute(os.Stdout, name) // my name is : boss
	if err != nil {
		panic(err)
	}
	// 根据pipeline的定义，上面的{{SayYouName .}}等价于{{.|SayYouName}}

	// 预定义的全局函数如下：
	// and
	// 	函数返回它的第一个empty参数或者最后一个参数；
	// 	就是说"and x y"等价于"if x then y else x"；所有参数都会执行；
	// or
	// 	返回第一个非empty参数或者最后一个参数；
	// 	亦即"or x y"等价于"if x then x else y"；所有参数都会执行；
	// not
	// 	返回它的单个参数的布尔值的否定
	// len
	// 	返回它的参数的整数类型长度
	// index
	// 	执行结果为第一个参数以剩下的参数为索引/键指向的值；
	// 	如"index x 1 2 3"返回x[1][2][3]的值；每个被索引的主体必须是数组、切片或者字典。
	// print
	// 	即fmt.Sprint
	// printf
	// 	即fmt.Sprintf
	// println
	// 	即fmt.Sprintln
	// html
	// 	返回其参数文本表示的HTML逸码等价表示。
	// urlquery
	// 	返回其参数文本表示的可嵌入URL查询的逸码等价表示。
	// js
	// 	返回其参数文本表示的JavaScript逸码等价表示。
	// call
	// 	执行结果是调用第一个参数的返回值，该参数必须是函数类型，其余参数作为调用该函数的参数；
	// 	如"call .X.Y 1 2"等价于go语言里的dot.X.Y(1, 2)；
	// 	其中Y是函数类型的字段或者字典的值，或者其他类似情况；
	// 	call的第一个参数的执行结果必须是函数类型的值（和预定义函数如print明显不同）；
	// 	该函数类型值必须有1到2个返回值，如果有2个则后一个必须是error接口类型；
	// 	如果有2个返回值的方法返回的error非nil，模板执行会中断并返回给调用模板执行者该错误；
	// 布尔函数会将任何类型的零值视为假，其余视为真。
	name1 := "boss"
	tmpl5, err := template.New("test").Parse(`{{printf "%q\n" .}}`)
	if err != nil {
		panic(err)
	}
	err = tmpl5.Execute(os.Stdout, name1) // "boss"
	if err != nil {
		panic(err)
	}

	// 定义为函数的二元比较运算的集合：
	// eq 如果arg1 == arg2则返回真
	// ne 如果arg1 != arg2则返回真
	// lt 如果arg1 < arg2则返回真
	// le 如果arg1 <= arg2则返回真
	// gt 如果arg1 > arg2则返回真
	// ge 如果arg1 >= arg2则返回真
	// 为了简化多参数相等检测，eq（只有eq）可以接受2个或更多个参数，它会将第一个参数和其余参数依次比较，返回下式的结果：
	// arg1==arg2 || arg1==arg3 || arg1==arg4 ... // （和go的||不一样，不做惰性运算，所有参数都会执行）
	// 比较函数只适用于基本类型（或重定义的基本类型，如"type Celsius float32"）。
	// 它们实现了go语言规则的值的比较，但具体的类型和大小会忽略掉，因此任意类型有符号整数值都可以互相比较；
	// 任意类型无符号整数值都可以互相比较；等等。但是，整数和浮点数不能互相比较。

	// 所有例子中的{{ }}内的操作我们将其称作pipelines
	// pipeline通常是将一个command序列分割开，再使用管道符'|'连接起来（但不使用管道符的command序列也可以视为一个管道），
	// 一个变量的作用域只到声明它的控制结构（"if"、"with"、"range"）的"end"为止，如果不是在控制结构里声明会直到模板结束为止。
	// 子模板的调用不会从调用它的位置（作用域）继承变量。
	// 如果没有定义变量的名字，而是只使用$,那么在模板开始执行时，$会设置为传递给Execute方法的参数，就是说，dot的初始值。
	// 在一个链式的pipeline里，每个command的结果都作为下一个command的最后一个参数。pipeline最后一个command的输出作为整个管道执行的结果。
	// command的输出可以是1到2个值，如果是2个后一个必须是error接口类型。如果error类型返回值非nil，模板执行会中止并将该错误返回给执行模板的调用者。

	// action（动作）的列表
	// {{/* a comment */}}
	// 	注释，执行时会忽略。可以多行。注释不能嵌套，并且必须紧贴分界符始止，就像这里表示的一样。
	// {{pipeline}}
	// 	pipeline的值的默认文本表示会被拷贝到输出里。
	// {{if pipeline}} T1 {{end}}
	// 	如果pipeline的值为empty，不产生输出，否则输出T1执行结果。不改变dot的值。
	// 	Empty值包括false、0、任意nil指针或者nil接口，任意长度为0的数组、切片、字典。
	// {{if pipeline}} T1 {{else}} T0 {{end}}
	// 	如果pipeline的值为empty，输出T0执行结果，否则输出T1执行结果。不改变dot的值。
	// {{if pipeline}} T1 {{else if pipeline}} T0 {{end}}
	// 	用于简化if-else链条，else action可以直接包含另一个if；等价于：
	// 		{{if pipeline}} T1 {{else}}{{if pipeline}} T0 {{end}}{{end}}
	// {{range pipeline}} T1 {{end}}
	// 	pipeline的值必须是数组、切片、字典或者通道。
	// 	如果pipeline的值其长度为0，不会有任何输出；
	// 	否则dot依次设为数组、切片、字典或者通道的每一个成员元素并执行T1；
	// 	如果pipeline的值为字典，且键可排序的基本类型，元素也会按键的顺序排序。
	// {{range pipeline}} T1 {{else}} T0 {{end}}
	// 	pipeline的值必须是数组、切片、字典或者通道。
	// 	如果pipeline的值其长度为0，不改变dot的值并执行T0；否则会修改dot并执行T1。
	// {{template "name"}}
	// 	执行名为name的模板，提供给模板的参数为nil，如模板不存在输出为""
	// {{template "name" pipeline}}
	// 	执行名为name的模板，提供给模板的参数为pipeline的值。
	// {{with pipeline}} T1 {{end}}
	// 	如果pipeline为empty不产生输出，否则将dot设为pipeline的值并执行T1。不修改外面的dot。
	// {{with pipeline}} T1 {{else}} T0 {{end}}
	// 	如果pipeline为empty，不改变dot并执行T0，否则dot设为pipeline的值并执行T1。

	// 条件判断-if
	// {{if pipeline}} T1 {{end}}
	// 	如果pipeline的值为empty，不产生输出，否则输出T1执行结果。不改变dot的值。
	// 	Empty值包括false、0、任意nil指针或者nil接口，任意长度为0的数组、切片、字典。
	// {{if pipeline}} T1 {{else}} T0 {{end}}
	// 	如果pipeline的值为empty，输出T0执行结果，否则输出T1执行结果。不改变dot的值。
	// {{if pipeline}} T1 {{else if pipeline}} T0 {{end}}
	// 	用于简化if-else链条，else action可以直接包含另一个if；等价于：
	// 		{{if pipeline}} T1 {{else}}{{if pipeline}} T0 {{end}}{{end}}
	// {{if not .condition}}
	// {{end}}
	// {{if and .condition1 .condition2}} //即如果condition1成立则返回condition2，否则返回condition1
	// {{end}}
	// {{if or .condition1 .condition2}}  //即如果condition1成立则返回condition1，否则返回condition2
	// {{end}}
	// {{if eq .var1 .var2}}
	// {{end}}
	// {{with pipeline}} T1 {{end}}
	// 	如果pipeline为empty不产生输出，否则将dot设为pipeline的值并执行T1。不修改外面的dot。
	// {{with pipeline}} T1 {{else}} T0 {{end}}
	// 	如果pipeline为empty，不改变dot并执行T0，否则dot设为pipeline的值并执行T1。

	// Must函数用于包装返回(*Template, error)的函数/方法调用，它会在err非nil时panic，一般用于变量初始化：
	// var t = template.Must(template.New("name").Parse("text"))

	// 在{{- else}}、{{- end}}和{{with .Gift -}}中的-表示消除{{else}}等会导致的空行
	// {{with .Gift}}表示如果Gift不为空的话，则打印下面的句子

	// 遍历-range
	// {{range pipeline}} T1 {{end}}
	// 	pipeline的值必须是数组、切片、字典或者通道。
	// 	如果pipeline的值其长度为0，不会有任何输出；
	// 	否则dot依次设为数组、切片、字典或者通道的每一个成员元素并执行T1；
	// 	如果pipeline的值为字典，且键可排序的基本类型，元素也会按键的顺序排序。
	// {{range pipeline}} T1 {{else}} T0 {{end}}
	// 	pipeline的值必须是数组、切片、字典或者通道。
	// 	如果pipeline的值其长度为0，即没有可遍历的值时，不改变dot的值并执行T0；否则会修改dot并执行T1。
	// {{range $i, $v := .Var}} //显示得到遍历的index和value
	// {{$i}} => {{$v}}
	// {{end}}
	// {{range .Var}} //没有显示去获取遍历得到的index和value，这时候要获得value值，使用{{.}}表示
	// {{.}}
	// {{end}}
	// {{range .slice}} //如果想要在range...end中访问非遍历得到的value，即外部的其他值，则在前面添加$来表示
	// {{$.OutsideContent}}
	// {{end}}
	rangeTemplate := `
{{if .Kind}}
{{range $i, $v := .MapContent}}
{{$i}} => {{$v}} , {{$.OutsideContent}}
{{end}}
{{else}}
{{range .MapContent}}
{{.}} , {{$.OutsideContent}}
{{end}}    
{{end}}`

	str1 := []string{"this is the first range", "use its index and value"}
	str2 := []string{"this is the second range", "do not use its index and value"}

	type Content struct {
		MapContent     []string
		OutsideContent string
		Kind           bool
	}
	var contents = []Content{
		{str1, "this is the first outside content", true},
		{str2, "this is the second outside content", false},
	}

	// Create a new template and parse the letter into it.
	t := template.Must(template.New("range").Parse(rangeTemplate))

	// Execute the template for each recipient.
	for _, c := range contents {
		err := t.Execute(os.Stdout, c)
		if err != nil {
			log.Println("executing template:", err)
		}
	}
	// 模版回车,上面的空行与模版中的回车有关，如果想要没有输出的空行，上面的模版应该写成：
	// rangeTemplate := `{{if .Kind}}{{range $i, $v := .MapContent}}
	// {{$i}} => {{$v}} , {{$.OutsideContent}}
	// {{end}}
	// {{else}}{{range .MapContent}}
	// {{.}} , {{$.OutsideContent}}
	// {{end}}
	// {{end}}`

	// 模版嵌套
	// {{template "name"}}
	//     执行名为name的模板，提供给模板的参数为nil，如模板不存在输出为""。当然首先要使用{{define "name"}}{{end}}定义好该模版
	// {{template "name" pipeline}}
	//     执行名为name的模板，提供给模板的参数为pipeline的值。将管道的值赋给子模板中的"."（即"{{.}}"），即{{template "name" .}}
	// templateContent := `{{define "T1"}}ONE{{end}}{{define "T2"}}TWO{{end}}{{define "T3"}}{{template "T1"}} {{template "T2"}}{{end}}{{template "T3"}}`
	// t := template.Must(template.New("template").Parse(templateContent))
	// template1 := "ONE"
	// template2 := "TWO"
	// template3 := `{{template "T1"}} {{template "T2"}}`
	// // Create a new template and parse the letter into it.
	// t := template.Must(template.New("T1").Parse(template1))
	// t = template.Must(t.New("T2").Parse(template2))
	// t = template.Must(t.New("T3").Parse(template3))

	// 多模版
	// New方法创建一个和t关联的名字为name的模板并返回它。这种可以传递的关联允许一个模板使用template action调用另一个模板。
	sweaters1 := Inventory{"wool", 17}
	template1 := "{{.Count}} of {{.Material}}\n"
	template2 := "{{.Material}} of {{.Count}}\n"

	tmpla := template.Must(template.New("T1").Parse(template1))
	fmt.Println(tmpl.Name()) //T1
	tmpla = template.Must(tmpla.New("T2").Parse(template2))
	fmt.Println(tmpl.Name()) //T2

	err1 := tmpla.ExecuteTemplate(os.Stdout, "T1", sweaters1) //返回 17 of wool
	if err != nil {
		panic(err1)
	}
	err1 = tmpla.ExecuteTemplate(os.Stdout, "T2", sweaters1) //返回 wool of 17
	if err != nil {
		panic(err1)
	}

	tmpla = tmpla.Lookup("T1")
	fmt.Println(tmpla.Name()) //T1

	mapTemplate := tmpla.Templates()
	for _, v := range mapTemplate { //先得到T2，再得到T1
		fmt.Println(v.Name())
	}

	// 可以将你的模版内容写到文件当中，然后再从文件中调用
	// ParseFiles函数创建一个模板并解析filenames指定的文件里的模板定义。返回的模板的名字是第一个文件的文件名（不含扩展名），
	// 内容为解析后的第一个文件的内容。至少要提供一个文件。如果发生错误，会停止解析并返回nil。
	// ParseGlob创建一个模板并解析匹配pattern的文件（参见glob规则）里的模板定义。返回的模板的名字是第一个匹配的文件的文件名（不含扩展名），
	// 内容为解析后的第一个文件的内容。至少要存在一个匹配的文件。如果发生错误，会停止解析并返回nil。ParseGlob等价于使用匹配pattern的文件的列表
	// 为参数调用ParseFiles。
	// ParseFiles接受一个字符串，字符串的内容是一个模板文件的路径（绝对路径or相对路径）
	// ParseGlob是用正则的方式匹配多个文件,如当你要选取某文件夹下的所有txt模版文件时，就可以调用ParseGlob("*.txt")

	// Clone方法返回模板的一个副本，包括所有相关联的模板。模板的底层表示树并未拷贝，而是拷贝了命名空间，因此拷贝调用Parse方法不会修改原模板的命名空间。
	// Clone方法用于准备模板的公用部分，向拷贝中加入其他关联模板后再进行使用。

	// Delims方法用于设置action的分界字符串，应用于之后的Parse、ParseFiles、ParseGlob方法。嵌套模板定义会继承这种分界符设置。
	// 空字符串分界符表示相应的默认分界符：{{或}}。返回值就是t，以便进行链式调用。默认的分界符即left = {{ , right = }}

	// func HTMLEscape(w io.Writer, b []byte) 函数向w中写入b的HTML转义等价表示。
	// func HTMLEscapeString(s string) string 返回s的HTML转义等价表示字符串。
	// func HTMLEscaper(args ...interface{}) string 函数返回其所有参数文本表示的HTML转义等价表示字符串。
	// func JSEscape(w io.Writer, b []byte) 函数向w中写入b的JavaScript转义等价表示。
	// func JSEscapeString(s string) string 返回s的JavaScript转义等价表示字符串。
	// func JSEscaper(args ...interface{}) string 函数返回其所有参数文本表示的JavaScript转义等价表示字符串。
	// func URLQueryEscaper(args ...interface{}) string 函数返回其所有参数文本表示的可以嵌入URL查询的转义等价表示字符串。
}

type Inventory struct {
	Material string
	Count    uint
}

type MyMethod struct {
	Say  string
	Name string
}

func (my *MyMethod) SayHello() string { //没参数
	return "world"
}

func (my *MyMethod) SayYouName(name string) string { //有参数
	return "my name is : " + name
}

func SayHello1() string { //没参数
	return "hello world"
}

func SayYouName1(name string) string { //有参数
	return "my name is : " + name
}
