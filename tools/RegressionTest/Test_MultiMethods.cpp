#include <iostream>
#include <typeinfo>
#include <memory>
#include <cassert>
#include "MultiMethods.h"

using ::std::cout;

struct Shape 
{
    virtual ~Shape() = 0;
};

Shape::~Shape() {}

class Rectangle : public Shape {};
class Poly      : public Shape {};
class Ellipse   : public Shape {};
class Cycloid   : public Shape {};

class SymHachingExecutor
{
public:
    typedef void ResultType;

    ResultType Fire(Rectangle&, Rectangle&)
    { cout << "Fire(Rectangle&, Rectangle&)" << '\n'; }

    ResultType Fire(Rectangle&, Poly&)
    { cout << "Fire(Rectangle&, Poly&)" << '\n'; }

    ResultType Fire(Rectangle&, Ellipse&)
    { cout << "Fire(Rectangle&, Ellipse&)" << '\n'; }

    ResultType Fire(Poly&, Poly&)
    { cout << "Fire(Poly&, Poly&)" << '\n'; }

    ResultType Fire(Poly&, Ellipse&)
    { cout << "Fire(Poly&, Ellipse&)" << '\n'; }
    
    ResultType Fire(Ellipse&, Ellipse&)
    { cout << "Fire(Ellipse&, Ellipse&)" << '\n'; }

    void OnError(Shape& x, Shape& y)
    { 
        cout << "OnError: " 
             << typeid(x).name() << ", "
             << typeid(y).name() << '\n';
    }
};

typedef ::Loki::StaticDispatcher
<
    SymHachingExecutor,
    Shape,
    TYPELIST_3(Rectangle, Poly, Ellipse)
>
SymHachingDispatcher;


class HachingExecutor : SymHachingExecutor
{
public:
    typedef SymHachingExecutor::ResultType ResultType;

    using SymHachingExecutor::Fire;

    ResultType Fire(Poly&, Rectangle&)
    { cout << "Fire(Poly&, Rectangle&)" << '\n'; }

    ResultType Fire(Ellipse&, Rectangle&)
    { cout << "Fire(Ellipse&, Rectangle&)" << '\n'; }

    ResultType Fire(Ellipse&, Poly&)
    { cout << "Fire(Ellipse&, Poly&)" << '\n'; }
    
    using SymHachingExecutor::OnError;
};

typedef ::Loki::StaticDispatcher
<
    HachingExecutor,
    Shape,
    TYPELIST_3(Rectangle, Poly, Ellipse),
    false
>
HachingDispatcher;

template<class TestedHachingDispatcher, class TestedHachingExecutor>
void Test_HachingDispatcher(TestedHachingExecutor Exec)
{
    using ::std::auto_ptr;

    auto_ptr<Shape>   shape1(new Rectangle);
    auto_ptr<Shape>   shape2(new Ellipse);
    auto_ptr<Shape>   shape3(new Poly);
    auto_ptr<Shape>   shape4(new Cycloid);

    static const std::size_t number_of_shapes = 4;
    
    Shape * const shapes[number_of_shapes] = 
    {
        shape1.get(), shape2.get(),
        shape3.get(), shape4.get(),
    };

    for (std::size_t i = 0 ; i < number_of_shapes ; ++i)
        for (std::size_t j = 0 ; j < number_of_shapes ; ++j)
        {
            TestedHachingDispatcher::Go(*shapes[i], *shapes[j], Exec);
        }
}

typedef ::Loki::BasicDispatcher<Shape>   ShapeBasicDispatcher;
typedef ::Loki::FnDispatcher<Shape>      ShapeFnDispatcher;
typedef ::Loki::FunctorDispatcher<Shape> ShapeFunctorDispatcher;
            
namespace // anonymous
{
    template<class Shape1, class Shape2>
    void HatchShapesDo(Shape1 &, Shape2 &)
    {
        cout << "HatchShapes: " 
             << typeid(Shape1).name() << ", "
             << typeid(Shape2).name() << '\n';
    }

    template<class Shape1, class Shape2>
    void HatchShapes(Shape &lhs, Shape &rhs)
    {
        assert(dynamic_cast<Shape1*>(&lhs) != 0);
        assert(dynamic_cast<Shape2*>(&rhs) != 0);

        HatchShapesDo(static_cast<Shape1&>(lhs),
                      static_cast<Shape2&>(rhs));
    }

    template<class Shape1, class Shape2>
    struct HatchShapesFunctor
    {
        void operator()(Shape1 &lhs, Shape2 &rhs)
        {
            HatchShapesDo(lhs, rhs);
        }
    };

} // anonymous namespace

template<class Shape1, class Shape2>
void ShapeDispatcherAdd(ShapeBasicDispatcher &x)
{
    x.Add<Shape1, Shape2>(HatchShapes<Shape1, Shape2>);
}

template<class Shape1, class Shape2>
void ShapeDispatcherAdd(ShapeFnDispatcher &x)
{
    x.Add<Shape1, Shape2>(HatchShapes<Shape1, Shape2>);
}

template<class Shape1, class Shape2>
void ShapeDispatcherAdd(ShapeFunctorDispatcher &x)
{
    x.Add<Shape1, Shape2>(HatchShapesFunctor<Shape1, Shape2>());
}

template<class Dispacher>
void Test_LogDispacher()
{
    Dispacher    testedBasicDispatcher;

    ShapeDispatcherAdd<Rectangle, Poly>(testedBasicDispatcher);
    ShapeDispatcherAdd<Poly, Rectangle>(testedBasicDispatcher);
    ShapeDispatcherAdd<Rectangle, Ellipse>(testedBasicDispatcher);
    ShapeDispatcherAdd<Ellipse, Poly>(testedBasicDispatcher);
    ShapeDispatcherAdd<Poly, Poly>(testedBasicDispatcher);


    using ::std::auto_ptr;

    auto_ptr<Shape>   shape1(new Rectangle);
    auto_ptr<Shape>   shape2(new Ellipse);
    auto_ptr<Shape>   shape3(new Poly);

    static const std::size_t number_of_shapes = 3;
    
    Shape * const shapes[number_of_shapes] = 
    {
        shape1.get(), shape2.get(), shape3.get(),
    };

    for (std::size_t i = 0 ; i < number_of_shapes ; ++i)
        for (std::size_t j = 0 ; j < number_of_shapes ; ++j)
        {
            try
            {
                testedBasicDispatcher.Go(*shapes[i], *shapes[j]);
            }
            catch(::std::runtime_error &exp)
            {
                cout << exp.what() << ": "
                     << typeid(*shapes[i]).name() << ", "
                     << typeid(*shapes[j]).name() << '\n';
            }
        }
}

int main()
{
    SymHachingExecutor  SymExec;
    Test_HachingDispatcher<SymHachingDispatcher>(SymExec);

    HachingExecutor     Exec;
    Test_HachingDispatcher<HachingDispatcher>(Exec);

    Test_LogDispacher<ShapeBasicDispatcher>();
    Test_LogDispacher<ShapeFnDispatcher>();
    Test_LogDispacher<ShapeFunctorDispatcher>();
}

