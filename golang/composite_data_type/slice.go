package main

import "fmt"

func main() {
	// Slice(切片)代表变长的序列,序列中每个元素都有相同的类型。一个slice类型一般写作
	// []T,其中T代表slice中元素的类型;slice的语法和数组很像,只是没有固定长度而已。

	// 数组和slice之间有着紧密的联系。一个slice是一个轻量级的数据结构,提供了访问数组子序
	// 列(或者全部)元素的功能,而且slice的底层确实引用一个数组对象。一个slice由三个部分
	// 构成:指针、长度和容量。指针指向第一个slice元素对应的底层数组元素的地址,要注意的
	// 是slice的第一个元素并不一定就是数组的第一个元素。长度对应slice中元素的数目;长度不
	// 能超过容量,容量一般是从slice的开始位置到底层数据的结尾位置。内置的len和cap函数分
	// 别返回slice的长度和容量。

	// 多个slice之间可以共享底层的数据,并且引用的数组部分区间可能重叠。

	// 数组的第一个元素从索引0开始,但是月份一般是从1开始的,因此我们声明数组时直接跳过第0个元素,
	// 第0个元素会被自动初始化为空字符串。
	months := [...]string{1: "January", 2: "February", 3: "March", 4: "April", 5: "May",
		6: "June", 7: "July", 8: "August", 9: "September", 10: "October", 11: "November", 12: "December"}
	// slice的切片操作s[i:j],其中0	≤	i≤	j≤	cap(s),用于创建一个新的slice,引用s的从第i个元素开
	// 始到第j-1个元素的子序列。新的slice将只有j-i个元素。如果i位置的索引被省略的话将使用0代
	// 替,如果j位置的索引被省略的话将使用len(s)代替。因此,months[1:13]切片操作将引用全部
	// 有效的月份,和months[1:]操作等价;months[:]切片操作则是引用整个数组。
	Q2 := months[4:7]
	summer := months[6:9]
	fmt.Println(Q2)     // [April May June]
	fmt.Println(summer) // [June July August]

	// 字符串的切片操作和[]byte字节类型切片的切片操作是类似的。都写作x[m:n],并且都
	// 是返回一个原始字节系列的子序列,底层都是共享之前的底层数组,因此这种操作都是常量
	// 时间复杂度。x[m:n]切片操作对于字符串则生成一个新字符串,如果x是[]byte的话则生成一个
	// 新的[]byte。

	// 因为slice值包含指向第一个slice元素的指针,因此向函数传递slice将允许在函数内部修改底
	// 层数组的元素。换句话说,复制一个slice只是对底层的数组创建了一个新的slice别名。
	a := [...]int{0, 1, 2, 3, 4, 5}
	reverse(a[:])
	fmt.Println(a) // [5 4 3 2 1 0]

	// 一种将slice元素循环向左旋转n个元素的方法是三次调用reverse反转函数,第一次是反转开头
	// 的n个元素,然后是反转剩下的元素,最后是反转整个slice的元素。(如果是向右循环旋转,
	// 则将第三个函数调用移到第一个调用位置就可以了。)
	s := []int{0, 1, 2, 3, 4, 5}
	reverse(s[:2])
	reverse(s[2:])
	reverse(s)
	fmt.Println(s) // [2 3 4 5 0 1]

	// slice类型的变量s和数组类型的变量a的初始化语法的差异。slice和数组的字面值
	// 语法很类似,它们都是用花括弧包含一系列的初始化元素,但是对于slice并没有指明序列的
	// 长度。这会隐式地创建一个合适大小的数组,然后slice的指针指向底层的数组。就像数组字
	// 面值一样,slice的字面值也可以按顺序指定初始化值序列,或者是通过索引和元素值指定,
	// 或者的两种风格的混合语法初始化。

	// 和数组不同的是,slice之间不能比较,因此我们不能使用==操作符来判断两个slice是否含有
	// 全部相等元素。不过标准库提供了高度优化的bytes.Equal函数来判断两个字节型slice是否相
	// 等([]byte),但是对于其他类型的slice,我们必须自己展开每个元素进行比较。

	// 为何slice不直接支持比较运算符呢?这方面有两个原因:
	// 第一个原因,一个slice的元素是间接引用的,一个slice甚至可以包含自身。虽然有很多办
	// 法处理这种情形,但是没有一个是简单有效的。
	// 第二个原因,因为slice的元素是间接引用的,一个固定的slice值(指slice本身的值,不
	// 是元素的值)在不同的时刻可能包含不同的元素,因为底层数组的元素可能会被修改。而例如
	// Go语言中map的key只做简单的浅拷贝,它要求key在整个生命周期内保持不变性(slice扩容,
	// 就会导致其本身的值/地址变化)。而用深度相等判断的话,显然在map的key这种场合不合适。
	// 对于像指针或chan之类的引用类型,==相等测试可以判断两个是否是引用相同的对象。一个针
	// 对slice的浅相等测试的==操作符可能是有一定用处的,也能临时解决map类型的key问题,但
	// 是slice和数组不同的相等测试行为会让人困惑。因此,安全的做法是禁止slice之间的比较操作。

	// slice唯一合法的比较操作是和nil比较:
	if summer == nil { /*	...	*/
	}

	// 一个零值的slice等于nil。一个nil值的slice并没有底层数组。一个nil值的slice的长度和容量都
	// 是0,但是也有非nil值的slice的长度和容量也是0的,例如[]int{}或make([]int,3)[3:]。与任意类
	// 型的nil值一样,我们可以用[]int(nil)类型转换表达式来生成一个对应类型slice的nil值。
	var s1 []int                //	len(s) == 0, s == nil
	var s2 []int = nil          //	len(s) == 0, s == nil
	s3 := []int(nil)            //	len(s) == 0, s == nil
	s4 := []int{}               //	len(s) == 0, s != nil
	fmt.Println(s1, s2, s3, s4) // [] [] [] []

	// 如果你需要测试一个slice是否是空的,使用len(s)==0来判断,而不应该用s==nil来判断。
	// 除了和nil相等比较外,一个nil值的slice的行为和其它任意0长度的slice一样;例如reverse(nil)
	// 也是安全的。除了文档已经明确说明的地方,所有的Go语言函数应该以相同的方式对待nil值
	// 的slice和0长度的slice。

	// 内置的make函数创建一个指定元素类型、长度和容量的slice。容量部分可以省略,在这种情
	// 况下,容量将等于长度。
	// make([]T, len)
	// make([]T, len, cap) // same as make([]T,cap)[:len]
	// 在底层,make创建了一个匿名的数组变量,然后返回一个slice;只有通过返回的slice才能引
	// 用底层匿名的数组变量。在第一种语句中,slice是整个数组的view。在第二个语句中,slice
	// 只引用了底层数组的前len个元素,但是容量将包含整个的数组。额外的元素是留给未来的增
	// 长用的。

	// 内置的append函数用于向slice追加元素:
	var runes []rune
	for _, r := range "Hello,	世界" {
		runes = append(runes, r)
	}
	fmt.Printf("%q\n", runes) // ['H' 'e' 'l' 'l' 'o' ',' '\t' '世' '界']

	// 每次调用appendInt函数,必须先检测slice底层数组是否有足够的容量来保存新添加的元素。
	// 如果有足够空间的话,直接扩展slice(依然在原有的底层数组之上),将新添加的y元素复制
	// 到新扩展的空间,并返回slice。因此,输入的x和输出的z共享相同的底层数组。
	// 如果没有足够的增长空间的话,appendInt函数则会先分配一个足够大的slice用于保存新的结
	// 果,先将输入的x复制到新的空间,然后添加y元素。结果z和输入的x引用的将是不同的底层
	// 数组。

	// copy函数的第一个参数是要复制的目标slice,第二个参数是源slice,目标和源
	// 的位置顺序和dst=src	赋值语句是一致的。两个slice可以共享同一个底层数组,甚至有重
	// 叠也没有问题。copy函数将返回成功复制的元素的个数,等于两个slice中较小的长度,所以
	// 我们不用担心覆盖会超出目标slice的范围。

	// 为了提高内存使用效率,新分配的数组一般略大于所需要的最低大小。通过在每次
	// 扩展数组时直接将长度翻倍从而避免了多次内存分配,也确保了添加单个元素操的平均时间
	// 是一个常数时间。
	var x, y []int
	for i := 0; i < 10; i++ {
		y = append(x, i)
		fmt.Printf("%d	cap=%d\t%v\n", i, cap(y), y)
		x = y
	}
	// 0       cap=1   [0]
	// 1       cap=2   [0 1]
	// 2       cap=4   [0 1 2]
	// 3       cap=4   [0 1 2 3]
	// 4       cap=8   [0 1 2 3 4]
	// 5       cap=8   [0 1 2 3 4 5]
	// 6       cap=8   [0 1 2 3 4 5 6]
	// 7       cap=8   [0 1 2 3 4 5 6 7]
	// 8       cap=16  [0 1 2 3 4 5 6 7 8]
	// 9       cap=16  [0 1 2 3 4 5 6 7 8 9]

	// append函数则可以追加多个元素,甚至追加一个slice。
	var x1 []int
	x1 = append(x1, 1)
	x1 = append(x1, 2, 3)
	x1 = append(x1, 4, 5, 6)
	x1 = append(x1, x1...) // append the slice x
	fmt.Println(x1)        // [1 2 3 4 5 6 1 2 3 4 5 6]

	data := []string{"one", "", "three"}
	fmt.Printf("%q\n", nonempty(data)) // ["one" "three"]
	fmt.Printf("%q\n", data)           // ["one" "three" "three"]
}

// 在原内存空间将[]int类型的slice反转,而且它可以用于任意长度的slice。
func reverse(s []int) {
	for i, j := 0, len(s)-1; i < j; i, j = i+1, j-1 {
		s[i], s[j] = s[j], s[i]
	}
}

// 在原有slice内存空间之上返回不包含空字符串的列表。 输入的slice和输出
// 的slice共享一个底层数组。这可以避免分配另一个数组,不过原来的数据将可能会被覆盖。
// 一般这样使用：data = nonempty(data)
func nonempty(strings []string) []string {
	i := 0
	for _, s := range strings {
		if s != "" {
			strings[i] = s
			i++
		}
	}
	return strings[:i]
}

// 使用append实现
func nonempty2(strings []string) []string {
	out := strings[:0] // zero-length slice	of original
	for _, s := range strings {
		if s != "" {
			out = append(out, s)
		}
	}
	return out
}
