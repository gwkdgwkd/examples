#include <climits>
#include <iostream>

using namespace std;

// C++11标准规定，每种整数类型必须同时具备有符号（signed）和无符号（unsigned）两种类型，且每种具体的有符号整形和无符号整形所占用的存储空
// 间（也就是位数）必须相同。注意，C++11标准中只限定了每种类型最少占用多少存储空间，不同的平台可以占用不同的存储空间。

// 早在1995年，就有人提议将longlong整形写入C++98标准，但被委员会拒绝了。而后longlong整形被C99标准（C语言标准之一）采纳，并逐渐被很多
// 编译器支持，于是C++标准委员会重新决定将long long整形写入C++ 11标准中。

// 如同long类型整数需明确标注"L"或者"l"后缀一样，要使用longlong类型的整数，也必须标注对应的后缀：
// 对于有符号longlong整形，后缀用"LL"或者"ll"标识。例如，"10LL"就表示有符号超长整数10；
// 对于无符号longlong整形，后缀用"ULL"、"ull"、"Ull"或者"uLL"标识。例如，"10ULL"就表示无符号超长整数10；
// 如果不添加任何标识，则所有的整数都会默认为int类型。

int main() {
  // 对于任意一种数据类型，读者可能更关心的是此类型的取值范围。对于longlong类型来说，如果想了解当前平台上longlong整形的取值范围，
  // 可以使用<climits>头文件中与longlong整形相关的3个宏，分别为LLONG_MIN、LLONG_MAX和ULLONG_MIN：
  // LLONG_MIN：代表当前平台上最小的longlong类型整数；
  // LLONG_MAX：代表当前平台上最大的longlong类型整数；
  // ULLONG_MIN：代表当前平台上最大的unsignedlonglong类型整数（无符号超长整型的最小值为0）；
  cout << "ll min：" << LLONG_MIN << " " << hex << LLONG_MIN << endl;
  // ll min：-9223372036854775808 8000000000000000
  cout << dec << "ll max：" << LLONG_MAX << " " << hex << LLONG_MAX << endl;
  // ll max：9223372036854775807 7fffffffffffffff
  cout << dec << "ull max：" << ULLONG_MAX << " " << hex << ULLONG_MAX << endl;
  // ull max：18446744073709551615 fffffffffffffff

  return 0;
}

/*
C++11标准中所有的整形数据类型:
整数类型 	                     等价类型 	                        C++11标准规定占用最少位数
short 	                        short int（有符号短整型） 	      至少16位（2个字节）
signed short
signed short int
-----------------------------------------------------------------
unsigned short 	                unsigned short int（无符号短整型）
unsigned short int
------------------------------------------------------------------------------------------
int 	                        int（有符号整形） 	                 至少16位（2个字节）
signed
signed int
-----------------------------------------------------------------
unsigned 	                    unsigned int（无符号整形）
unsigned int
------------------------------------------------------------------------------------------
long 	                        long int（有符号长整形） 	           至少32位（4个字节）
long int
signed long
signed long int
-----------------------------------------------------------------
unsigned long 	                unsigned long int（无符号长整形）
unsigned long int
------------------------------------------------------------------------------------------
long long（C++11） 	             long long int（有符号超长整形） 	  至少64位（8个字节）
long long int（C++11）
signed long long（C++11）
signed long long int（C++11）
-----------------------------------------------------------------
unsigned long long（C++11） 	 unsigned long long int（无符号超长整型）
unsigned long long int（C++11）
*/