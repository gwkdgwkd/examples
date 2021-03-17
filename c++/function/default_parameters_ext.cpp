#include <iostream>
using namespace std;

void func2(int a, int b = 10, int c = 36) {
  cout << a << ", " << b << ", " << c << endl;
}

void func3(int a, int b = 8, int c = 44) {
  cout << a << ", " << b << ", " << c << endl;
}