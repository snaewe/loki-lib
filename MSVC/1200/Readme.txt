Loki VC 6.0 Port or how to produce C1001 - Internal Compiler Errors
-------------------------------------------------------------------
Version: 0.3a

Introduction/Compatibility:
---------------------------
This is a partial MSVC 6.0 Sp5 compatible port of Andrei Alexandrescu's excellent Loki Library.
Because I could not retain the originial interface in all places, this port is not
compatible to the original library and therefore code using this port *cannot* generally be
used together with the original lib.
This is, of course, a great pity.
So if you know of a complete and full interface-compatible VC 6.0
port or if you know how to improve this port, please let me know.

Contact:
--------
For any suggestions, bug reports, comments and questions please email me to
Hume@c-plusplus.de

Using this port:
----------------
To use this port, simply extract the files from the archive, give your compiler access to
their path, and include them appropriately in your code via #include.

If you use the small object allocator directly or indirectly (through the Functor class)
you must add SmallObj.cpp to your project/makefile.

If you use Singletons with longevity you must add Singleton.cpp to your project/makefile.

Fixes:
------

    Jan 12, 2003:
    -------------
    	* changed the signature of SmallObject's op new. Now it
    	matches the corresponding op delete.
		Thanks to M.Yamada for the hint and the solution.

    Dec 08, 2002:
    -------------
        * In HierarchyGenerators.h: Sergey Khachatrian reported a bug
        in GenScatterHierarchy when used with a typelist containing
        equal types (e.g. GenScatterHierarchy<TYPELIST_2(int, int), UnitWrapper>
        resp. Tuple<TYPELIST_2(int, int)>)
        Fixing the bug I found another MSVC6-Problem in the Field-function.
        The workaround for this problems results in an interface change.

        please refer to the section "Interface changes" below for further information.

    Dec 03, 2002
    -------------
        * In MSVC6Helpers.h: The original version failed to qualify some types from the
        Private-Namespace.
        Thanks to Adi Shavit for pointing that out

        * In Threads.h: Changed wrong ctor/dtor names in ObjectLevelLockable.
        Thanks to Adi Shavit for pointing that out

    Nov 19, 2002:
    -------------
        * In SmartPtr.h: Changed template ctors. See Notes.

Notes:
------
The original Loki Lib uses some pretty advanced (resp. new) C++ features like:

A. partial template specialization.
B. template template parameters.
C. explicit template argument specification for member- and nonmeber functions.
D. covariant return types.
E. Template parameters with a default type void
F. return statements with an expression of type cv in functions with a return type of cv void.

