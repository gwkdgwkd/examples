#include <stdio.h>

// C语言概念少，词汇少，包含了基本的编程元素，后来的很多语言（C++、Java等）都参考了C语言，说C语言是现代编程语言的开山鼻祖毫不夸张，它改变了编程世界。
// 在世界编程语言排行榜中，C语言、Java和C++霸占了前三名，拥有绝对优势。
// 编程语言的发展大概经历了以下几个阶段：汇编语言-->面向过程编程-->面向对象编程
//  汇编语言是编程语言的拓荒年代，它非常底层，直接和计算机硬件打交道，开发效率低，学习成本高；
//  C语言是面向过程的编程语言，已经脱离了计算机硬件，可以设计中等规模的程序了；
//  Java、C++、Python、C#、PHP等是面向对象的编程语言，它们在面向过程的基础上又增加了很多概念。
// C语言被誉为“上帝语言”，它不但奠定了软件产业的基础，还创造了很多其它语言，例如：
//  PHP、Python等都是用C语言开发出来的，虽然平时做项目的时候看不到C语言的影子，但是如果想深入学习PHP和Python，那就要有C语言基础了。
//  C++和Objective-C干脆在C语言的基础上直接进行扩展，增加一些新功能后变成了新的语言，所以学习C++和Objective-C之前也要先学习C语言。
// C语言是有史以来最为重要的编程语言：要进入编程行业高手级别必学C语言，要挣大钱必学C语言，要做黑客、红客必学C语言，要面试名企、外企、高薪职位必学C语言。

// 从C语言到内存，从内存到进程和线程，环环相扣：不学C语言就吃不透内存，不学内存就吃不透进程和线程。
// 「内存+进程+线程」这几个最基本的计算机概念是菜鸟和大神的分水岭，也只有学习C语言才能透彻地理解它们。Java、C#、PHP、Python、JavaScript程序员工作几
// 年后会遇到瓶颈，有很多人会回来学习C语言，重拾底层概念，让自己再次突破。

// C语言是1972年由美国贝尔实验室研制成功的，在当时算是高级语言，它的很多新特性都让汇编程序员羡慕不已，就像今天的Go语言，刚出生就受到追捧。
// C语言也是”时髦“的语言，后来的很多软件都用C语言开发，包括Windows、Linux等。
// C++主要在C语言的基础上增加了面向对象和泛型的机制，提高了开发效率，以适用于大中型软件的编写。
// C、C++代码都会通过一个编译器来编译，很难说C++是一门独立的语言，还是对C的扩展。
// 从“学院派”的角度来说，C++支持面向过程编程、面向对象编程和泛型编程，而C语言仅支持面向过程编程。就面向过程编程而言，C++和C几乎是一样的，所以学习了C语
// 言，也就学习了C++的一半，不需要从头再来。

// 内存条是一个非常精密的部件，包含了上亿个电子元器件，它们很小，达到了纳米级别。这些元器件，实际上就是电路；电路的电压会变化，要么是0V，要么是5V，只有
// 这两种电压。5V是通电，用1来表示，0V是断电，用0来表示。所以，一个元器件有2种状态，0或者1。
// 8个元器件有28=256种不同的组合，16个元器件有216=65536种不同的组合。虽然一个元器件只能表示2个数值，但是多个结合起来就可以表示很多数值了。
// 一般情况下我们不一个一个的使用元器件，而是将8个元器件看做一个单位，即使表示很小的数，例如1，也需要8个，也就是 00000001。
// 1个元器件称为1比特（Bit）或1位，8个元器件称为1字节（Byte），那么16个元器件就是2Byte，32个就是4Byte，以此类推：
//  8×1024个元器件就是1024Byte，简写为1KB；
//  8×1024×1024个元器件就是1024KB，简写为1MB；
//  8×1024×1024×1024个元器件就是1024MB，简写为1GB。
// 单位换算：
//  1Byte = 8 Bit
//  1KB = 1024Byte = 210Byte
//  1MB = 1024KB = 220Byte
//  1GB = 1024MB = 230Byte
//  1TB = 1024GB = 240Byte
//  1PB = 1024TB = 250Byte
//  1EB = 1024PB = 260Byte
// 通常只会设计到KB、MB、GB、TB这几个单位，PB和EB这两个高级单位一般在大数据处理过程中才会用到。

