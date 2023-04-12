#include <stdio.h>
#include <stdlib.h>

// 所谓表达式，就是由变量、常量以及运算符组合而成的式子。
// 其中，常用的运算符无非!（阶乘运算符）、^（指数运算符）、+、-、*、/ 、()这几种，
// 比如： 3!+4*2/(1-5)^2就是一个表达式。
// 那么，如何用栈结构求一个表达式的值呢？实际上，已经有前辈设计好了一种完美的解决方案。
// 1929年，波兰逻辑学家J・卢卡西维兹提出了一种全新的表示表达式的方法，
// 称为后缀表达式或者逆波兰表达式，和普通表达式不同，
// 后缀表达式习惯将运算符写在它的运算项之后，且整个表达式中不用括号()表明运算的优先级关系。

// 用栈结构求表达式的值的完整解决方案为：
// 1.将用户输入的普通表达式，借助调用场算法转换为后缀表达式；
// 2.由第一步得到的后缀表达式，计算出它的值。

// 以3!为例，!为运算符，3为运算项，因此3!本身就是一个后缀表达式；
// 再以4*2为例，*为运算符，4和2作为它的运算项，其对应的后缀表达式为42*。
// 在此基础上，试着将 3!+4*2/(1-5)^2转换成后缀表达式，
// 其过程也就是将表达式中所有运算符放置在它的运算项之后：
//  !运算符对应的运算项为3，转换后得到3!；
//  +运算符对应的运算项是3!和4*2/(1-5)^2，转换之后得到：3! 4*2/(1-5)^2 +；
//  *运算符对应的运算项是4和 2，转换之后得到4 2 *；
//  /运算符对应的运算项是4 2 *和(1-5)^2，转换后得到4 2 * (1-5)^2 /；
//  -运算符对应的运算项是1和5，转换后得到1 5 -；
//  ^运算符对应的运算项是15 -和2 ，转换后得到1 5 - 2 ^。
// 整合之后，整个普通表达式就转换成了3 ! 4 2 * 1 5 - 2 ^ / +，这就是其对应的后缀表达式。
// 不难发现，后缀表达式完全舍弃了表达式本该有的可读性，但有失必有得，
// 相比普通表达式，后缀表达式的值可以轻松借助栈存储结构求得。

// 当用户给定一个后缀表达式时，按照从左到右的顺序依次扫描表达式中的各个运算项和运算符，
// 对它们进行如下处理：
// 1.遇到运算项时，直接入栈；
// 2.遇到运算符时，将位于栈顶的运算项出栈，对于!运算符，取栈顶1个运算项；
//   其它运算符，取栈顶2个运算项，第一个取出的运算项作为该运算符的右运算项，
//   另一个作为左运算项，求此表达式的值并将其入栈。
// 经过以上操作，直到栈中仅存在一个运算项为止，此运算项即为整个表达式的值。
// 以3 ! 4 2 * 1 5 - 2 ^ / +表达式为例，求值的过程为：
// 1.从3开始，它是运算项:[3]
// 2.!作为运算符，从栈顶取1个运算项（也就是3），求3!的值（3! = 3*2*1=6）并将其入栈：[6]
// 3.将4和2先后入栈：[6 4 2]
// 4.对于*运算符，取栈顶2个运算项（2和4），其中先取出的2作为*的右操作数，4作为左操作数。
//   求的4*2的值8，并将其入栈：[6 8]
// 5.将1和5先后入栈：[6 8 1 5]
// 6.对于-运算符，取栈顶2个运算项（5和1），计算出1-5的值为-4，将其入栈：[6 8 -4]
// 7.将2入栈：[6 8 -4 2]
// 8.对于^运算符，取栈顶2个运算项（2和-4），计算出-4^2的值16 ，将其入栈：[6 8 16]
// 9.对于/运算符，取栈顶2个运算项（16和8），计算出8/16的值0.5，将其入栈：[6 0.5]
// 10.对于+运算符，取栈顶2个运算符（0.5和6），计算出6+0.5的值6.5，将其入栈：[6.5]

typedef enum { false = 0, true = 1 } bool;
#define MAXSIZE 50

typedef struct {
  char data[MAXSIZE];
  int top;
} Stack;

void InitStack(Stack **s) {
  *s = (Stack *)malloc(sizeof(Stack));
  (*s)->top = -1;
}

bool Push(Stack *s, char e) {
  if (s->top == MAXSIZE - 1) return false;
  s->top++;
  s->data[s->top] = e;
  return true;
}

bool Pop(Stack **s, char *e) {
  if ((*s)->top == -1) return false;
  *e = (*s)->data[(*s)->top];
  (*s)->top--;
  return true;
}

bool GetTop(Stack **s, char *e) {
  if ((*s)->top == -1) return false;
  *e = (*s)->data[(*s)->top];
  return true;
}

bool StackEmpty(Stack **s) {
  if ((*s)->top == -1) return true;
  return false;
}

