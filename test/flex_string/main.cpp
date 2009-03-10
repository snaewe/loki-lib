////////////////////////////////////////////////////////////////////////////////
// flex_string
// Copyright (c) 2001 by Andrei Alexandrescu
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


#ifdef _MSC_VER

#pragma warning (disable : 4786)    // Get rid of browser information too long names
#ifdef _DEBUG
#pragma warning (disable : 4786)
#endif

#endif

#include <stdio.h>
#include <list>

#ifndef __MWERKS__
//#define NO_ALLOCATOR_REBIND
//#define NO_ITERATOR_TRAITS
#endif

#include <loki/flex/flex_string.h>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <utility>
#include <vector>


namespace StringsToTest
{
  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    SimpleStringStorage<char, std::allocator<char> >
  > my_string_SimpleStorage;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    AllocatorStringStorage<char, std::allocator<char> >
  > my_string_AllocatorStorage;

  typedef flex_string<
    char,
    std::char_traits<char>,
    mallocator<char>,
    AllocatorStringStorage<char, mallocator<char> >
  > my_string_MallocatorStorage;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    VectorStringStorage<char, std::allocator<char> >
  > my_string_VectorStorage;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    SmallStringOpt<SimpleStringStorage<char, std::allocator<char> >, 31>
  > my_string_SmallStringSimple;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    SmallStringOpt<SimpleStringStorage<char, std::allocator<char> >, 126>
  > my_string_SmallStringSimpleBigBuffer;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    SmallStringOpt<VectorStringStorage<char, std::allocator<char> >, 23>
  > my_string_SmallStringVector;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    CowStringOpt<SimpleStringStorage<char, std::allocator<char> > >
  > my_string_CowSimple;

  typedef flex_string<
    char,
    std::char_traits<char>,
    std::allocator<char>,
    CowStringOpt<AllocatorStringStorage<char, std::allocator<char> > >
  > my_string_CowAllocator;
}


template<class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up)
{
  Integral2 low2(low);
  assert(up >= low2);
  if (low2 == up) return low;
  Integral2 x = Integral2(low2 + (rand() * (up - low2)) / RAND_MAX);
  assert(x >= low2 && x <= up);
  return x;
}

template<class String>
String RandomString(size_t maxSize)
{
  const typename String::size_type size = random(0, maxSize);
  String result(size, '\0');
  size_t i = 0;
  for (; i != size; ++i)
  {
    result[i] = random('a', 'z');
  }
  return result;
}

// Specialize these method for different String types.
template<class String>
typename String::value_type NullTerminator()
{
  return '\0';
}

template<class String>
String LengthErrorExceptionString()
{
  return "length_error exception thrown";
}

template<class String>
String OutOfRangeExceptionString()
{
  return "out_of_range exception thrown";
}

template<class String>
String UnknownExceptionString()
{
  return "Unknown exception thrown";
}

template<class String>
String NposString()
{
  return "{npos}";
}

template<class String>
String OpenCurlyString()
{
  return "{";
}

template<class String>
String CloseCurlyString()
{
  return "}";
}

template<class String>
String SeparatorString()
{
  return "--";
}

template<class String>
String TrueString()
{
  return "{true}";
}

template<class String>
String FalseString()
{
  return "{false}";
}

template<class String, class Integral>
String Num2String(Integral value)
{
  typedef typename String::value_type CharType;
  std::basic_ostringstream<CharType, std::char_traits<CharType>, std::allocator<CharType> > stream;
  stream << OpenCurlyString<String>() << value << CloseCurlyString<String>();
  return stream.str().c_str();
}

template<class String>
String Num2String(typename String::size_type value)
{
  if(String::npos != value)
  {
    return Num2String<String, typename String::size_type>(value);
  }
  else
  {
    // Not all strings will have the same value for npos.
    // Since methods like find return npos on failure we want to represent npos in an implementation-independent manner.
    return NposString<String>();
  }
}

template<class String>
String Num2String(bool value)
{
  return value ? TrueString<String>() : FalseString<String>();
}

// Some comparison functions return 0 or a value greater/smaller than zero.
// This function makes the greater/smaller than zero specification implementation-independent.
template<class String>
String Tristate2String(int tristate)
{
  if(0 == tristate)
    return Num2String<String>(0);
  else if(0 < tristate)
    return Num2String<String>(1);
  else
    return Num2String<String>(2);
}

template<class String>
std::list<typename String::value_type> RandomList(typename String::size_type maxSize)
{
  const typename String::size_type size = random(0, maxSize);
  std::list<typename String::value_type> list(size);
  std::list<typename String::value_type>::iterator i = list.begin();
  for (; i != list.end(); ++i)
  {
    *i = random('a', 'z');
  }
  return list;
}

namespace Tests
{
  template<class String>
  struct MaxString
  {
    static const typename String::size_type value = 1050;
  };

