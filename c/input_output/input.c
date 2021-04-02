#include <stdio.h>

// 在C语言中，有多个函数可以从键盘获得用户输入：
//  scanf()：和printf()类似，scanf()可以输入多种类型的数据。
//  getchar()、getche()、getch()：这三个函数都用于输入单个字符。
//  gets()：获取一行数据，并作为字符串处理。
// scanf()是最灵活、最复杂、最常用的输入函数，但它不能完全取代其他函数。
// 对于scanf()，输入数据的格式要和控制字符串的格式保持一致。
// scanf()对输入数据之间的空格的处理比较宽松，并不要求空格数严格对应，多几个少几个无所谓，只要有空格就行。
// 每次按下回车键，就会认为完成了一次输入操作，scanf()开始读取输入的内容，并根据格式控制字符串从中提取有效数据，只要用户输入的内容和格式控制字符串匹配，就能够正确提取。

// 连续输入
// scanf()读取完毕后没有抛弃多余的值，而是将它们保存在了某个地方，下次接着使用。
// 从本质上讲，从键盘输入的数据并没有直接交给scanf()，而是放入了缓冲区中，直到我们按下回车键，scanf()才到缓冲区中读取数据。
// 如果缓冲区中的数据符合scanf()的要求，那么就读取结束；如果不符合要求，那么就继续等待用户输入，或者干脆读取失败。
// scanf()不会跳过不符合要求的数据，遇到不符合要求的数据会读取失败，而不是再继续等待用户输入。
// 总而言之，正是由于缓冲区的存在，才使得我们能够多输入一些数据，或者一次性输入所有数据，这可以认为是缓冲区的一点优势。

// 除了输入整数，scanf()还可以输入单个字符、字符串、小数等。
// scanf()和printf()虽然功能相反，但是格式控制符是一样的，单个字符、整数、小数、字符串对应的格式控制符分别是%c、%d、%f、%s。
// scanf()读取字符串时以空格为分隔，遇到空格就认为当前字符串结束了，所以无法读取含有空格的字符串。

// scanf()格式控制符汇总：
// %c 	           读取一个单一的字符
// %hd、%d、%ld 	读取一个十进制整数，并分别赋值给short、int、long类型
// %ho、%o、%lo 	读取一个八进制整数（可带前缀也可不带），并分别赋值给short、int、long类型
// %hx、%x、%lx 	读取一个十六进制整数（可带前缀也可不带），并分别赋值给short、int、long类型
// %hu、%u、%lu 	读取一个无符号整数，并分别赋值给unsigned short、unsigned int、unsigned long类型
// %f、%lf 	        读取一个十进制形式的小数，并分别赋值给float、double类型
// %e、%le 	        读取一个指数形式的小数，并分别赋值给float、double类型
// %g、%lg 	        既可以读取一个十进制形式的小数，也可以读取一个指数形式的小数，并分别赋值给float、double类型
// %s 	            读取一个字符串（以空白符为结束）

// 输入单个字符
// getchar()、getche()和getch()这三个专用的字符输入函数，它们具有某些scanf()没有的特性，是scanf()不能代替的。
// 1) getchar()
//    最容易理解的字符输入函数是getchar()，它就是scanf("%c",c)的替代品，除了更加简洁，没有其它优势了；或者说，getchar()就是scanf()的一个简化版本。
// 2) getche()
//    getche()就比较有意思了，它没有缓冲区，输入一个字符后会立即读取，不用等待用户按下回车键，这是它和scanf()、getchar()的最大区别。
//    getche()位于conio.h 头文件中，而这个头文件是Windows特有的，Linux和Mac OS下没有包含该头文件。
//    换句话说，getche()并不是标准函数，默认只能在Windows下使用，不能在Linux和Mac OS下使用。
// 3) getch()
//    getch()也没有缓冲区，输入一个字符后会立即读取，不用按下回车键，这一点和getche()相同。getch()的特别之处是它没有回显，看不到输入的字符。所谓回显，
//    就是在控制台上显示出用户输入的字符；没有回显，就不会显示用户输入的字符，就好像根本没有输入一样。
//    和getche()一样，getch()也位于conio.h头文件中，也不是标准函数，默认只能在Windows下使用，不能在Linux和Mac OS下使用。

