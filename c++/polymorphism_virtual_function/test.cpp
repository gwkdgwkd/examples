// 静态多态：函数重载、模板
// 动态多态：虚函数
#include <iostream>

class A{};
class B{
    public:

    private:
    int i;
};

class C{
public:
    void func1() {}
    private:
    static int j;
    int i;
};

class D{
public:

    void func1() {std::cout << "D::func1"<< std::endl;}
    virtual void func2() {std::cout << "D::func2"<< std::endl;}
    private:
    static int j;
    int i;
};

class Dri :public D{
    public:
    virtual void func2() {std::cout << "Dri::func2"<< std::endl;}
    private:
    int k;
};



int main() {
    std::cout << sizeof(A) << std::endl;
    std::cout << sizeof(B) << std::endl;
    std::cout << sizeof(C) << std::endl;
    std::cout << sizeof(D) << std::endl;

    D d;
    d.func1();
    d.func2();

    Dri dd;
    dd.func2();

    D d1 = dd;

    d1.func2();
    d1.func1();

    D *d2 = &dd;
    d2->func2();
    d2->func1();

    // d2 = &d;

    D &d3 = dd;
    d3.func2();
    d3.func1();
    
}