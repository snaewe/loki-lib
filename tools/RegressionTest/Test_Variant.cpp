#if defined(_MSC_VER) && _MSC_VER >= 1300
#pragma warning (disable : 4503 4244)
#endif

// 4503:  decorated name length exceeded, name was truncated
// 4244: 'argument' : conversion from '<tyep1>' to '<type2>', possible loss of data

#include <string>
#include <cassert>
#include <iostream>
#include "Variant.h"

using namespace std;

int main()
{
    try 
    {
        typedef Variant<TYPELIST_3(string, int, double)> DBField;
                
        DBField fld1(string("Hello, world"));
        DBField fld2(25);
        DBField fld3(3.14);
    
        assert(fld1.TypeId() == typeid(string));
        assert(fld2.TypeId() == typeid(int));
    
        string* p = fld1.GetPtr<string>();
        assert(p != 0);
        *p += '!';
        assert(fld1.Get<std::string>() == "Hello, world!");

        fld3 = fld1;
        assert(fld1.Get<std::string>() == "Hello, world!");
        assert(fld3.Get<std::string>() == "Hello, world!");

        fld3 = std::string("Assignment");
        assert(fld3.Get<std::string>() == "Assignment");

        DBField fld5;

        assert(fld5.TypeId() == typeid(string));
        assert(fld5.GetPtr<string>()->empty());

        DBField fld4 = fld1;
        assert(fld4.TypeId() == typeid(string));
        assert(fld4.Get<std::string>() == "Hello, world!");

        DBField fld(45);
        float f = fld.ConvertTo<float>();
        assert(f == 45);
        (void)f;

        fld2.ChangeType<double>();
        assert(fld2.Get<double>() == 25);

        typedef Variant<TYPELIST_3(string, int, float)> AlternateDBField;    
        AlternateDBField fld6(fld1);
        assert(fld6.Get<std::string>() == "Hello, world!");
        
        fld6 = fld3;
        assert(fld6.Get<std::string>() == "Assignment");
    
#if defined(_MSC_VER) && _MSC_VER >= 1300
        //
        // Variant with VC non-standard alignment support
        //
        typedef TYPELIST_3(string, double, int) DBField2Typelist;
        typedef Variant<DBField2Typelist, VC_AlignedPOD<DBField2Typelist> > AlternateDBField2;

        AlternateDBField2 fld7(fld6);
        assert(fld7.Get<std::string>() == "Assignment");
        
        fld7 = fld2;
        assert(fld7.Get<double>() == 25.0);

        enum {
            OrigAlign = __alignof(fld1),
            VCExAlign = __alignof(fld7)
        };

        STATIC_CHECK(OrigAlign == VCExAlign, WrongAlignment);

#endif // def _MSC_VER
    }
    catch (const std::exception& e)
    {
        cout << "Something weird happened: " << e.what();
    }
    catch (...)
    {
        cout << "Something really weird happened.";
    }
}

