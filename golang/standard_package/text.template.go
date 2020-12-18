package main

import (
	"html/template"
	"os"
)

// text/template和html/template等模板包提供了一个将变量值填充到一个文本或HTML格式的模板的机制。

// 一个模板是一个字符串或一个文件,里面包含了一个或多个由双花括号包含的 {{action}}对
// 象。大部分的字符串只是按面值打印,但是对于actions部分将触发其它的行为。每个actions
// 都包含了一个用模板语言书写的表达式,一个action虽然简短但是可以输出复杂的打印值,模
// 板语言包含通过选择结构体的成员、调用函数或方法、表达式控制流if-else语句和range循环
// 语句,还有其它实例化模板等诸多特性。
func main() {
	sweaters := Inventory{"wool", 17}
	tmpl, err := template.New("test").Parse("{{.Count}} of {{.Material}}\n") //{{.Count}}获取的是struct对象中的Count字段的值
	if err != nil {
		panic(err)
	}
	err = tmpl.Execute(os.Stdout, sweaters) // 17 of wool
	if err != nil {
		panic(err)
	}
}

type Inventory struct {
	Material string
	Count    uint
}