// 输入字符串
// 输入字符串当然可以使用scanf()这个通用的输入函数，对应的格式控制符为%s。 gets()这个专用的字符串输入函数，它拥有一个scanf()不具备的特性。
// gets()是有缓冲区的，每次按下回车键，就代表当前输入结束了，gets()开始从缓冲区中读取内容，这一点和scanf()是一样的。gets()和scanf()的主要区别是：
//  scanf()读取字符串时以空格为分隔，遇到空格就认为当前字符串结束了，所以无法读取含有空格的字符串。
//  gets()认为空格也是字符串的一部分，只有遇到回车键时才认为字符串输入结束，所以，不管输入了多少个空格，只要不按下回车键，对gets()来说就是一个完整的字符串。
// 也就是说，gets()能读取含有空格的字符串，而scanf()不能。

// 总结:
// 1 C语言中常用的从控制台读取数据的函数有五个，它们分别是scanf()、getchar()、getche()、getch()和gets()。
//   其中scanf()、getchar()、gets()是标准函数，适用于所有平台；getche()和getch()不是标准函数，只能用于Windows。
// 2 scanf()是通用的输入函数，它可以读取多种类型的数据。
// 3 getchar()、getche()和getch()是专用的字符输入函数，它们在缓冲区和回显方面与scanf()有着不同的特性，是scanf()不能替代的。
// 4 gets()是专用的字符串输入函数，与 scanf()相比gets()的主要优势是可以读取含有空格的字符串。
// 5 scanf()可以一次性读取多份类型相同或者不同的数据，getchar()、getche()、getch()和gets()每次只能读取一份特定类型的数据，不能一次性读取多份数据。

int main() {
  int flag = 7;
  if (1 == flag) {
    // 12 60 10 23↙ 两个scanf()都能正确读取。
    // 12 60 10 23 99↙ scanf仍能正常工作，只是99没有用。
    // 12 60 10↙ 第一个scanf工作正常，第二个等待输入。
    int a = 0, b = 0, c = 0, d = 0;
    scanf("%d", &a);            // 输入整数并赋值给变量a
    scanf("%d", &b);            // 输入整数并赋值给变量b
    printf("a+b=%d\n", a + b);  // 计算a+b的值并输出
    scanf("%d %d", &c, &d);     // 输入两个整数并分别赋值给c、d
    printf("c*d=%d\n", c * d);  // 计算c*d的值并输出
  } else if (2 == flag) {
    int a = 'F';
    int b = 12;
    int c = 452;
    printf("&a=%p, &b=%p, &c=%p\n", &a, &b, &c);
    // &a=0x7ffe91e79078, &b=0x7ffe91e7907c, &c=0x7ffe91e79080
  } else if (3 == flag) {
    int a, b, c;
    scanf("%d %d", &a, &b);
    printf("a+b=%d\n", a + b);
    scanf("%d   %d", &a, &b);
    printf("a+b=%d\n", a + b);
    scanf("%d, %d, %d", &a, &b, &c);
    printf("a+b+c=%d\n", a + b + c);

    scanf("%d is bigger than %d", &a, &b);
    printf("a-b=%d\n", a - b);
  } else if (4 == flag) {
    // 然而，缓冲区也带来了一定的负面影响，甚至会导致很奇怪的行为
    int a = 1, b = 2;
    scanf("a=%d", &a);
    scanf("b=%d", &b);
    printf("a=%d, b=%d\n", a, b);
    // 输入a=99，按下回车键，程序竟然运行结束了，只有第一个scanf()成功读取了数据，第二个scanf()仿佛没有执行一样，根本没有给用户任何机会去输入数据。
    // a=99↙
    // a=99, b=2
    // 中间是没有任何空格的，两个scanf都读取正常。
    // a=99b=200↙
    // a=99, b=200
    // 两个数据之间有空格又会怎么样呢？第二个scanf()又读取失败了！
    // a=99 b=200↙
    // a=99, b=2
  } else if (5 == flag) {
    char letter;
    int age;
    char url[30];
    float price;

    scanf("%c", &letter);
    scanf("%d", &age);
    scanf("%s", url);  //可以加&也可以不加&
    scanf("%f", &price);

    printf("26个英文字母的最后一个是 %c。\n", letter);
    printf("C语言中文网已经成立%d年了，网址是 %s，开通VIP会员的价格是%g。\n",
           age, url, price);
    // c 12 nihao 3.14↙
    // 26个英文字母的最后一个是 c。
    // C语言中文网已经成立12年了，网址是 nihao，开通VIP会员的价格是3.14。
  } else if (6 == flag) {
    char c;
    c = getchar();
    printf("c: %c\n", c);
  } else if (7 == flag) {
    char author[30], lang[30], url[30];
    gets(author);
    printf("author: %s\n", author);
    gets(lang);
    printf("lang: %s\n", lang);
    gets(url);
    printf("url: %s\n", url);
  }

  return 0;
}