  template<class String>
  String operator_plus_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = random1 + random2;
    return result;
  }

  template<class String>
  String operator_plus_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = random1.c_str() + random2;
    return result;
  }

  template<class String>
  String operator_plus_char_string()
  {
    // 21.3
    const typename String::value_type value = random('a', 'z');
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result = value + random1;
    return result;
  }

  template<class String>
  String operator_plus_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = random1 + random2.c_str();
    return result;
  }

  template<class String>
  String operator_plus_string_char()
  {
    // 21.3
    const typename String::value_type value = random('a', 'z');
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result = random1 + value;
    return result;
  }

  template<class String>
  String operator_equal_equal_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 == random2);
    return result;
  }

  template<class String>
  String operator_equal_equal_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() == random2);
    return result;
  }

  template<class String>
  String operator_equal_equal_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 == random2.c_str());
    return result;
  }

  template<class String>
  String operator_not_equal_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 != random2);
    return result;
  }

  template<class String>
  String operator_not_equal_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() != random2);
    return result;
  }

  template<class String>
  String operator_not_equal_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 != random2.c_str());
    return result;
  }

  template<class String>
  String operator_smaller_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 < random2);
    return result;
  }

  template<class String>
  String operator_smaller_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() < random2);
    return result;
  }

  template<class String>
  String operator_smaller_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 < random2.c_str());
    return result;
  }

  template<class String>
  String operator_greater_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 > random2);
    return result;
  }

  template<class String>
  String operator_greater_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() > random2);
    return result;
  }

  template<class String>
  String operator_greater_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 > random2.c_str());
    return result;
  }

  template<class String>
  String operator_smaller_equal_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 <= random2);
    return result;
  }

  template<class String>
  String operator_smaller_equal_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() <= random2);
    return result;
  }

  template<class String>
  String operator_smaller_equal_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 <= random2.c_str());
    return result;
  }

  template<class String>
  String operator_greater_equal_string_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 >= random2);
    return result;
  }

  template<class String>
  String operator_greater_equal_cstr_string()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1.c_str() >= random2);
    return result;
  }

  template<class String>
  String operator_greater_equal_string_cstr()
  {
    // 21.3
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Num2String<String>(random1 >= random2.c_str());
    return result;
  }

  template<class String>
  String swap_string_string()
  {
    using std::swap;
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    swap(random1, random2);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String swap_string_string__self()
  {
    using std::swap;
    String random1(RandomString<String>(MaxString<String>::value));
    swap(random1, random1);
    return random1;
  }

  template<class String>
  String swap_string_string__selfcopy()
  {
    using std::swap;
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    swap(random1, copy);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String swap_string_string__selfcopy2()
  {
    using std::swap;
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    swap(copy, random1);
    return random1 + SeparatorString<String>() + copy;
  }

  // TODO: operator>>(istream, string)
  // TODO: operator<<(ostream, string)
  // TODO: getline(istream, string, char delimiter)
  // TODO: getline(istream, string)

  template<class String>
  String constructor()
  {
    // 21.3.1
    return String();
  }

  template<class String>
  String constructor_string()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    return random1;
  }

  template<class String>
  String constructor_string_position_number()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, random1.size() - position);
    String result(random1, position, number);
    return result;
  }

  template<class String>
  String constructor_string_position_number__bigNumber()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, String::npos);
    String result(random1, position, number);
    return result;
  }

  template<class String>
  String constructor_cstr_number()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, random1.size());
    String result(random1.c_str(), number);
    return result;
  }

  template<class String>
  String constructor_number_char()
  {
    // 21.3.1
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::size_type value = random('a', 'z');
    String result(number, value);
    return result;
  }

  template<class String>
  String constructor_iterator_iterator()
  {
    // 21.3.5
    std::list<typename String::value_type> list(RandomList<String>(MaxString<String>::value));
    String result(list.begin(), list.end());
    return result;
  }

  // TODO: destructor

  template<class String>
  String operator_equal_string()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1 = random2;
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String operator_equal_string__self()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    random1 = random1;
    return random1;
  }

  template<class String>
  String operator_equal_string__selfCopy()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(random1);
    random1 = random2;
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String operator_equal_cstr()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1 = random2.c_str();
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String operator_equal_cstr__self()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    random1 = random1.c_str();
    return random1;
  }

  template<class String>
  String operator_equal_cstr__selfCopy()
  {
    // 21.3.1
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(random1);
    random1 = random2.c_str();
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String iterators()
  {
    // 21.3.2
    String random1(RandomString<String>(MaxString<String>::value));
    const String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result += Num2String<String>(random1.end() - random1.begin())
           +  Num2String<String>(random1.rend() - random1.rbegin())
           +  Num2String<String>(random2.end() - random2.begin())
           +  Num2String<String>(random2.rend() - random2.rbegin())
           +  Num2String<String>(random1.end() == random1.begin())
           +  Num2String<String>(random1.rend() == random1.rbegin())
           +  Num2String<String>(random2.end() == random2.begin())
           +  Num2String<String>(random2.rend() == random2.rbegin())
           +  Num2String<String>(random1.end() != random1.begin())
           +  Num2String<String>(random1.rend() != random1.rbegin())
           +  Num2String<String>(random2.end() != random2.begin())
           +  Num2String<String>(random2.rend() != random2.rbegin());
    return result;
  }

  template<class String>
  String capacity()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result += Num2String<String>(random1.size())
           +  Num2String<String>(random1.length())
           +  Num2String<String>(random1.size() == random1.length())
           +  Num2String<String>(random1.capacity() >= random1.size())
           +  Num2String<String>(random1.max_size() >= random1.size())
           +  Num2String<String>(random1.max_size() >= random1.capacity());
    return result;
  }

  template<class String>
  String resize_number_char()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.resize(number, value);
    return random1;
  }

  template<class String>
  String resize_number()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.resize(number);
    return random1;
  }

  template<class String>
  String reserve_number()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.reserve(number);
    random1 += Num2String<String>(random1.capacity() >= number);
    return random1;
  }

  template<class String>
  String clear()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    random1.clear();
    return random1;
  }

  template<class String>
  String empty()
  {
    // 21.3.3
    String random1(RandomString<String>(MaxString<String>::value));
    return Num2String<String>(random1.empty());
  }

  template<class String>
  String element_access()
  {
    // 21.3.4
    String random1(RandomString<String>(MaxString<String>::value));
    const String random2(RandomString<String>(MaxString<String>::value));
    String result;
    if(!random1.empty())
    {
      {
        const typename String::size_type index = random(0, random1.size() - 1);
        result += random1[index] + SeparatorString<String>();
      }
      {
        const typename String::size_type index = random(0, random1.size() - 1);
        result += random1.at(index) + SeparatorString<String>();
      }
    }
    if(!random2.empty())
    {
      {
        const typename String::size_type index = random(0, random2.size() - 1);
        result += random2[index] + SeparatorString<String>();
      }
      {
        const typename String::size_type index = random(0, random2.size() - 1);
        result += random2.at(index) + SeparatorString<String>();
      }
    }
    return result;
  }

  template<class String>
  String operator_plus_equal_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1 += random2;
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String operator_plus_equal_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1 += random1;
    return random1;
  }

  template<class String>
  String operator_plus_equal_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1 += copy;
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String operator_plus_equal_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1 += random2.c_str();
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String operator_plus_equal_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1 += random1.c_str();
    return random1;
  }

  template<class String>
  String operator_plus_equal_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1 += copy.c_str();
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String operator_plus_equal_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    random1 += value;
    return random1;
  }

  template<class String>
  String append_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1.append(random2);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String append_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.append(random1);
    return random1;
  }

  template<class String>
  String append_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.append(copy);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String append_string_position_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random2.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.append(random2, position, number);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String append_string_position_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.append(random1, position, number);
    return random1;
  }

  template<class String>
  String append_string_position_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.append(copy, position, number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String append_cstr_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, random2.size());
    random1.append(random2.c_str(), number);
    return random1;
  }

  template<class String>
  String append_cstr_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, random1.size());
    random1.append(random1.c_str(), number);
    return random1;
  }

  template<class String>
  String append_cstr_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type number = random(0, random1.size());
    random1.append(copy.c_str(), number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String append_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1.append(random2.c_str());
    return random1;
  }

  template<class String>
  String append_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.append(random1.c_str());
    return random1;
  }

  template<class String>
  String append_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.append(copy.c_str());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String append_number_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.append(number, value);
    return random1;
  }

  template<class String>
  String append_iterator_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    std::list<typename String::value_type> list(RandomList<String>(MaxString<String>::value));
    random1.append(list.begin(), list.end());
    return random1;
  }

  template<class String>
  String append_iterator_iterator__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.append(random1.begin(), random1.end());
    return random1;
  }

  template<class String>
  String append_iterator_iterator__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.append(copy.begin(), copy.end());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String append_iterator_iterator__self_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.append(random1.rbegin(), random1.rend());
    return random1;
  }

  template<class String>
  String append_iterator_iterator__selfcopy_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.append(copy.rbegin(), copy.rend());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String push_back_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    random1.push_back(value);
    return random1;
  }

  template<class String>
  String assign_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1.assign(random2);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String assign_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.assign(random1);
    return random1;
  }

  template<class String>
  String assign_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.assign(copy);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String assign_string_position_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random2.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.assign(random2, position, number);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String assign_string_position_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.assign(random1, position, number);
    return random1;
  }

  template<class String>
  String assign_string_position_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.assign(copy, position, number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String assign_cstr_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, random2.size());
    random1.assign(random2.c_str(), number);
    return random1;
  }

  template<class String>
  String assign_cstr_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, random1.size());
    random1.assign(random1.c_str(), number);
    return random1;
  }

  template<class String>
  String assign_cstr_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String copy(random1);
    const typename String::size_type number = random(0, copy.size());
    random1.assign(copy.c_str(), number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String assign_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String random2(RandomString<String>(MaxString<String>::value));
    random1.assign(random2.c_str());
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String assign_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.assign(random1.c_str());
    return random1;
  }

  template<class String>
  String assign_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const String copy(random1);
    random1.assign(copy.c_str());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String assign_number_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.assign(number, value);
    return random1;
  }

  template<class String>
  String assign_iterator_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    std::list<typename String::value_type> list(RandomList<String>(MaxString<String>::value));
    random1.assign(list.begin(), list.end());
    return random1;
  }

  template<class String>
  String assign_iterator_iterator__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.assign(random1.begin(), random1.end());
    return random1;
  }

  template<class String>
  String assign_iterator_iterator__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.assign(copy.begin(), copy.end());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String assign_iterator_iterator__self_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.assign(random1.rbegin(), random1.rend());
    return random1;
  }

  template<class String>
  String assign_iterator_iterator__selfcopy_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    random1.assign(copy.rbegin(), copy.rend());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String insert_position_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, random2);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String insert_position_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, random1);
    return random1;
  }

  template<class String>
  String insert_position_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, copy);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String insert_position_string_position_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(0, random2.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.insert(position1, random2, position2, number);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String insert_position_string_position_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.insert(position1, random1, position2, number);
    return random1;
  }

  template<class String>
  String insert_position_string_position_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(0, copy.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.insert(position1, copy, position2, number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String insert_position_cstr_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, random2.size());
    random1.insert(position, random2.c_str(), number);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String insert_position_cstr_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, random1.size());
    random1.insert(position, random1.c_str(), number);
    return random1;
  }

  template<class String>
  String insert_position_cstr_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, copy.size());
    random1.insert(position, copy.c_str(), number);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String insert_position_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, random2.c_str());
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String insert_position_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, random1.c_str());
    return random1;
  }

  template<class String>
  String insert_position_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    random1.insert(position, copy.c_str());
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String insert_iterator_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::value_type value = random('a', 'z');
    random1.insert(random1.begin() + position, value);
    return random1;
  }

  template<class String>
  String insert_position_number_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.insert(position, number, value);
    return random1;
  }

  template<class String>
  String insert_iterator_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    std::list<typename String::value_type> list(RandomList<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(random1.begin() + position, list.begin(), list.end());
    return random1;
  }

  template<class String>
  String insert_iterator_iterator__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(random1.begin() + position, random1.begin(), random1.end());
    return random1;
  }

  template<class String>
  String insert_iterator_iterator__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    random1.insert(random1.begin() + position, copy.begin(), copy.end());
    return random1;
  }

  template<class String>
  String insert_iterator_iterator__self_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    random1.insert(random1.begin() + position, random1.rbegin(), random1.rend());
    return random1;
  }

  template<class String>
  String insert_iterator_iterator__selfcopy_reverse()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    random1.insert(random1.begin() + position, copy.rbegin(), copy.rend());
    return random1;
  }

  template<class String>
  String erase_position_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type start = random(0, random1.size());
    const typename String::size_type end = random(0, MaxString<String>::value);
    random1.erase(start, end);
    return random1;
  }

  template<class String>
  String erase_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    if(!random1.empty())
    {
      const typename String::size_type position = random(0, random1.size());
      random1.erase(random1.begin() + position);
    }
    return random1;
  }

  template<class String>
  String erase_iterator_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    random1.erase(random1.begin() + position1, random1.begin() + position2);
    return random1;
  }

  template<class String>
  String replace_position_number_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.replace(position, number, random2);
    return random1;
  }

  template<class String>
  String replace_position_number_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.replace(position, number, random1);
    return random1;
  }

  template<class String>
  String replace_position_number_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    random1.replace(position, number, copy);
    return random1;
  }

  template<class String>
  String replace_position_number_string_position_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type position2 = random(0, random2.size());
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, random2, position2, number2);
    return random1;
  }

  template<class String>
  String replace_position_number_string_position_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type position2 = random(0, random1.size());
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, random1, position2, number2);
    return random1;
  }

  template<class String>
  String replace_position_number_string_position_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type position2 = random(0, random1.size());
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, copy, position2, number2);
    return random1;
  }

  template<class String>
  String replace_position_number_cstr_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type number2 = random(0, random2.size());
    random1.replace(position1, number1, random2.c_str(), number2);
    return random1;
  }

  template<class String>
  String replace_position_number_cstr_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type number2 = random(0, random1.size());
    random1.replace(position1, number1, random1.c_str(), number2);
    return random1;
  }

  template<class String>
  String replace_position_number_cstr_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type number2 = random(0, copy.size());
    random1.replace(position1, number1, copy.c_str(), number2);
    return random1;
  }

  template<class String>
  String replace_position_number_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, random2.c_str());
    return random1;
  }

  template<class String>
  String replace_position_number_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, random1.c_str());
    return random1;
  }

  template<class String>
  String replace_position_number_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    random1.replace(position1, number1, copy.c_str());
    return random1;
  }

  template<class String>
  String replace_position_number_number_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.replace(position1, number1, number2, value);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_string()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random2);
    else
      random1.replace(random1.begin(), random1.end(), random2);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_string__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random1);
    else
      random1.replace(random1.begin(), random1.end(), random1);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_string__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, copy);
    else
      random1.replace(random1.begin(), random1.end(), copy);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr_number()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type number = random(0, random2.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random2.c_str(), number);
    else
      random1.replace(random1.begin(), random1.end(), random2.c_str(), number);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr_number__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type number = random(0, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random1.c_str(), number);
    else
      random1.replace(random1.begin(), random1.end(), random1.c_str(), number);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr_number__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type number = random(0, copy.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, copy.c_str(), number);
    else
      random1.replace(random1.begin(), random1.end(), copy.c_str(), number);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random2.c_str());
    else
      random1.replace(random1.begin(), random1.end(), random2.c_str());
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, random1.c_str());
    else
      random1.replace(random1.begin(), random1.end(), random1.c_str());
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_cstr__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    if(!random1.empty())
      random1.replace(random1.begin() + position1, random1.begin() + position2, copy.c_str());
    else
      random1.replace(random1.begin(), random1.end(), copy.c_str());
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_number_char()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    random1.replace(random1.begin() + position1, random1.begin() + position2, number, value);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_iterator_iterator()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    std::list<typename String::value_type> list(RandomList<String>(MaxString<String>::value));
    random1.replace(random1.begin() + position1, random1.begin() + position2, list.begin(), list.end());
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_iterator_iterator__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type position3 = random(0, random1.size());
    const typename String::size_type position4 = random(position3, random1.size());
    random1.replace(random1.begin() + position1, random1.begin() + position2, random1.begin() + position3, random1.begin() + position4);
    return random1;
  }

  template<class String>
  String replace_iterator_iterator_iterator_iterator__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(random1);
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    const typename String::size_type position3 = random(0, copy.size());
    const typename String::size_type position4 = random(position3, copy.size());
    random1.replace(random1.begin() + position1, random1.begin() + position2, copy.begin() + position3, copy.begin() + position4);
    return random1;
  }

  template<class String>
  String copy_char_number_position()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String destination(random(2, MaxString<String>::value), '\0');
    const typename String::size_type position = random(0, random1.size());
    random1.copy(&destination[0], destination.size() - 1, position);
    return destination;
  }

  template<class String>
  String swap()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    random1.swap(random2);
    return random1 + SeparatorString<String>() + random2;
  }

  template<class String>
  String swap__self()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    random1.swap(random1);
    return random1;
  }

  template<class String>
  String swap__selfcopy()
  {
    // 21.3.5
    String random1(RandomString<String>(MaxString<String>::value));
    String copy(RandomString<String>(MaxString<String>::value));
    random1.swap(copy);
    return random1 + SeparatorString<String>() + copy;
  }

  template<class String>
  String string_operations()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result += Num2String<String>(random1.c_str() == random1.data());
    result += Num2String<String>(NullTerminator<String>() == *(random1.c_str() + random1.size()));
    result += Num2String<String>(random1.get_allocator() == result.get_allocator());
    return result;
  }

  template<class String>
  String find_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(searched, position3));
    return result;
  }

  template<class String>
  String find_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(random2, position));
    return result;
  }

  template<class String>
  String find_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String find_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String find_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String find_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(random2.c_str(), position));
    return result;
  }

  template<class String>
  String find_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find(value, position));
    return result;
  }

  template<class String>
  String rfind_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(searched, position3));
    return result;
  }

  template<class String>
  String rfind_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(random2, position));
    return result;
  }

  template<class String>
  String rfind_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String rfind_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String rfind_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String rfind_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(random2.c_str(), position));
    return result;
  }

  template<class String>
  String rfind_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.rfind(value, position));
    return result;
  }

  template<class String>
  String find_overflow()
  {
    // This tests bug 2536490.

    String testString;
    String result;

    // Validate that find with a single character doesn't overflow.
    result += Num2String<String>(testString.find('6'));
    testString = "12345";
    result += Num2String<String>(testString.find('6'));
    testString = "123456";
    result += Num2String<String>(testString.find('6'));
    testString = "12345";
    result += Num2String<String>(testString.find('6'));

    // Validate that find with multiple characters doesn't overflow.
    testString = "12345";
    result += Num2String<String>(testString.find("123"));
    testString = "12345";
    result += Num2String<String>(testString.find("12345"));
    testString = "12345";
    result += Num2String<String>(testString.find("345"));
    testString = "123456";
    result += Num2String<String>(testString.find("456"));
    testString = "12345";
    result += Num2String<String>(testString.find("456"));

    return result;
  }

  template<class String>
  String rfind_overflow()
  {
    String testString;
    String result;

    // Validate that rfind with a single character doesn't overflow.
    result += Num2String<String>(testString.rfind('6'));
    testString = "12345";
    result += Num2String<String>(testString.rfind('6'));
    testString = "123456";
    result += Num2String<String>(testString.rfind('6'));
    testString = "12345";
    result += Num2String<String>(testString.rfind('6'));

    // Validate that rfind with multiple characters doesn't overflow.
    testString = "12345";
    result += Num2String<String>(testString.rfind("123"));
    testString = "12345";
    result += Num2String<String>(testString.rfind("12345"));
    testString = "12345";
    result += Num2String<String>(testString.rfind("345"));
    testString = "123456";
    result += Num2String<String>(testString.rfind("456"));
    testString = "12345";
    result += Num2String<String>(testString.rfind("456"));

    return result;
  }

  template<class String>
  String find_first_of_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(searched, position3));
    return result;
  }

  template<class String>
  String find_first_of_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(random2, position));
    return result;
  }

  template<class String>
  String find_first_of_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String find_first_of_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String find_first_of_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String find_first_of_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(random2.c_str(), position));
    return result;
  }

  template<class String>
  String find_first_of_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_of(value, position));
    return result;
  }

  template<class String>
  String find_last_of_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(searched, position3));
    return result;
  }

  template<class String>
  String find_last_of_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(random2, position));
    return result;
  }

  template<class String>
  String find_last_of_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String find_last_of_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String find_last_of_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String find_last_of_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(random2.c_str(), position));
    return result;
  }

  template<class String>
  String find_last_of_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_of(value, position));
    return result;
  }

  template<class String>
  String find_first_not_of_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(searched, position3));
    return result;
  }

  template<class String>
  String find_first_not_of_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(random2, position));
    return result;
  }

  template<class String>
  String find_first_not_of_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String find_first_not_of_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String find_first_not_of_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String find_first_not_of_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(random2.c_str(), position));
    return result;
  }

  template<class String>
  String find_first_not_of_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_first_not_of(value, position));
    return result;
  }

  template<class String>
  String find_last_not_of_string_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(searched, position3));
    return result;
  }

  template<class String>
  String find_last_not_of_string_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(random2, position));
    return result;
  }

  template<class String>
  String find_last_not_of_cstr_position_number__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(searched.c_str(), position3, searched.size()));
    return result;
  }

  template<class String>
  String find_last_not_of_cstr_position_number__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(random2.c_str(), position, random2.size()));
    return result;
  }

  template<class String>
  String find_last_not_of_cstr_position__existing()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type position2 = random(position1, random1.size());
    String searched(random1.substr(position1, position2));
    const typename String::size_type position3 = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(searched.c_str(), position3));
    return result;
  }

  template<class String>
  String find_last_not_of_cstr_position__random()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(random2.c_str(), position));
    return result;
  }

  template<class String>
  String find_last_not_of_char_position()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = Num2String<String>(random1.find_last_not_of(value, position));
    return result;
  }

  template<class String>
  String substr_position_number()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(position, random1.size());
    String result;
    result = random1.substr(position, number);
    return result;
  }

  template<class String>
  String substr_position_number__overflow()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String result;
    result = random1.substr(position, number);
    return result;
  }

  template<class String>
  String substr_position_number__npos()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    String result;
    result = random1.substr(position, String::npos);
    return result;
  }

  template<class String>
  String compare_string()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(random2));
    return result;
  }

  template<class String>
  String compare_string__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(random1));
    return result;
  }

  template<class String>
  String compare_position_number_string()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(position, number, random2));
    return result;
  }

  template<class String>
  String compare_position_number_string__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String same(random1.substr(position, number));
    String result;
    result = Tristate2String<String>(random1.compare(position, number, same));
    return result;
  }

  template<class String>
  String compare_position_number_string_position_number()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position2 = random(0, random2.size());
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    String result;
    result = Tristate2String<String>(random1.compare(position1, number1, random2, position2, number2));
    return result;
  }

  template<class String>
  String compare_position_number_string_position_number__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String result;
    result = Tristate2String<String>(random1.compare(position, number, random1, position, number));
    return result;
  }

  template<class String>
  String compare_cstr()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(random2.c_str()));
    return result;
  }

  template<class String>
  String compare_cstr__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(random1.c_str()));
    return result;
  }

  template<class String>
  String compare_position_number_cstr()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String random2(RandomString<String>(MaxString<String>::value));
    String result;
    result = Tristate2String<String>(random1.compare(position, number, random2.c_str()));
    return result;
  }

  template<class String>
  String compare_position_number_cstr__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String same(random1.substr(position, number));
    String result;
    result = Tristate2String<String>(random1.compare(position, number, same.c_str()));
    return result;
  }

  template<class String>
  String compare_position_number_cstr_number()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position1 = random(0, random1.size());
    const typename String::size_type number1 = random(0, MaxString<String>::value);
    String random2(RandomString<String>(MaxString<String>::value));
    const typename String::size_type number2 = random(0, MaxString<String>::value);
    String result;
    result = Tristate2String<String>(random1.compare(position1, number1, random2.c_str(), number2));
    return result;
  }

  template<class String>
  String compare_position_number_cstr_number__same()
  {
    // 21.3.6
    String random1(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, random1.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    String result;
    result = Tristate2String<String>(random1.compare(position, number, random1.c_str(), number));
    return result;
  }

}


