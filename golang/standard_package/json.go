package main

import (
	"encoding/json"
	"fmt"
	"log"
)

// JavaScript对象表示法(JSON)是一种用于发送和接收结构化信息的标准协议。在类似的协
// 议中,JSON并不是唯一的一个标准协议。	XML、ASN.1和Google的Protocol
// Buffers都是类似的协议,并且有各自的特色,但是由于简洁性、可读性和流行程度等原因,
// JSON是应用最广泛的一个。
// Go语言对于这些标准格式的编码和解码都有良好的支持,由标准库中的encoding/json、
// encoding/xml、encoding/asn1等包提供支持(Protocol Buffers的支持由
// github.com/golang/protobuf包提供),并且这类包都有着相似的API接口。

type Movie struct {
	Title  string
	Year   int  `json:"released"`
	Color  bool `json:"color,omitempty"`
	Actors []string
}

func main() {
	// 基本的JSON类型有数字(十进制或科学记数法)、布尔值(true或false)、字符串,其中字
	// 符串是以双引号包含的Unicode字符序列,支持和Go语言类似的反斜杠转义特性,不过JSON
	// 使用的是\Uhhhh转义数字来表示一个UTF-16编码(UTF-16和UTF-8一样是一种变长
	// 的编码,有些Unicode码点较大的字符需要用4个字节表示;而且UTF-16还有大端和小端的问
	// 题),而不是Go语言的rune类型。
	// 这些基础类型可以通过JSON的数组和对象类型进行递归组合。一个JSON数组是一个有序的
	// 值序列,写在一个方括号中并以逗号分隔;一个JSON数组可以用于编码Go语言的数组和
	// slice。一个JSON对象是一个字符串到值的映射,写成以系列的name:value对形式,用花括号
	// 包含并以逗号分隔;JSON的对象类型可以用于编码Go语言的map类型(key类型是字符串)
	// 和结构体。

	var movies = []Movie{
		{Title: "Casablanca", Year: 1942, Color: false,
			Actors: []string{"Humphrey Bogart", "Ingrid Bergman"}},
		{Title: "Cool Hand Luke", Year: 1967, Color: true,
			Actors: []string{"Paul Newman"}},
		{Title: "Bullitt", Year: 1968, Color: true,
			Actors: []string{"Steve McQueen", "Jacqueline Bisset"}}}
	fmt.Println(movies) // [{Casablanca 1942 false [Humphrey Bogart Ingrid Bergman]} {Cool Hand Luke 1967 true [Paul Newman]} {Bullitt 1968 true [Steve McQueen Jacqueline Bisset]}]

	// 将一个Go语言中结构体slice转为JSON的过程叫编组(marshaling)。编组通过调用json.Marshal函数完成:
	data, err := json.Marshal(movies)
	if err != nil {
		log.Fatalf("JSON marshaling failed: %s", err)
	}
	fmt.Printf("%s\n", data)
	// [{"Title":"Casablanca","released":1942,"Actors":["Humphrey Bogart","Ingrid Bergman"]},
	// {"Title":"Cool Hand Luke","released":1967,"color":true,"Actors":["Paul Newman"]},
	// {"Title":"Bullitt","released":1968,"color":true,"Actors":["Steve McQueen","Jacqueline Bisset"]}]

	// 为了生成便于阅读的格式,另一个json.MarshalIndent函数将产生整齐缩进的输出。
	// 该函数有两个额外的字符串参数用于表示每一行输出的前缀和每一个层级的缩进:
	data1, err := json.MarshalIndent(movies, " ", "	")
	if err != nil {
		log.Fatalf("JSON marshaling failed: %s", err)
	}
	fmt.Printf("%s\n", data1)
	// [
	//   {
	//    "Title": "Casablanca",
	//    "released": 1942,
	//    "Actors": [
	//     "Humphrey Bogart",
	//     "Ingrid Bergman"
	//    ]
	//   },
	//   {
	//    "Title": "Cool Hand Luke",
	//    "released": 1967,
	//    "color": true,
	//    "Actors": [
	//     "Paul Newman"
	//    ]
	//   },
	//   {
	//    "Title": "Bullitt",
	//    "released": 1968,
	//    "color": true,
	//    "Actors": [
	//     "Steve McQueen",
	//     "Jacqueline Bisset"
	//    ]
	//   }
	//  ]

	// 在编码时,默认使用Go语言结构体的成员名字作为JSON的对象(通过reflect反射技术)。
	// 只有导出的结构体成员才会被编码,这也就是我们为什么选择用大写字母开头的成员名称。

	// 一个构体成员Tag是和在编译阶段关联到该成员的元信息字符串。
	// 结构体的成员Tag可以是任意的字符串面值,但是通常是一系列用空格分隔的key:"value"键值
	// 对序列;因为值中含义双引号字符,因此成员Tag一般用原生字符串面值的形式书写。json开
	// 头键名对应的值用于控制encoding/json包的编码和解码的行为,并且encoding/...下面其它的
	// 包也遵循这个约定。
	// Year int `json:"released"`
	// Color bool `json:"color,omitempty"`

	// 编码的逆操作是解码,对应将JSON数据解码为Go语言的数据结构,Go语言中一般叫
	// unmarshaling,通过json.Unmarshal函数完成。通过定义合适的Go语言数据结构,可以选择性
	// 地解码JSON中感兴趣的成员。当Unmarshal函数调用返回,slice将被只含有Title信息值填
	// 充,其它JSON成员将被忽略。
	var titles []struct{ Title string }
	if err := json.Unmarshal(data, &titles); err != nil {
		log.Fatalf("JSON unmarshaling failed: %s", err)
	}
	fmt.Println(titles) // [{Casablanca} {Cool Hand Luke} {Bullitt}]
}