// QQ是怎么运行起来的呢？
// 首先，有一点你要明确，你安装的QQ软件是保存在硬盘中的。
// 双击QQ图标，操作系统就会知道你要运行这个软件，它会在硬盘中找到你安装的QQ软件，将数据（安装的软件本质上就是很多数据的集合）复制到内存。
// 对！就是复制到内存！QQ不是在硬盘中运行的，而是在内存中运行的。为什么呢？因为内存的读写速度比硬盘快很多。
// 对于读写速度，内存>固态硬盘>机械硬盘。机械硬盘是靠电机带动盘片转动来读写数据的，而内存条通过电路来读写数据，电机的转速肯定没有电的传输速度（几乎是光速）快。
// 虽然固态硬盘也是通过电路来读写数据，但是因为与内存的控制方式不一样，速度也不及内存。
// 所以，不管是运行QQ还是编辑Word文档，都是先将硬盘上的数据复制到内存，才能让CPU来处理，这个过程就叫作载入内存（Load into Memory）。完成这个过程需要一个特
// 殊的程序（软件），这个程序就叫做加载器（Loader）。
// CPU直接与内存打交道，它会读取内存中的数据进行处理，并将结果保存到内存。如果需要保存到硬盘，才会将内存中的数据复制到硬盘。
// 如果我们运行的程序较多，占用的空间就会超过内存（内存条）容量。例如计算机的内存容量为2G，却运行着10个程序，这10个程序共占用3G的空间，也就意味着需要从硬盘复
// 制3G的数据到内存，这显然是不可能的。
// 操作系统（Operating System，简称OS）为我们解决了这个问题：当程序运行需要的空间大于内存容量时，会将内存中暂时不用的数据再写回硬盘；需要这些数据时再从硬盘
// 中读取，并将另外一部分不用的数据写入硬盘。这样，硬盘中就会有一部分空间用来存放内存中暂时不用的数据。这一部分空间就叫做虚拟内存（Virtual Memory）。
// 硬盘的读写速度比内存慢很多，反复交换数据会消耗很多时间，所以如果你的内存太小，会严重影响计算机的运行速度，甚至会出现”卡死“现象，即使CPU强劲，也不会有大的改观。
// 如果经济条件允许，建议将内存升级为4G，在 win7、win8、win10下运行软件就会比较流畅了。
// 总结：CPU直接从内存中读取数据，处理完成后将结果再写入内存。

// 怎样将文字与二进制对应起来呢？需要有一套规范，计算机公司和软件开发者都必须遵守，这样的一套规范就称为字符集（Character Set）或者字符编码（Character Encoding）。
// 在计算机逐步发展的过程中，先后出现了几十种甚至上百种字符集，有些还在使用，有些已经淹没在了历史的长河中。专门针对英文的字符集——ASCII编码。
// 拉丁字母、阿拉伯字母、斯拉夫字母（西里尔字母）被称为世界三大字母体系。
// 总起来说：
//  基本拉丁字母就是26个英文字母；
//  扩展拉丁字母就是在基本的 26 个英文字母的基础上添加变音符号、横线、斜线等演化而来，每个国家都不一样。
// 计算机是美国人发明的，他们首先要考虑的问题是，如何将二进制和英文字母（也就是拉丁文）对应起来。
// ASCII是“American Standard Code for Information Interchange”的缩写，翻译过来是“美国信息交换标准代码”。ASCII编码已经成了计算机的通用标准。
// ASCII的标准版本于1967年第一次发布，最后一次更新则是在1986年，迄今为止共收录了128个字符，包含了基本的拉丁字母（英文字母）、阿拉伯数字（也就是1234567890）、
// 标点符号（,.!等）、特殊符号（@#$%^&等）以及一些具有控制功能的字符（往往不会显示出来）。
// 在ASCII编码中，大写字母、小写字母和阿拉伯数字都是连续分布的，程序设计很方便。例如要判断一个字符是否是大写字母，就可以判断该字符的ASCII编码值是否在65~90的范围内。
// C语言有时候使用ASCII编码，有时候却不是。

