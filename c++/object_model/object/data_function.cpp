#include <iostream>

using namespace std;

// 4.1 Member的各种调用方式

// Nonstatic Member Functions（非静态成员函数）
// C++的设计准则之一就是：非静态成员函数至少必须和普通函数有相同的效率。
// 选择成员函数不应该带来什么额外负担，这是因为编译器内部已将成员函数转换为对等的普通函数实体。转换步骤为：
//  1 改写函数签名，以安插一个额外的参数到成员函数中，用以提供一个存取管道，是对象可以调用该函数。额外参数被称为this指针。
//    Point3d Point3d::magnitude(Point3d *const this)
//    Point3d Point3d::magnitude(const Point3d *const this)
//  2 将每一个对nonstatic data member的存取操作改为经由this指针来存取。
//    this->_x * this->_x
//  3 将成员函数重新写成一个外部函数。对函数名称进行“manglint”处理，是它在程序中称为独一无二的名称。
//    magnitude__7Point3dFv(register Point3d *const this);
//    obj.magnitude();  ->  magnitude__7Point3dFv(&obj);
//    ptr->magnitude();  ->  magnitude__7Point3dFv(ptr);

// 名称的特殊处理（Name Mangling）
// 一般而言，member的名称会被加上class名称，形成独一无二的命名。为了处理重载，名称还要加上它们的参数链表。
// 目前C++编译器对name mangling的做法还没有统一。

// Virtual Member Function(虚拟成员函数)
// ptr->normalize();  ->  (*ptr->vptr[1])(ptr);
//  vptr表示有编译器产生的指针，指向vitual table。它被安插在每一个“声明有（或继承自）一个或多个虚函数”的对象中。
//  事实上其名称也会被“mangled”，因为在一个复杂的class派生体系中，可能存在多个vptrs。
//  1是virtual table slot的索引值，关联到normalize()函数。
//  第二个ptr表示this指针。
// 在normalize中调用虚拟函数magnitude:
//  由于normaliz已经有虚拟机制而决议妥当，所以可以明确调用的实体会比较有效率，并因此压制虚拟机而产生不必要的重复调用操作：
//  register float mag = Point3d::magnitude();不是(*this->vptr[2])(this);
// 如果magnitude声明为inline函数会更有效率。
// “通过对象调用虚函数”这种操作总是被编译器像对待一般的非静态成员函数一样地加以决议。

// Static Member Functions（静态成员函数）
// 无论是通过指针还是通过对象调用静态成员函数，都将被转换为一般的nonmember函数调用。
// Static Member Functions特性：
//  没有this指针(主要)。
//  不能够直接存取其class中nonstatic members。
//  不能够被声明为const、volatile或virtual。
//  不需要经由对象才能被调用，虽然大部分时候它是这样被调用的。
// 对一个静态成员函数取地址，获得的是内存中的位置。由于没有this指针，所以地址的类型不是“指向类成员函数的指针”，而是一个“非成员函数指针”。
// 静态成员函数差不多等同于普通函数，因此提供了意想不到的好处：
// 成为一个callback函数，使我们得以将C++和C系统结合。也可以成功地应用在线程函数身上。

// 4.2 Virtual Member Functions（虚拟成员函数）

// 在C++中，多态表示“以一个public base class的指针（或引用），寻址出一个派生类对象”的意思。
// 判断一个类是否支持多态，唯一适当的方法就是看看它是否有任何虚函数。只要有虚函数，它就需要额外的执行期信息：
//  一个表格，包含了程序的虚函数的执行期地址。
//  为了找到表格，每一个对象被安插上一个由编译器内部产生的指针，指向该表格。
//  为了找到函数地址，每一个虚函数被指派一个表格索引值。
// 上面的工作都由编译器完成。执行期间要做的，只是在特定的virtual table slot中调用virtual function。

// 一个类只会有一个virtual table。每一个table内含其对应的对象中所有active virtual functions函数实体的地址。可能包括：
//  这个类所定义的函数实体，它会改写一个可能存在的基类虚函数实体。
//  继承自基类的函数实体。这是在派生类决定不改写虚函数时才会出现的情况。
//  一个pure_virtual_called()函数实体，它既可以扮演pure virtual function的空间保卫者角色，也可以当做执行期异常处理函数。
// 每一个虚函数都别指派一个固定的索引值，这个索引值在整个继承体系中保持与特定的虚函数的关联。
class Point {
 public:
  virtual ~Point();                // 被赋值slot 1
  virtual Point &mult(float) = 0;  // slot 2，如果被调用，通常是结束程序
  float x() const { return _x; }         // 没有slot，因为是非虚函数
  virtual float y() const { return 0; }  // slot 3
  virtual float z() const { return 0; }  // slot 4

 protected:
  Point(float x = 0.0);
  float _x;

  // 内存布局：
  //  __vptr__Point
  //  _x
  // Virtual Table Point
  //  type_info for Point
  //  Point::~Point()
  //  pure_virtual_called()
  //  Point::y()
  //  Point::z()
};
class Point2d : public Point {
 public:
  Point2d(float x = 0.0, float y = 0.0) : Point(x), _y(y) {}
  ~Point2d();
  Point2d &mult(float);
  float y() const { return _y; }

