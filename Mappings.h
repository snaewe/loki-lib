#ifndef MAPPINGS_H_
#define MAPPINGS_H_

namespace Loki
{
    template <int v>
    struct Int2Type
    {
        enum { value = v };
    };
    
    template <typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };
    
    template <bool flag, typename T, typename U>
    struct Select
    {
        typedef T Result;
    };
    template <typename T, typename U>
    struct Select<false, T, U>
    {
        typedef U Result;
    };

}

#endif