// 为了让本国公民也能使用上计算机，各个国家（地区）也开始效仿ASCII，开发了自己的字符编码。这些字符编码和ASCII一样，只考虑本国的语言文化，不兼容其它国家的文字。
// 这样做的后果就是，一台计算机上必须安装多套字符编码，否则就不能正确地跨国传递数据，例如在中国编写的文本文件，拿到日本的电脑上就无法打开，或者打开后是一堆乱码。
// 标准ASCII编码共包含了128个字符，用一个字节就足以存储（实际上是用一个字节中较低的7位来存储），而日文、中文、韩文等包含的字符非常多，有成千上万个，一个字节肯定
// 是不够的（一个字节最多存储28 = 256个字符），所以要进行扩展，用两个、三个甚至四个字节来表示。
// 在制定字符编码时还要考虑内存利用率的问题。我们经常使用的字符，其编码值一般都比较小，例如字母和数字都是ASCII编码，其编码值不会超过127，用一个字节存储足以，如果
// 硬要用多个字节存储，就会浪费很多内存空间。
// 为了达到「既能存储本国字符，又能节省内存」的目的，Shift-Jis、Big5、GB2312等都采用变长的编码方式：
//  对于原来的ASCII编码部分，用一个字节存储足以；
//  对于本国的常用字符（例如汉字、标点符号等），一般用两个字节存储；
//  对于偏远地区，或者极少使用的字符（例如藏文、蒙古文等），才使用三个甚至四个字节存储。
// GB2312 	简体中文字符集，1980年发布，共收录了6763个汉字，其中一级汉字3755个，二级汉字3008个；
// 同时收录了包括拉丁字母、希腊字母、日文平假名及片假名字母、俄语西里尔字母在内的682个字符。
// GB2312-->GBK-->GB18030是中文编码的三套方案，出现的时间从早到晚，收录的字符数目依次增加，并且向下兼容。
// GB2312和GBK收录的字符数目较少，用1~2个字节存储；GB18030收录的字符最多，用1、2、4个字节存储。
// GBK于1995年发布，这一年也是互联网爆发的元年，国人使用电脑越来越多，也许是GBK这头猪正好站在风口上，它就飞起来了，后来的中文版Windows都将GBK作为默认的中文编码方案。