template<class String>
class TestFunctions
{
public:
  typedef String (*TestFunction)();

  TestFunctions()
  {
    using namespace Tests;

#define ADD_TEST(test) (testFunctions.push_back(std::make_pair((#test), (test<String>))))
    ADD_TEST(operator_plus_string_string);
    ADD_TEST(operator_plus_cstr_string);
    ADD_TEST(operator_plus_char_string);
    ADD_TEST(operator_plus_string_cstr);
    ADD_TEST(operator_plus_string_char);
    ADD_TEST(operator_equal_equal_string_string);
    ADD_TEST(operator_equal_equal_cstr_string);
    ADD_TEST(operator_equal_equal_string_cstr);
    ADD_TEST(operator_not_equal_string_string);
    ADD_TEST(operator_not_equal_cstr_string);
    ADD_TEST(operator_not_equal_string_cstr);
    ADD_TEST(operator_smaller_string_string);
    ADD_TEST(operator_smaller_cstr_string);
    ADD_TEST(operator_smaller_string_cstr);
    ADD_TEST(operator_greater_string_string);
    ADD_TEST(operator_greater_cstr_string);
    ADD_TEST(operator_greater_string_cstr);
    ADD_TEST(operator_smaller_equal_string_string);
    ADD_TEST(operator_smaller_equal_cstr_string);
    ADD_TEST(operator_smaller_equal_string_cstr);
    ADD_TEST(operator_greater_equal_string_string);
    ADD_TEST(operator_greater_equal_cstr_string);
    ADD_TEST(operator_greater_equal_string_cstr);
    ADD_TEST(swap_string_string);
    ADD_TEST(swap_string_string__self);
    ADD_TEST(swap_string_string__selfcopy);
    ADD_TEST(swap_string_string__selfcopy2);
    ADD_TEST(constructor);
    ADD_TEST(constructor_string);
    ADD_TEST(constructor_string_position_number);
    ADD_TEST(constructor_string_position_number__bigNumber);
    ADD_TEST(constructor_cstr_number);
    ADD_TEST(constructor_number_char);
    ADD_TEST(constructor_iterator_iterator);
    ADD_TEST(operator_equal_string);
    ADD_TEST(operator_equal_string__self);
    ADD_TEST(operator_equal_string__selfCopy);
    ADD_TEST(operator_equal_cstr);
    ADD_TEST(operator_equal_cstr__self);
    ADD_TEST(operator_equal_cstr__selfCopy);
    ADD_TEST(iterators);
    ADD_TEST(capacity);
    ADD_TEST(resize_number_char);
    ADD_TEST(resize_number);
    ADD_TEST(reserve_number);
    ADD_TEST(clear);
    ADD_TEST(empty);
    ADD_TEST(element_access);
    ADD_TEST(operator_plus_equal_string);
    ADD_TEST(operator_plus_equal_string__self);
    ADD_TEST(operator_plus_equal_string__selfcopy);
    ADD_TEST(operator_plus_equal_cstr);
    ADD_TEST(operator_plus_equal_cstr__self);
    ADD_TEST(operator_plus_equal_cstr__selfcopy);
    ADD_TEST(operator_plus_equal_char);
    ADD_TEST(append_string);
    ADD_TEST(append_string__self);
    ADD_TEST(append_string__selfcopy);
    ADD_TEST(append_string_position_number);
    ADD_TEST(append_string_position_number__self);
    ADD_TEST(append_string_position_number__selfcopy);
    ADD_TEST(append_cstr_number);
    ADD_TEST(append_cstr_number__self);
    ADD_TEST(append_cstr_number__selfcopy);
    ADD_TEST(append_cstr);
    ADD_TEST(append_cstr__self);
    ADD_TEST(append_cstr__selfcopy);
    ADD_TEST(append_number_char);
    ADD_TEST(append_iterator_iterator);
    ADD_TEST(append_iterator_iterator__self);
    ADD_TEST(append_iterator_iterator__selfcopy);
    ADD_TEST(append_iterator_iterator__self_reverse);
    ADD_TEST(append_iterator_iterator__selfcopy_reverse);
    ADD_TEST(push_back_char);
    ADD_TEST(assign_string);
    ADD_TEST(assign_string__self);
    ADD_TEST(assign_string__selfcopy);
    ADD_TEST(assign_string_position_number);
    ADD_TEST(assign_string_position_number__self);
    ADD_TEST(assign_string_position_number__selfcopy);
    ADD_TEST(assign_cstr_number);
    ADD_TEST(assign_cstr_number__self);
    ADD_TEST(assign_cstr_number__selfcopy);
    ADD_TEST(assign_cstr);
    ADD_TEST(assign_cstr__self);
    ADD_TEST(assign_cstr__selfcopy);
    ADD_TEST(assign_number_char);
    ADD_TEST(assign_iterator_iterator);
    ADD_TEST(assign_iterator_iterator__self);
    ADD_TEST(assign_iterator_iterator__selfcopy);
    ADD_TEST(assign_iterator_iterator__self_reverse);
    ADD_TEST(assign_iterator_iterator__selfcopy_reverse);
    ADD_TEST(insert_position_string);
    ADD_TEST(insert_position_string__self);
    ADD_TEST(insert_position_string__selfcopy);
    ADD_TEST(insert_position_string_position_number);
    ADD_TEST(insert_position_string_position_number__self);
    ADD_TEST(insert_position_string_position_number__selfcopy);
    ADD_TEST(insert_position_cstr_number);
    ADD_TEST(insert_position_cstr_number__self);
    ADD_TEST(insert_position_cstr_number__selfcopy);
    ADD_TEST(insert_position_cstr);
    ADD_TEST(insert_position_cstr__self);
    ADD_TEST(insert_position_cstr__selfcopy);
    ADD_TEST(insert_iterator_char);
    ADD_TEST(insert_position_number_char);
    ADD_TEST(insert_iterator_iterator);
    ADD_TEST(insert_iterator_iterator__self);
    ADD_TEST(insert_iterator_iterator__selfcopy);
    ADD_TEST(insert_iterator_iterator__self_reverse);
    ADD_TEST(insert_iterator_iterator__selfcopy_reverse);
    ADD_TEST(erase_position_number);
    ADD_TEST(erase_iterator);
    ADD_TEST(erase_iterator_iterator);
    ADD_TEST(replace_position_number_string);
    ADD_TEST(replace_position_number_string__self);
    ADD_TEST(replace_position_number_string__selfcopy);
    ADD_TEST(replace_position_number_string_position_number);
    ADD_TEST(replace_position_number_string_position_number__self);
    ADD_TEST(replace_position_number_string_position_number__selfcopy);
    ADD_TEST(replace_position_number_cstr_number);
    ADD_TEST(replace_position_number_cstr_number__self);
    ADD_TEST(replace_position_number_cstr_number__selfcopy);
    ADD_TEST(replace_position_number_cstr);
    ADD_TEST(replace_position_number_cstr__self);
    ADD_TEST(replace_position_number_cstr__selfcopy);
    ADD_TEST(replace_position_number_number_char);
    ADD_TEST(replace_iterator_iterator_string);
    ADD_TEST(replace_iterator_iterator_string__self);
    ADD_TEST(replace_iterator_iterator_string__selfcopy);
    ADD_TEST(replace_iterator_iterator_cstr_number);
    ADD_TEST(replace_iterator_iterator_cstr_number__self);
    ADD_TEST(replace_iterator_iterator_cstr_number__selfcopy);
    ADD_TEST(replace_iterator_iterator_cstr);
    ADD_TEST(replace_iterator_iterator_cstr__self);
    ADD_TEST(replace_iterator_iterator_cstr__selfcopy);
    ADD_TEST(replace_iterator_iterator_number_char);
    ADD_TEST(replace_iterator_iterator_iterator_iterator);
    ADD_TEST(replace_iterator_iterator_iterator_iterator__self);
    ADD_TEST(replace_iterator_iterator_iterator_iterator__selfcopy);
    ADD_TEST(copy_char_number_position);
    ADD_TEST(swap);
    ADD_TEST(swap__self);
    ADD_TEST(swap__selfcopy);
    ADD_TEST(string_operations);
    ADD_TEST(find_string_position__existing);
    ADD_TEST(find_string_position__random);
    ADD_TEST(find_cstr_position_number__existing);
    ADD_TEST(find_cstr_position_number__random);
    ADD_TEST(find_cstr_position__existing);
    ADD_TEST(find_cstr_position__random);
    ADD_TEST(find_char_position);
    ADD_TEST(rfind_string_position__existing);
    ADD_TEST(rfind_string_position__random);
    ADD_TEST(rfind_cstr_position_number__existing);
    ADD_TEST(rfind_cstr_position_number__random);
    ADD_TEST(rfind_cstr_position__existing);
    ADD_TEST(rfind_cstr_position__random);
    ADD_TEST(rfind_char_position);
    ADD_TEST(find_overflow);
    ADD_TEST(rfind_overflow);
    ADD_TEST(find_first_of_string_position__existing);
    ADD_TEST(find_first_of_string_position__random);
    ADD_TEST(find_first_of_cstr_position_number__existing);
    ADD_TEST(find_first_of_cstr_position_number__random);
    ADD_TEST(find_first_of_cstr_position__existing);
    ADD_TEST(find_first_of_cstr_position__random);
    ADD_TEST(find_first_of_char_position);
    ADD_TEST(find_last_of_string_position__existing);
    ADD_TEST(find_last_of_string_position__random);
    ADD_TEST(find_last_of_cstr_position_number__existing);
    ADD_TEST(find_last_of_cstr_position_number__random);
    ADD_TEST(find_last_of_cstr_position__existing);
    ADD_TEST(find_last_of_cstr_position__random);
    ADD_TEST(find_last_of_char_position);
    ADD_TEST(find_first_not_of_string_position__existing);
    ADD_TEST(find_first_not_of_string_position__random);
    ADD_TEST(find_first_not_of_cstr_position_number__existing);
    ADD_TEST(find_first_not_of_cstr_position_number__random);
    ADD_TEST(find_first_not_of_cstr_position__existing);
    ADD_TEST(find_first_not_of_cstr_position__random);
    ADD_TEST(find_first_not_of_char_position);
    ADD_TEST(find_last_not_of_string_position__existing);
    ADD_TEST(find_last_not_of_string_position__random);
    ADD_TEST(find_last_not_of_cstr_position_number__existing);
    ADD_TEST(find_last_not_of_cstr_position_number__random);
    ADD_TEST(find_last_not_of_cstr_position__existing);
    ADD_TEST(find_last_not_of_cstr_position__random);
    ADD_TEST(find_last_not_of_char_position);
    ADD_TEST(substr_position_number);
    ADD_TEST(substr_position_number__overflow);
    ADD_TEST(substr_position_number__npos);
    ADD_TEST(compare_string);
    ADD_TEST(compare_string__same);
    ADD_TEST(compare_position_number_string);
    ADD_TEST(compare_position_number_string__same);
    ADD_TEST(compare_position_number_string_position_number);
    ADD_TEST(compare_position_number_string_position_number__same);
    ADD_TEST(compare_cstr);
    ADD_TEST(compare_cstr__same);
    ADD_TEST(compare_position_number_cstr);
    ADD_TEST(compare_position_number_cstr__same);
    ADD_TEST(compare_position_number_cstr_number);
    ADD_TEST(compare_position_number_cstr_number__same);
  }