// 后缀表达式的转换
// 对于普通用户来讲，令其输入一个正确的后缀表达式显然是不实现的，
// 只能要求他们输入一个正确的普通表达式，如何将一个普通表达式转换成后缀表达式？
// 幸运的是，针对这个问题，伟人迪杰斯特拉给出了一个完美的解决方案，
// 称为调用场算法，该算法可以实现将一个普通表达式转换成后缀表达式。
// 调用场算法的实现，需要借助一个空栈（假设栈名为Optr）和一个空数组（假设数组名为postexp）。
// 对于给定的一个普通表达式，调用场算法的转换过程是逐个遍历表达式中的每个字符：
// 1.如果为0~9的字符，将其添加到postexp数组的末尾；
// 2.如果该字符为除(和)以外的运算符，将其与Optr栈顶的运算符进行大小比较，
//   如果该运算符大于栈顶运算符，则将其入栈；
//   反之，如果该运算符小于栈顶运算符，则将栈顶运算符出栈并添加到postexp数组的尾部，
//   然后继续拿当前运算符同新的栈顶运算符做大小比较，以此类推。
//   如果该字符为(运算符，直接入栈；如果为)运算符，
//   依次取Optr栈顶运算符并将其添加到postexp数组末尾，直到遇到'('字符为止。
//   注意，(字符也从栈顶取出，但不将其添加postexp数组中。
//  依照以上处理过程，直到将普通表达式遍历完毕，如果Optr栈中仍有运算符，
//  依次将它们出栈并添加到postexp数组尾部。
//  最终，postexp数组内存储的表达式就是转换后的后缀表达式。
//  第2步中关于运算符的大小比较，迪杰斯塔拉给出了如下所示的表格：
//                       当前运算符
//    栈顶元素       + 	 - 	 * 	 / 	 ^ 	 !
// 			+       > 	> 	< 	< 	< 	<
// 			-       > 	> 	< 	< 	< 	<
// 			*       > 	> 	> 	> 	< 	<
// 			/       > 	> 	> 	> 	< 	<
// 			^       > 	> 	> 	> 	> 	<
// 			!       > 	> 	> 	> 	> 	>
// 			(       < 	< 	< 	< 	< 	<
// 以3!+4*2/(1-5)^2为例，接下来为大家演示调用场算法的整个转换过程，遍历整个表达式：
// 1.对于字符3，直接将其添加postexp数组的尾部：ostexp[3],Optr[]
// 2.遍历至!，将其与Optr栈顶字符进行比较，由于此时Optr为空栈，直接入栈：ostexp[3],Optr[!]
// 3.遍历至+，Optr栈顶运算符! > +，将!从Optr栈中取出并添加到postexp数组末尾。
//   此时，Optr栈为空，将+入栈：ostexp[3 !],Optr[+]
// 4.遍历至4，直接添加到postexp数组末尾：ostexp[3 ! 4],Optr[+]
// 5.遍历至*，Optr栈顶运算符+ < *，所以将*入栈：ostexp[3 ! 4],Optr[+ *]
// 6.遍历至2，将其添加至postexp数组的末尾：ostexp[3 ! 4 2],Optr[+ *]
// 7.遍历至/，Optr栈顶运算符* > /，将*取出并添加到postexp数组末尾：ostexp[3 ! 4 2 *],Optr[+]
//   继续用/ 同Optr栈顶的+运算符比较，+ < /，将/入栈：ostexp[ 3 ! 4 2 * ],Optr[ + / ]
// 8.遍历至(，直接入栈：ostexp[3 ! 4 2 *],Optr[+ / (]
// 9.遍历至1，将其添加到postexp数组末尾：ostexp[3 ! 4 2 * 1],Optr[+ / (]
// 10.遍历至-，Optr栈顶运算符( < -，将-入栈：ostexp[3 ! 4 2 * 1],Optr[+ / ( -]
// 11.遍历至5，添加到postexp数组末尾：ostexp[3 ! 4 2 * 1 5],Optr[+ / ( -]
// 12.遍历至)，对Optr栈一直做出栈操作并将出栈元素添加到postexp数组末尾，直到将(取出：
//    ostexp[3 ! 4 2 * 1 5 -],Optr[+ /]
// 13.遍历至^，Optr栈顶运算符/ < ^，将^入栈：ostexp[3 ! 4 2 * 1 5 -],Optr[+ / ^]
// 14.遍历至2，将其添加到postexp数组末尾：ostexp[ 3 ! 4 2 * 1 5 - 2 ],Optr[ + / ^ ]
// 15.将Optr栈做出栈操作，并逐个将出栈元素添加到postexp数组末尾，直至Optr栈为空：
//    postexp[3 ! 4 2 * 1 5 - 2 ^ / +],Optr[]
//    postexp数组中存储的就是3!+4*2/(1-5)^2对应的后缀表达式。
void trans(char *exp, char postexp[]) {
  int i = 0;
  char e;
  Stack *Optr;
  InitStack(&Optr);

  while (*exp != '\0') {
    switch (*exp) {
      case '(':
        Push(Optr, '(');
        exp++;
        break;
      case ')':
        Pop(&Optr, &e);
        while (e != '(') {
          postexp[i++] = e;
          Pop(&Optr, &e);
        }
        exp++;
        break;
      case '+':
      case '-':
        while (!StackEmpty(&Optr)) {
          GetTop(&Optr, &e);
          if (e == '(')
            break;
          else {
            postexp[i++] = e;
            Pop(&Optr, &e);
          }
        }
        Push(Optr, *exp);
        exp++;
        break;
      case '*':
      case '/':
        while (!StackEmpty(&Optr)) {
          GetTop(&Optr, &e);
          if (e == '/' || e == '*' || e == '^' || e == '!') {
            postexp[i++] = e;
            Pop(&Optr, &e);
          } else
            break;
        }
        Push(Optr, *exp);
        exp++;
        break;
      case '^':
        while (!StackEmpty(&Optr)) {
          GetTop(&Optr, &e);
          if (e == '^' || e == '!') {
            postexp[i++] = e;
            Pop(&Optr, &e);
          } else
            break;
        }
        Push(Optr, *exp);
        exp++;
        break;
      case '!':
        while (!StackEmpty(&Optr)) {
          GetTop(&Optr, &e);
          if (e == '!') {
            postexp[i++] = e;
            Pop(&Optr, &e);
          } else
            break;
        }
        Push(Optr, *exp);
        exp++;
        break;
      default:
        while (*exp > '0' && *exp < '9') {
          postexp[i++] = *exp;
          exp++;
        }
        postexp[i++] = '#';
    }
  }
  while (!StackEmpty(&Optr)) {
    Pop(&Optr, &e);
    postexp[i++] = e;
  }
  postexp[i] = '\0';
  free(Optr);
}