// ASCII、GB2312、GBK、Shift_Jis、ISO/IEC 8859等地区编码都是各个国家为了自己的语言文化开发的，不具有通用性，在一种编码下开发的软件或者编写的文档，拿到另一种编
// 码下就会失效，必须提前使用程序转码，非常麻烦。
// 迫切希望有一种编码能够统一世界各地的字符，只要安装了这一种字编码，就能支持使用世界上所有的文字，再也不会出现乱码，再也不需要转码了，这对数据传递来说是多么的方便呀！
// 就在这种呼吁下，Unicode诞生了。Unicode也称为统一码、万国码；看名字就知道，Unicode希望统一所有国家的字符编码。
// Unicode于1994年正式公布第一个版本，现在的规模可以容纳100多万个符号，是一个很大的集合。
// Unicode可以使用的编码方案有三种，分别是：
//  UTF-8：一种变长的编码方案，使用1~6个字节来存储；
//  UTF-32：一种固定长度的编码方案，不管字符编号大小，始终使用4个字节来存储；
//  UTF-16：介于UTF-8和UTF-32之间，使用2个或者4个字节来存储，长度既固定又可变。
// 1) UTF-8的编码规则很简单：
//    如果只有一个字节，那么最高的比特位为0，这样可以兼容ASCII；
//    如果有多个字节，那么第一个字节从最高位开始，连续有几个比特位的值为1，就使用几个字节编码，剩下的字节均以10开头。
//    具体的表现形式为：
//     0xxxxxxx：单字节编码形式，这和ASCII编码完全一样，因此UTF-8是兼容ASCII的；
//     110xxxxx 10xxxxxx：双字节编码形式（第一个字节有两个连续的1）；
//     1110xxxx 10xxxxxx 10xxxxxx：三字节编码形式（第一个字节有三个连续的1）；
//     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx：四字节编码形式（第一个字节有四个连续的1）。
//    xxx就用来存储Unicode中的字符编号。
//    对于常用的字符，它的Unicode编号范围是0~FFFF，用1~3个字节足以存储，只有及其罕见，或者只有少数地区使用的字符才需要4~6个字节存储。
// 2) UTF-32是固定长度的编码，始终占用4个字节，足以容纳所有的Unicode 字符，所以直接存储Unicode编号即可，不需要任何编码转换。浪费了空间，提高了效率。
// 3) UFT-16比较奇葩，它使用2个或者4个字节来存储。
//    对于Unicode编号范围在0~FFFF之间的字符，UTF-16使用两个字节存储，并且直接存储Unicode编号，不用进行编码转换，这跟UTF-32非常类似。
//    对于Unicode编号范围在10000~10FFFF之间的字符，UTF-16使用四个字节存储，具体来说就是：将字符编号的所有比特位分成两部分，较高的一些比特位用一个值介于D800~DBFF
//    之间的双字节存储，较低的一些比特位（剩下的比特位）用一个值介于DC00~DFFF之间的双字节存储。
// 对比以上三种编码方案:
// 首先，只有UTF-8兼容ASCII，UTF-32和UTF-16都不兼容ASCII，因为它们没有单字节编码。
// 1) UTF-8使用尽量少的字节来存储一个字符，不但能够节省存储空间，而且在网络传输时也能节省流量，所以很多纯文本类型的文件（例如各种编程语言的源文件、各种日志文件和配置文
//    件等）以及绝大多数的网页（例如百度、新浪、163等）都采用UTF-8编码。
// 2) UTF-32是“以空间换效率”，正好弥补了UTF-8的缺点，UTF-32的优势就是效率高：UTF-32在存储和读取字符时不需要任何转换，在处理字符串时也能最快速地定位字符。
//    UTF-32的缺点也很明显，就是太占用存储空间了，在网络传输时也会消耗很多流量。平常使用的字符编码值一般都比较小，用一两个字节存储足以，用四个字节简直是暴殄天物，甚至
//    说是不能容忍的，所以UTF-32在应用上不如UTF-8和UTF-16广泛。
// 3) UTF-16可以看做是UTF-8和UTF-32的折中方案，它平衡了存储空间和处理效率的矛盾。对于常用的字符，用两个字节存储足以，这个时候是不需要转换的，直接存储字符的编码值即可。
// Windows内核、.NET Framework、Cocoa、Java String内部采用的都是UTF-16编码。UTF-16是幕后的功臣。
// 不过，UNIX 家族的操作系统（Linux、Mac OS、iOS 等）内核都采用UTF-8编码。
// 有的编码方式采用1~n个字节存储，是变长的，例如UTF-8、GB2312、GBK等；如果一个字符使用了这种编码方式，我们就将它称为多字节字符，或者窄字符。
// 有的编码方式是固定长度的，不管字符编号大小，始终采用n个字节存储，例如UTF-32、UTF-16等；如果一个字符使用了这种编码方式，我们就将它称为宽字符。
// Unicode字符集可以使用窄字符的方式存储，也可以使用宽字符的方式存储；GB2312、GBK、Shift-JIS等国家编码一般都使用窄字符的方式存储；ASCII只有一个字节，无所谓窄字符和宽字符。