  ~TestFunctions()
  {
    testFunctions.clear();
  }

  size_t getNumberInTestRange(size_t number) const
  {
    return number % testFunctions.size();
  }

  TestFunction getTest(size_t number) const
  {
    return testFunctions.at(getNumberInTestRange(number)).second;
  }

  const char * getTestName(size_t number) const
  {
    return testFunctions.at(getNumberInTestRange(number)).first;
  }

private:
  std::vector<std::pair<const char *, TestFunction> > testFunctions;

  TestFunctions(const TestFunctions &);
  TestFunctions & operator=(const TestFunctions &);
};


template <class String>
String Test(size_t count, const TestFunctions<String> & testFunctions)
{
  try
  {
    String result(testFunctions.getTest(count)());
    return result;
  }
  catch(const std::length_error &)
  {
    return LengthErrorExceptionString<String>();
  }
  catch(const std::out_of_range &)
  {
    return OutOfRangeExceptionString<String>();
  }
  catch(...)
  {
    return UnknownExceptionString<String>();
  }
}

template<class String>
void checkResults(const std::string & reference, const String & tested,
                  const TestFunctions<String> & testFunctions,
                  unsigned int seedForThisIteration, size_t count)
{
  if( (tested.size() != reference.size()) ||
      (std::string(tested.data(), tested.size()) != reference) )
  {
    std::cout << "\n\nTest #"
              << testFunctions.getNumberInTestRange(count)            << " \""
              << testFunctions.getTestName(count)                     << "\" failed for string type \""
              << typeid(String).name()                                << "\":\n"
              << "seed for this iteration = " << seedForThisIteration << "\n"
              << "reference.size() = "        << reference.size()     << "\n"
              << "tested.size()    = "        << tested.size()        << "\n"
              << "reference data   = {"       << reference            << "}\n"
              << "tested    data   = {"       << tested               << "}\n\n";
  }
}