typedef struct {
  double data[MAXSIZE];
  int top;
} Stack_num;

void InitStack_num(Stack_num **s) {
  *s = (Stack_num *)malloc(sizeof(Stack_num));
  (*s)->top = -1;
}

bool Push_num(Stack_num **s, double e) {
  if ((*s)->top == MAXSIZE - 1) return false;
  (*s)->top++;
  (*s)->data[(*s)->top] = e;
  return true;
}

bool Pop_num(Stack_num **s, double *e) {
  if ((*s)->top == -1) return false;
  *e = (*s)->data[(*s)->top];
  (*s)->top--;
  return true;
}

double compvalue(char *postexp) {
  Stack_num *num;
  int i = 1;
  double result;
  double a, b;
  double c;
  double d;
  InitStack_num(&num);
  while (*postexp != '\0') {
    switch (*postexp) {
      case '+':
        Pop_num(&num, &a);
        Pop_num(&num, &b);
        c = b + a;
        Push_num(&num, c);
        break;
      case '-':
        Pop_num(&num, &a);
        Pop_num(&num, &b);
        c = b - a;
        Push_num(&num, c);
        break;
      case '*':
        Pop_num(&num, &a);
        Pop_num(&num, &b);
        c = b * a;
        Push_num(&num, c);
        break;
      case '/':
        Pop_num(&num, &a);
        Pop_num(&num, &b);
        if (a != 0) {
          c = b / a;
          Push_num(&num, c);
        } else {
          printf("除0错误\n");
          exit(0);
        }
        break;
      case '^':
        Pop_num(&num, &a);
        Pop_num(&num, &b);
        if (a != 0) {
          i = 1;
          c = 1;
          while (i <= a) {
            c = c * b;
            i++;
          }
        } else if (b != 0) {
          c = 1;
        } else {
          c = 0;
        }
        Push_num(&num, c);
        break;
      case '!':
        Pop_num(&num, &a);
        c = 1;
        i = a;
        while (i != 0) {
          c = c * i;
          i--;
        }
        Push_num(&num, c);
        break;
      default:
        d = 0;
        while (*postexp >= '0' && *postexp <= '9') {
          d = 10 * d + (*postexp - '0');
          postexp++;
        }
        Push_num(&num, d);
    }
    postexp++;
  }
  Pop_num(&num, &result);
  return result;
}

bool test(char *exp) {
  int i = 0;
  Stack *s;
  char e;
  for (; exp[i] != '\0'; i++) {
    if (!((exp[i] >= '0' && exp[i] <= '9') || exp[i] == '!' || exp[i] == '^' ||
          exp[i] == '+' || exp[i] == '-' || exp[i] == '*' || exp[i] == '/' ||
          exp[i] == '(' || exp[i] == ')'))

      return false;
  }

  InitStack(&s);
  for (i = 0; exp[i] != '\0'; i++) {
    switch (exp[i]) {
      case '(':
        Push(s, exp[i]);
        break;
      case ')':
        if (Pop(&s, &e)) {
          if (exp[i] == ')' && e == '(')
            return true;
          else
            return false;
        } else
          return false;
        break;
    }
  }

  if (s->top != -1)
    return false;
  else
    return true;
  return true;
}

int main() {
  char exp[MAXSIZE];
  char postexp[MAXSIZE];
  while (true) {
    printf("输入表达式：");
    scanf("%s", exp);
    if (!test(exp)) {
      printf("非法字符\n");
      continue;
    }
    trans(exp, postexp);
    printf("后缀表达式为:%s\n", postexp);
    printf("表达式的值为:%lf\n", compvalue(postexp));
  }
  return 0;
}