// 计算机起源于美国，C语言、C++、Java、JavaScript等很多流行的编程语言都是美国人发明的，所以在编写代码的时候必须使用英文半角输入法，尤其是标点符号。
// C语言的编译器有很多种，不同的平台下有不同的编译器，例如：
//  Windows下常用的是微软开发的Visual C++，它被集成在Visual Studio中，一般不单独使用；
//  Linux下常用的是GUN组织开发的GCC，很多Linux发行版都自带GCC；
//  Mac下常用的是LLVM/Clang，它被集成在Xcode中（Xcode以前集成的是GCC，后来由于GCC的不配合才改为LLVM/Clang，LLVM/Clang的性能比GCC更加强大）。
// C语言代码经过编译以后，并没有生成最终的可执行文件，而是生成了一种叫做目标文件（Object File）的中间文件（或者说临时文件）。目标文件也是二进制形式的，它和可执行文件的格
// 式是一样的。对于Visual C++，目标文件的后缀是.obj；对于GCC，目标文件的后缀是.o。
// 目标文件经过链接（Link）以后才能变成可执行文件。既然目标文件和可执行文件的格式是一样的，为什么还要再链接一次呢，直接作为可执行文件不行吗？
// 不行的！因为编译只是将我们自己写的代码变成了二进制形式，它还需要和系统组件（比如标准库、动态链接库等）结合起来，这些组件都是程序运行所必须的。
// 链接（Link）其实就是一个“打包”的过程，它将所有二进制形式的目标文件和系统组件组合成一个可执行文件。完成链接的过程也需要一个特殊的软件，叫做链接器（Linker）。
// 再次强调，编译是针对一个源文件的，有多少个源文件就需要编译多少次，就会生成多少个目标文件。
// 不管我们编写的代码有多么简单，都必须经过「编译-->链接」的过程才能生成可执行文件：
//  编译就是将我们编写的源代码“翻译”成计算机可以识别的二进制格式，它们以目标文件的形式存在；
//  链接就是一个“打包”的过程，它将所有的目标文件以及系统组件组合成一个可执行文件。

// Unix和C语言的开发者是同一人，名字叫丹尼斯·里奇（Dennis MacAlistair Ritchie）。
// 1967年，26岁的丹尼斯·里奇进入贝尔实验室开发Unix，并于1969年圣诞节前推出第一个试运行版本。这个时候的Unix是用汇编语言写的，移植性非常差，要想让Unix运行在不同型号的机
// 器上，就得针对每个型号的机器重写一遍操作系统，这显然是一个不可能完成的任务。
// 为了提高通用性和开发效率，丹尼斯·里奇决定发明一种新的编程语言——C语言。紧接着，丹尼斯·里奇就用C语言改写了Unix上的C语言编译器，他的同事汤姆森则使用C语言重写了Unix，使
// 它成为一种通用性强、移植简单的操作系统，从此开创了计算机编程史上的新篇章，C语言也成为了操作系统专用语言。
// 为统一C语言版本，1983 年美国国家标准局（American National Standards Institute，简称 ANSI）成立了一个委员会，专门来制定C语言标准。1989年C语言标准被批准，被称
// 为ANSI X3.159-1989 "Programming Language C"。这个版本的C语言标准通常被称为ANSI C。又由于这个版本是89年完成制定的，因此也被称为C89。
// 后来ANSI把这个标准提交到ISO（国际化标准组织），1990年被ISO采纳为国际标准，称为ISO C。又因为这个版本是1990年发布的，因此也被称为C90。
// ANSI C（C89）与 ISO C（C90）内容基本相同，主要是格式组织不一样。
// 因为ANSI与ISO的C标准内容基本相同，所以对于C标准，可以称为ANSI C，也可以说是ISO C，或者ANSI/ISO C。以后大家看到ANSI C、ISO C、C89、C90，要知道这些内容都是一样的。
// 在ANSI C标准确立之后，C语言的规范在很长一段时间内都没有大的变动。1995年C程序设计语言工作组对C语言进行了一些修改，增加了新的关键字，编写了新的库，取消了原有的限制，并于
// 1999年形成新的标准——ISO/IEC 9899:1999 标准，通常被成为C99。
// C11标准由国际标准化组织（ISO）和国际电工委员会（IEC）旗下的C语言标准委员会于2011年底正式发布，支持此标准的主流C语言编译器有GCC、LLVM/Clang、Intel C++ Compile等。

int main() { return 0; }