 protected:
  float _y;

  // 内存布局：
  //  __vptr__Point
  //  _x
  //  _y
  // Virtual Table Point2d
  //  type_info for Point2d
  //  Point2d::~Point2d()
  //  Point2d::mult()
  //  Point2d::y()
  //  Point::z()

  // 当一个类派生时，有三种可能：
  //  1 可以继承基类所声明的虚函数实体。正确地说，是该函数实体会被拷贝到派生类的virtual table相对应的slot中。
  //  2 可以使用自己的函数实体。这表示它自己的函数实体地址必须放在对应的slot中。
  //  3 可以加入一个新的virtual function。这时候virtual table的尺寸会增加一个，而新的函数实体被被放进该slot中。
};
class Point3d : public Point2d {
 public:
  Point3d(float x = 0.0, float y = 0.0, float z = 0.0) : Point2d(x, y), _z(z) {}
  ~Point3d();
  Point3d &mult(float);
  float z() const { return _z; }

 protected:
  float _z;

  // 内存布局：
  //  __vptr__Point
  //  _x
  //  _y
  //  _z
  // Virtual Table Point3d
  //  type_info for Point3d
  //  Point3d::~Point3d()
  //  Point3d::mult()
  //  Point2d::y()
  //  Point3d::z()
};
// ptr->z();
// 一般而言，并不知道ptr所指对象的真正类型。然后知道经由ptr可以存取到该对象的virtual table。
// 虽然不知道哪一个z()函数实体被调用，但知道每一个z()函数地址都被放在slot 4。
// 这些信息使得编译可以将调用转换为：
// (*ptr->vptr[4])(ptr);
// 唯一不在编译期知道的是：slot 4所指的到底是哪一个z()函数实体。
// 在一个单一继承体系中，虚函数机制十分良好，不但有效率而且很容被塑造出来。但是多重继承和虚拟继承中，对虚函数的支持就没那么好了。

// 多重继承下的Virtual Functions
// 在多重继承中支持virtual functions，其复杂度围绕在第二个及后续的基类身上，以及“必须在执行期调整this指针”这一点。
class Base1 {
 public:
  Base1();
  virtual ~Base1();
  virtual void speakClearly();
  virtual Base1 *clone() const;

 protected:
  float data_Base1;
};
class Base2 {
 public:
  Base2();
  virtual ~Base2();
  virtual void mumble();
  virtual Base2 *clone() const;

 protected:
  float data_Base2;
};
class Derived : public Base1, public Base2 {
 public:
  Derived();
  virtual ~Derived();
  virtual Derived *clone() const;

 protected:
  float data_Derived;

  // Derived支持virtual functions的困难度，统统落在Base2 subobjcet身上。有三个问题需要解决：
  //  virtual destructor
  //  被继承下来的Base2::mumble
  //  一组clone函数实体
};
// Base2 *pbase2 = new Derived;
// 新的Derived对象的地址必须调整，以指向其Base2 subobject。编译期会产生以下代码：
// Derived *temp = new Derived;
// Base2 *pbase2 = temp ? temp + size(Base1) : 0;
// 如果没有这样的调整，指针的任何“非多态运用”都将失败：
// pbase2->data_Base2;
// 当程序员要删除pbase2所指的对象时：delete pbase2;指针必须被再一次调整，以求再一次指向Dervied对象的起始处。
// 然后上述的offset加法却不能够在编译期直接设定，因为pbase2所指的真正对象只有在执行期才能确定。
// 一般规则是，经由指向“第二或后继的基类”的指针（或引用）来调用派生类虚函数。
// 调用操作所连带的“必要的this指针调整”操作，必须在执行期完成。
// 也就是说，offset的大小，以及把offset加到this指针上头的那一小段代码，必须有编译器插入到某个地方。问题是在哪个地方？
// 比较有效率的解决方法是理由所谓的thunk。是一小段assembly码，用来以适当的offset值调整this指针和跳到virtual function去。
// pbbase2_dtor_thunk:
//   this += sizeof(base1);
//   Derived::~Derived(this);
// thunk技术允许virtual table slot继续内含一个简单的指针，因此多重继承不需要任何空间上的额外负担。
// slot中的地址可以指向virtual function，也可以指向一个相关的thunk（如果需要调整this指针的话）。
// 因此对于那些不需要调整this指针的virtual function而言，也就不需承载效率上的额外负担。
// 调整this指针的第二个额外负担就是，由于两种不同的可能：
//  1 经由派生类（或第一个基类）调用
//  2 经由第二个（或其后继）基类调用，同一函数在virtual table中可能需要多笔对应的slots。
//    Base1 *pbase1 = new Derived;
//    Base2 *pbase2 = new Derived;
//    delete pbase1;
//    delete pbase2;
//    虽然两个delete操作导致调用相同的派生类析构函数，但它们需要两个不同的vitrual table slots:
//    pbase1不需要调整this指针，因为Base1是最左端的base class，已经指向了Derived对象的起始处。vitrual table slot放置的是真正的destructor地址。
//    pbase2需要调整this指针。其vitrual table slot需要相关的thunk地址。

int main() { return 0; }