Unfortunately the MSVC 6.0 supports neither of them.

    A. I used various techniques to simulate partial template specialization. In some cases
    these techniques allowed me to retain the original interfaces but often that was not
    possible (or better: i did not find a proper solution). In any case it leads
    to increasing code complexity :-)

    B. One way to simulate template template parameters is to replace the template class with
    a normal class containing a nested template class. You then move the original functionality
    to the nested class.
    The problem with this approach is MSVC's 'dependent template typedef bug'. MSVC 6.0 does not
    allow something like this:

    [code]
    template <class APolicy, class T>
    struct Foo
    {
        // 'error C1001 - Internal Compiler Error' here
        typedef typename APolicy::template In<T> type;
    };

    [/code]

    To make a long story short, I finally decided to use boost::mpl's apply-technique to
    simulate template template parameters. This approach works fine with MSVC 6.0. But be warned,
    this technique uses not valid C++.
    Of course, replacing template template parameters always results in some interface changes.

    C. I added dummy-Parameters to (Member-)Functions that depend on explicit template
    argument specification. These dummy-Parameters help the compiler in deducing the template
    parameters that otherwise need to be explicitly specified.
    Example:
    [code]
    struct Foo
    {
        template <class T>
        T Func();
    };
    [/code]
    becomes
    [code]
    struct Foo
    {
        template <class T>
        T Func(T* pDummy1);
    };
    [/code]
    in this port.

    Update:
    -------
    The MSVC 6.0 sometimes does not overload normal functions depending
    on explicit argument specification correctly (see: Microsoft KB Article - 240871)
    The following code demonstrates the problem:
    [code]
    template <unsigned i, class T>
    void BugDemonstration(T p)
    {
	    printf("BugDemonstration called with i = %d\n", i);
    }

    int main()
    {
        GenScatterHierarchy<TYPELIST_3(int, int, int), TestUnitWrapper> Bla;
	    // will always print: "BugDemonstration called with i = 2";
	    BugDemonstration<0>(Bla);
	    BugDemonstration<1>(Bla);
	    BugDemonstration<2>(Bla);
    }
    [/code]

    As a workaround i added dummy-parameters.

    D. Virtual functions that use covariant return types (e.g. return a pointer to Derived)
    in the original library were changed so that they have exactly the
    same return type as the original virtual function (e.g. return a pointer to Base).

    E. All template parameters that have a default type of void in the original lib now
    have int as default type.

    F. In Functor.h I changed a ResultType of type void to VoidAsType (an udt). This change is transparent
    for the user of Functor.
    Because I could not think of any general and transparent workaround I followed different
    strategies. In Visitor.h for example I created new classes (and macros) for the void-case.
    In other places (for example: MultiMethod.h) this port simply fails to support void as
    return type :-(

Some words to template-ctors resp. template assignment operators:
The MSVC 6.0 introduces an order-dependency for template ctor
resp. template assignemt operators.
If you need both a copy-ctor and a template copy ctor (same for copy-assignment), then
you *must* write the templated version first.
So instead of
[code]
template <class T>
struct Foo
{
    Foo(const Foo&)
    {}
    template <class U>
    Foo(const Foo<U>& r)
    {}
};
[/code]
you *need* to write:
[code]
template <class T>
struct Foo
{
    template <class U>
    Foo(const Foo<U>& r)
    {}

    Foo(const Foo& r)
    {}
};
[/code]

Many thanks to Nelson Elói for pointing that out and for providing me
with this solution.

The above solution unfortunately does not work if the template ctor does not have
the form of a copy-ctor. If you write something like this (as in the functor-class):
[code]
template <class T>
struct Foo
{
    template <class Fun>
    Foo(Fun r)
    {}

    Foo(const Foo& r)
    {}
};
[/code]
then the VC will no longer find a copy-ctor.

Because of this, i can't use Nelson Elói's solution in Functor.h

Interface changes:
------------------
1. In Threads.h:

    *   Thread-Policies changed from class templates to normal classes containing a
    nested class template 'In'.

    consequences:
    This change is not very dramatic because it won't break code using this port when
    switching to the original library (only new Thread-Policies must be changed)

2. In Singleton.h:

    *   The Creation- and Lifetime-Policies are no longer class templates. Instead they all use
    Member-Templates.

    consequences:
    Again this change will only break new Policies when switching to the
    original library.

3. In Functor.h:

    *   No covariant return types.

    consequences:
    DoClone always returns a FunctorImplBase<R, ThreadingModel>* where R is the functor's return
    type and  ThreadingModel its current ThreadingModel.

4. TypeTraits.h

    *   Because VC 6.0 lacks partial template specialization, the TypeTraits-Class provides not
    all the stuff provided by the original library's version.


5. HierarchyGenerator.h

    *   I used Mat Marcus' approach to port GenScatterHierarchy.
    See http://lists.boost.org/MailArchives/boost/msg20915.php) for the consequences.

    *   Same for GenLinearHierarchy

    *   Unit is no longer a template template parameter.

    consequences:
    For every concrete unit-template there must be a normal class containing
    a nested-template class called 'In'. 'In' should only contain a typedef to the
    concrete Unit.

    Update:
    The port's original version of GenScatterHierarchy does not work when used
    with typelists containing equal types.
    The problem is due to a VC bug. The VC fails to compile code similar
    to this, although it is perfectly legal.
    [code]
    template <class T>
    class Wrapper
    {};

    template <class T>
    struct B : public Wrapper<T>
    {};

    // ERROR: 'A<T>' : direct base 'Wrapper<T>' is inaccessible; already a base of 'B<T>'
    template <class T>
    class A : public B<T>, public Wrapper<T>
    {};
    [/code]

    Unfortunately my workaround has a big drawback.
    GenScatterHierarchy now has to generate a lot more classes.
    Alexandrescu's original implementation generates 3*n classes (n - number of types in the typelist)
    The old version of my port creates 4 * n + 1
    The new version will create 5 * n

    The fix also reveals the "Explicitly Specified Template Functions Not Overloaded Correctly"-Bug
    (Microsoft KB Article - 240871) in the Field-Function taking a nontype int Parameter.

    This leads to an interface change:
    Instead of: Field<0>(obj)
    one now has to write
    Field(obj, Int2Type<0>());

    I added a macro FIELD. Using this macro one can write
    FIELD(obj, 0)


6. Factory.h

    *   The Error-Policy for Factory and CloneFactory is no longer a template template parameter.
    Use a class with member-templates instead.

    consequences:
    This change will only break new Policies when switching to the
    original library.

7. AbstractFactory.h

    *   no covariant return types

    *   no template template parameters
    For every concrete Factory-Unit there must be a normal class containing
    a nested-template class called 'In'. 'In' shall contain a typedef to the
    concrete Factory-Unit.

    *   Added a dummy-Parameter to AbstractFactory::Create (see C.)
    Calling syntax changed from:
        ConcProduct* p = aFactory.Create<ConcProduct>();
    to
        ConcProduct* p = aFactory.Create((ConcProduct*)0);


8. SmartPtr.h

   * no template template parameters.
   (see 7.for a description of the consequences)

   * This port does not specialize std::less

9. Visitor.h

    * no template template parameters
    (see 7.for a description of the consequences)

    * This port fails to correctly support void return types. As a workaround it provides
    a set of complete new classes (and macros) for void. Default arguments of type void
    were replaced by arguments of type int.


10. MultiMethods.h

    * replaced all template template parameters with 'normal' parameters (see 7.
    for a description of the consequences)

    * This port does not support functions with return type void.

    * dummy parameters were added to functions that otherwise would depend on
    explicit template argument specification (14.8.1).


More info:
----------
The original Loki library can be found here: http://moderncppdesign.com
For Rani Sharoni's VC 7.0 port see: http://www.geocities.com/rani_sharoni/LokiPort.html
