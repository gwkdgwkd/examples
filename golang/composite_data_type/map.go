package main

import "fmt"

func main() {
	// 哈希表是一种巧妙并且实用的数据结构。它是一个无序的key/value对的集合,其中所有的key
	// 都是不同的,然后通过给定的key可以在常数时间复杂度内检索、更新或删除对应的value。

	// 在Go语言中,一个map就是一个哈希表的引用,map类型可以写为map[K]V,其中K和V分别
	// 对应key和value。map中所有的key都有相同的类型,所有的value也有着相同的类型,但是
	// key和value之间可以是不同的数据类型。其中K对应的key必须是支持==比较运算符的数据类
	// 型,所以map可以通过测试key是否相等来判断是否已经存在。虽然浮点数类型也是支持相等
	// 运算符比较的,但是将浮点数用做key类型则是一个坏的想法,最坏的情
	// 况是可能出现的NaN和任何浮点数都不相等。对于V对应的value数据类型则没有任何的限制。

	// 内置的make函数可以创建一个map:
	ages1 := make(map[string]int)
	fmt.Printf("%v\n", ages1) // map[]
	ages1["alice"]	=	31
	ages1["charlie"]	=	34
	fmt.Printf("%v\n", ages1) // map[alice:31 charlie:34]

	// 也可以用map字面值的语法创建map,同时还可以指定一些最初的key/value:
	ages2 := map[string]int{
				"alice": 31,
				"charlie":	34,
	}
	fmt.Printf("%v\n", ages2) // map[charlie:34 alice:31]

	// 一种创建空的map的表达式是map[string]int{}。
	ages3 := map[string]int{}
	fmt.Printf("%v\n", ages3) // map[]

	// 使用内置的delete函数可以删除元素:
	delete(ages1,"alice")
	fmt.Printf("%v\n", ages1) // map[charlie:34]

	// 所有这些操作是安全的,即使这些元素不在map中也没有关系;如果一个查找失败将返回
	// value类型对应的零值,例如,即使map中不存在“bob”下面的代码也可以正常工作,因为
	// ages["bob"]失败时将返回0。
	ages1["bob"] = ages1["bob"] + 1
	fmt.Printf("%v\n", ages1) // ages["bob"] = ages["bob"] + 1
	// 而且x+=y和x++等简短赋值语法也可以用在map上,所以上面的代码可以改写成
	// ages["bob"] += 1
	// 更简单的写法
	// ages["bob"]++

	// map中的元素并不是一个变量,因此不能对map的元素进行取址操作:
	// _ = &ages["bob"] // compile error: cannot take address of map element
	// 禁止对map元素取址的原因是map可能随着元素数量的增长而重新分配更大的内存空间,从而
	// 可能导致之前的地址无效。

	// 遍历map中全部的key/value对,可以使用range风格的for循环实现,和slice遍历语法类似。

	// Map的迭代顺序是不确定的,并且不同的哈希函数实现可能导致不同的遍历顺序。在实践
	// 中,遍历的顺序是随机的,每一次遍历的顺序都不相同。这是故意的,每次都使用随机的遍
	// 历顺序可以强制要求程序不会依赖具体的哈希函数实现。如果要按顺序遍历key/value对,我
	// 们必须显式地对key进行排序,可以使用sort包的Strings函数对字符串slice进行排序。

	// map类型的零值是nil,也就是没有引用任何哈希表。
	var	ages map[string]int
	fmt.Println(ages == nil) // true
	fmt.Println(len(ages) == 0) // true

	// map上的大部分操作,包括查找、删除、len和range循环都可以安全工作在nil值的map上,它
	// 们的行为和一个空的map类似。但是向一个nil值的map存入元素将导致一个panic异常:
	// ages["carol"] = 21 // panic: assignment to entry in nil map
	// 在向map存数据前必须先创建map。

	// 通过key作为索引下标来访问map将产生一个value。如果key在map中是存在的,那么将得到
	// 与key对应的value;如果key不存在,那么将得到value对应类型的零值,正如我们前面看到的
	// ages["bob"]那样。这个规则很实用,但是有时候可能需要知道对应的元素是否真的是在map
	// 之中。例如,如果元素类型是一个数字,你可以需要区分一个已经存在的0,和不存在而返回
	// 零值的0,可以像下面这样测试:
	// age, ok	:=	ages["bob"]
	// if	!ok	{	/*	"bob"	is	not	a	key	in	this	map;	age	==	0.	*/	}
	// 结合起来使用:
	// if	age,	ok	:=	ages["bob"];	!ok	{	/*	...	*/	}
	// map的下标语法将产生两个值;第二个是一个布尔值,用于报告元素是否真的存在。布尔变量一般命名为ok。

	// 和slice一样,map之间也不能进行相等比较;唯一的例外是和nil进行比较。要判断两个map是
	// 否包含相同的key和value,必须通过一个循环实现。

	// Go语言中并没有提供一个set类型,但是map中的key也是不相同的,可以用map实现类似set的功能。
	// Go程序员将这种忽略value的map当作一个字符串集合。

	// 有时候需要一个map或set的key是slice类型,但是map的key必须是可比较的类型,但是
	// slice并不满足这个条件。不过,可以通过两个步骤绕过这个限制。第一步,定义一个辅
	// 助函数k,将slice转为map对应的string类型的key,确保只有x和y相等时k(x)==k(y)才成立。
	// 然后创建一个key为string类型的map,在每次对map操作时先用k辅助函数将slice转化为string类型。
	// 使用同样的技术可以处理任何不可比较的key类型,而不仅仅是slice类型。这种技术对于想使
	// 用自定义key比较函数的时候也很有用,例如在比较字符串的时候忽略大小写。同时,辅助函
	// 数k(x)也不一定是字符串类型,它可以返回任何可比较的类型,例如整数、数组或结构体等。

	// Map的value类型也可以是一个聚合类型,比如是一个map或slice。	
	
}

// 判断两个map是否相等
func	equal(x,	y	map[string]int)	bool	{
				if	len(x)	!=	len(y)	{
								return	false
				}
				// 不能简单地用xv!=y[k]判断,下面的map判断会出错
				// equal(map[string]int{"A":0}, map[string]int{"B":42})
				for	k,	xv	:=	range	x	{
								if	yv,	ok	:=	y[k];	!ok	||	yv	!=	xv	{
												return	false
								}
				}
				return	true
}

// 使用map来记录提交相同的字符串列表的次数
var	m	=	make(map[string]int)
func	k(list	[]string)	string	{	return	fmt.Sprintf("%q",	list)	}
func	Add(list	[]string)							{	m[k(list)]++	}
func	Count(list	[]string)	int	{	return	m[k(list)]	}