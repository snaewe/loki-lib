#include <iostream>
#include "Visitor.h"

using namespace ::Loki;

class RasterBitmap 
    : public BaseVisitable<>
{
public:
    DEFINE_VISITABLE()
};

class Paragraph
    : public BaseVisitable<>
{
public:
    DEFINE_VISITABLE()
};


class ParagraphEx 
    : public Paragraph
{
public:
    DEFINE_VISITABLE()
};

class DummyParagraph
    : public Paragraph
{
public:
    DEFINE_VISITABLE()
};

class DummyRasterBitmap
    : public RasterBitmap
{
public:
    DEFINE_VISITABLE()
};

class SomeVisitor 
    : public BaseVisitor
    , public Visitor<TYPELIST_3(RasterBitmap, Paragraph, ParagraphEx)>
    , public BaseVisitorImpl<TYPELIST_2(DummyParagraph, DummyRasterBitmap)>
{
public:
     void Visit(RasterBitmap&)
     {
         std::cout << "SomeVisitor::Visit(RasterBitmap&)" << '\n';
     }

     void Visit(Paragraph &)
     {
         std::cout << "SomeVisitor::Visit(Paragraph&)" << '\n';
     }

     void Visit(ParagraphEx &)
     {
         std::cout << "SomeVisitor::Visit(ParagraphEx&)" << '\n';
     }
};


void Test_Visitor()
{
    SomeVisitor     someVisit;
    BaseVisitor    &baseVisit = someVisit;

    RasterBitmap    rast;
    rast.Accept(baseVisit);
    
    Paragraph       par;
    par.Accept(baseVisit);

    ParagraphEx     parEx;
    parEx.Accept(baseVisit);

    DummyRasterBitmap dummyRast;
    dummyRast.Accept(baseVisit);

    DummyParagraph    dummyPar;
    dummyPar.Accept(baseVisit);
}

typedef CyclicVisitor
<
    void,
    TYPELIST_3(class DocElement, class RasterBitmap2, class Paragraph2)
>
MyCyclicVisitor;


class DocElement
{
public:
    virtual void Accept(MyCyclicVisitor &) = 0;
    virtual ~DocElement() {}
};

class RasterBitmap2
    : public DocElement
{
public:
    DEFINE_CYCLIC_VISITABLE(MyCyclicVisitor)
};

class Paragraph2 
    : public DocElement
{
public:
    DEFINE_CYCLIC_VISITABLE(MyCyclicVisitor)
};

class SomeVisitor2
    : public MyCyclicVisitor
{
public:
     void Visit(DocElement &)
     {
         std::cout << "SomeVisitor::Visit(DocElement&)" << '\n';
     }

     void Visit(Paragraph2 &)
     {
         std::cout << "SomeVisitor::Visit(Paragraph2&)" << '\n';
     }

     void Visit(RasterBitmap2 &)
     {
         std::cout << "SomeVisitor::Visit(RasterBitmap2&)" << '\n';
     }
};

void Test_CyclicVisitor()
{
    SomeVisitor2     someVisit;
    MyCyclicVisitor  &baseVisit = someVisit;

    RasterBitmap2   rast;
    rast.Accept(baseVisit);
    
    Paragraph2      par;
    par.Accept(baseVisit);

    DocElement     &docElem1 = par;
    docElem1.Accept(baseVisit);

    DocElement     &docElem2 = rast;
    docElem2.Accept(baseVisit);
}

int main()
{
    Test_Visitor();
    Test_CyclicVisitor();
}