void Compare()
{
    size_t count = 0;

    using namespace StringsToTest;
    TestFunctions<std::string>                          testFunctions_std;
    TestFunctions<my_string_SimpleStorage>              testFunctions_SimpleStorage;
    TestFunctions<my_string_AllocatorStorage>           testFunctions_AllocatorStorage;
    TestFunctions<my_string_MallocatorStorage>          testFunctions_MallocatorStorage;
    TestFunctions<my_string_VectorStorage>              testFunctions_VectorStorage;
    TestFunctions<my_string_SmallStringSimple>          testFunctions_SmallStringSimple;
    TestFunctions<my_string_SmallStringSimpleBigBuffer> testFunctions_smallStringSimpleBigBuffer;
    TestFunctions<my_string_SmallStringVector>          testFunctions_SmallStringVector;
    TestFunctions<my_string_CowSimple>                  testFunctions_CowSimple;
    TestFunctions<my_string_CowAllocator>               testFunctions_CowAllocator;

    for (;;)
    {
      std::cout << ++count << '\r';

      const unsigned int seedForThisIteration = count + rand();
      srand(seedForThisIteration);

      const std::string reference(Test<std::string>(count, testFunctions_std));

      {
        srand(seedForThisIteration);
        const my_string_SimpleStorage tested(Test<my_string_SimpleStorage>(count, testFunctions_SimpleStorage));
        checkResults(reference, tested, testFunctions_SimpleStorage, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_AllocatorStorage tested(Test<my_string_AllocatorStorage>(count, testFunctions_AllocatorStorage));
        checkResults(reference, tested, testFunctions_AllocatorStorage, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_MallocatorStorage tested(Test<my_string_MallocatorStorage>(count, testFunctions_MallocatorStorage));
        checkResults(reference, tested, testFunctions_MallocatorStorage, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_VectorStorage tested = Test<my_string_VectorStorage>(count, testFunctions_VectorStorage);
        checkResults(reference, tested, testFunctions_VectorStorage, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_SmallStringSimple tested(Test<my_string_SmallStringSimple>(count, testFunctions_SmallStringSimple));
        checkResults(reference, tested, testFunctions_SmallStringSimple, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_SmallStringSimpleBigBuffer tested(Test<my_string_SmallStringSimpleBigBuffer>(count, testFunctions_smallStringSimpleBigBuffer));
        checkResults(reference, tested, testFunctions_smallStringSimpleBigBuffer, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_SmallStringVector tested(Test<my_string_SmallStringVector>(count, testFunctions_SmallStringVector));
        checkResults(reference, tested, testFunctions_SmallStringVector, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_CowSimple tested(Test<my_string_CowSimple>(count, testFunctions_CowSimple));
        checkResults(reference, tested, testFunctions_CowSimple, seedForThisIteration, count);
      }

      {
        srand(seedForThisIteration);
        const my_string_CowAllocator tested(Test<my_string_CowAllocator>(count, testFunctions_CowAllocator));
        checkResults(reference, tested, testFunctions_CowAllocator, seedForThisIteration, count);
      }
    }
}

int main()
{
  const time_t initialSeed(time(0));
  srand(unsigned(initialSeed));

  std::cout << "initial seed = " << initialSeed << "\n\n";

  Compare();

  return 0;
}
