package main

import (
	"fmt"
	"sort"
)

// sort包内置的提供了根据一些排序函数来对任何序列排序的功能。它的设计非常独
// 到。在很多语言中,排序算法都是和序列数据类型关联,同时排序函数和具体类型元素关
// 联。相比之下,Go语言的sort.Sort函数不会对具体的序列和它的元素做任何假设。相反,它
// 使用了一个接口类型sort.Interface来指定通用的排序算法和可能被排序到的序列类型之间的约
// 定。这个接口的实现由序列的具体表示和它希望排序的元素决定,序列的表示经常是一个切片。

// 一个内置的排序算法需要知道三个东西:序列的长度,表示两个元素比较的结果,一种交换
// 两个元素的方式;这就是sort.Interface的三个方法:
// type Interface interface {
// 	Len() int
// 	Less(i, j int) bool
// 	Swap(i, j int)
// }
// type reverse struct{ Interface }       //that is, sort.Interface
// func (r reverse) Less(i, j int) bool   { return r.Interface.Less(j, i) }
// func Reverse(data Interface) Interface { return reverse{data} }

type StringSlice []string

func (p StringSlice) Len() int           { return len(p) }
func (p StringSlice) Less(i, j int) bool { return p[i] < p[j] }
func (p StringSlice) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }

func main() {
	names1 := []string{"A", "D", "CB", "B", "CA"}
	fmt.Println(names1) // [A D CB B CA]
	sort.Sort(StringSlice(names1))
	fmt.Println(names1) // [A B CA CB D]

	// sort包定义了一个不公开的struct类型reverse,它嵌入了一个sort.Interface。reverse的
	// Less方法调用了内嵌的sort.Interface值的Less方法,但是通过交换索引的方式使排序结果变成逆序。
	// reverse的另外两个方法Len和Swap隐式地由原有内嵌的sort.Interface提供。因为reverse是一
	// 个不公开的类型,所以导出函数Reverse函数返回一个包含原有sort.Interface值的reverse类型实例。
	sort.Sort(sort.Reverse(StringSlice(names1)))
	fmt.Println(names1) // [D CB CA B A]

	// 对字符串切片的排序是很常用的需要,所以sort包提供了StringSlice类型,也提供了Strings函
	// 数能让上面这些调用简化成
	names2 := []string{"AD", "A", "D", "CB", "B", "CA"}
	fmt.Println(names2) // [AD A D CB B CA]
	sort.Strings(names2)
	fmt.Println(names2) // [A AD B CA CB D]

	// 为了使用方便,sort包为[]int,[]string和[]float64的正常排序提供了特定版本的函数和类型。对
	// 于其他类型,例如[]int64或者[]uint,尽管路径也很简单,还是依赖我们自己实现。
}
