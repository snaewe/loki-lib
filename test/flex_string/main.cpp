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
  for (; i != result.size(); ++i)
  {
    result[i] = random('a', 'z');
  }
  return result;
}

template<class String, class Integral>
String Num2String(Integral value)
{
  typedef typename String::value_type CharType;
  std::basic_ostringstream<CharType, std::char_traits<CharType>, std::allocator<CharType> > stream;
  stream << value;
  return stream.str().c_str();
}

template<class String>
std::list<typename String::value_type> RandomList(typename String::size_type maxSize)
{
  const typename String::size_type size = random(0, maxSize);
  std::list<typename String::value_type> lst(size);
  std::list<typename String::value_type>::iterator i = lst.begin();
  for (; i != lst.end(); ++i)
  {
    *i = random('a', 'z');
  }
  return lst;
}

namespace Tests
{
  template<class String>
  struct MaxString
  {
    static const typename String::size_type value = 1050;
  };


  template<class String>
  String default_constructor(String &)
  {
    // 21.3.1
    return String();
  }

  template<class String>
  String copy_constructor(String & test)
  {
    // 21.3.1
    String s(test);
    return s;
  }

  template<class String>
  String copy_constructor_with_size_and_range(String & test)
  {
    // 21.3.1
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type length = random(0, test.size() - pos);
    String s(test, pos, length);
    return s;
  }

  template<class String>
  String constructor_with_cstr_and_size(String & test)
  {
    // 21.3.1
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type n = random(0, test.size() - pos);
    String s(test.c_str() + pos, n);
    return s;
  }

  template<class String>
  String constructor_with_cstr(String & test)
  {
    // 21.3.1
    const typename String::size_type pos = random(0, test.size());
    String s(test.c_str() + pos);
    return s;
  }

  template<class String>
  String assignment(String & test)
  {
    // 21.3.1
    const typename String::size_type size = random(0, 1000);
    String s(size, '\0');
    typename String::size_type i = 0;
    for (; i != s.size(); ++i)
    {
      s[i] = random('a', 'z');
    }
    test = s;
    return test;
  }

  template<class String>
  String assignment_with_cstr(String & test)
  {
    // 21.3.1
    const typename String::size_type size = random(0, 1000);
    String s(size, '\0');
    typename String::size_type i = 0;
    for (; i != s.size(); ++i)
    {
      s[i] = random('a', 'z');
    }
    test = s.c_str();
    return test;
  }

  template<class String>
  String assignment_aliased(String & test)
  {
    // 21.3.1
    const size_t pos = random(0, test.size());
    test = test.c_str() + pos;
    return test;
  }

  template<class String>
  String assignment_non_aliased(String & test)
  {
    // 21.3.1
    const size_t pos = random(0, test.size());
    test = String(test.c_str() + pos);
    return test;
  }

  template<class String>
  String assignment_from_char(String & test)
  {
    // 21.3.1
    test = random('a', 'z');
    return test;
  }

  template<class String>
  String iterators_call(String & test)
  {
    // 21.3.2
    String result;
    result += Num2String<String>(test.end() - test.begin()) + " -- ";
    result += Num2String<String>(test.rend() - test.rbegin()) + " -- ";
    return result;
  }

  template<class String>
  String iterators_equality(String & test)
  {
    // 21.3.2
    String result;
    result += Num2String<String>(test.size()) + " -- ";
    if(0 != test.size())
    {
      result += Num2String<String>(test.begin() != test.end()) + " -- ";
      result += Num2String<String>(test.rbegin() != test.rend()) + " -- ";
    }
    else
    {
      result += Num2String<String>(test.begin() == test.end()) + " -- ";
      result += Num2String<String>(test.rbegin() == test.rend()) + " -- ";
    }
    return result;
  }

  template<class String>
  String capacity(String & test)
  {
    // 21.3.2
    String result;
    result += Num2String<String>(test.size()) + " -- ";
    result += Num2String<String>(test.length()) + " -- ";
    test.max_size();
    test.capacity();
    result += test;
    return result;
  }

  template<class String>
  String capacity_values(String & test)
  {
    // 21.3.2
    String result;
    result += Num2String<String>(test.size() == test.length()) + " -- ";
    result += Num2String<String>(test.capacity() >= test.size()) + " -- ";
    result += Num2String<String>(test.max_size() >= test.size()) + " -- ";
    result += Num2String<String>(test.max_size() >= test.capacity()) + " -- ";
    return result;
  }

  template<class String>
  String resize(String & test)
  {
    const typename String::size_type newSize = random(0, test.size());
    const typename String::value_type value = random('a', 'z');
    test.resize(newSize, value);
    return test;
  }

  template<class String>
  String resize_with_1_argument(String & test)
  {
    const typename String::size_type newSize = random(0, test.size());
    test.resize(newSize);
    return test;
  }

  template<class String>
  String reserve(String & test)
  {
    const typename String::size_type reserveSize = random(0, 1000);
    test.reserve(reserveSize);
    return test;
  }

  template<class String>
  String clear(String & test)
  {
    test.clear();
    return test;
  }

  template<class String>
  String empty1(String & test)
  {
    if (test.empty()) test = "empty";
    else test = "not empty";
    return test;
  }

  template<class String>
  String empty2(String & test)
  {
    const char* kEmptyString = "empty";
    const char* kNonEmptyString = "not empty";
    if (test.empty()) test = String(kEmptyString, kEmptyString + strlen(kEmptyString));
    else test = String(kNonEmptyString, kNonEmptyString + strlen(kNonEmptyString));
    return test;
  }

  template<class String>
  String element_access(String & test)
  {
    // 21.3.4
    if(!test.empty())
    {
      const typename String::size_type index1 = random(0, test.size() - 1);
      test += test[index1];
      const typename String::size_type index2 = random(0, test.size() - 1);
      test += test.at(index2);
    }
    return test;
  }

  template<class String>
  String operator_plus_equal(String & test)
  {
    String str(RandomString<String>(MaxString<String>::value));
    test += str;
    return test;
  }

  template<class String>
  String operator_plus_equal_aliasing(String & test)
  {
    test += test;
    return test;
  }

  template<class String>
  String operator_plus_equal_with_cstr(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    test += str.c_str();
    return test;
  }

  template<class String>
  String operator_plus_equal_no_aliasing(String & test)
  {
    // 21.3.5
    const typename String::size_type offset = random(0, test.size());
    test += String(test.c_str() + offset);
    return test;
  }

  template<class String>
  String operator_plus_equal_aliasing_cstr(String & test)
  {
    // 21.3.5
    const typename String::size_type offset = random(0, test.size());
    test += test.c_str() + offset;
    return test;
  }

  template<class String>
  String operator_plus_equal_char(String & test)
  {
    // 21.3.5
    test += random('a', 'z');
    return test;
  }

  template<class String>
  String append_string(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    test.append(str);
    return test;
  }

  template<class String>
  String append_string_start_range(String & test)
  {
    // 21.3.5
    String s(RandomString<String>(MaxString<String>::value));
    const typename String::size_type start = random(0, s.size());
    const typename String::size_type range = random(0, MaxString<String>::value);
    test.append(s, start, range);
    return test;
  }

  template<class String>
  String append_cstr_size(String & test)
  {
    // 21.3.5
    String s(RandomString<String>(MaxString<String>::value));
    const typename String::size_type size = random(0, s.size());
    test.append(s.c_str(), size);
    return test;
  }

  template<class String>
  String append_cstr(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    test.append(str.c_str());
    return test;
  }

  template<class String>
  String append_count_char(String & test)
  {
    // 21.3.5
    const typename String::size_type count = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    test.append(count, value);
    return test;
  }

  template<class String>
  String append_iterators(String & test)
  {
    // 21.3.5
    std::list<typename String::value_type> lst(RandomList<String>(MaxString<String>::value));
    test.append(lst.begin(), lst.end());
    return test;
  }

  template<class String>
  String push_back_char(String & test)
  {
    // 21.3.5
    const typename String::value_type value = random('a', 'z');
    test.push_back(value);
    return test;
  }

  template<class String>
  String assign_string(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    test.assign(str);
    return test;
  }

  template<class String>
  String assign_string_start_size(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type start = random(0, str.size());
    const typename String::size_type size = random(0, MaxString<String>::value);
    test.assign(str, start, size);
    return test;
  }

  template<class String>
  String assign_cstr_size(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type size = random(0, str.size());
    test.assign(str.c_str(), size);
    return test;
  }

  template<class String>
  String assign_cstr(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    test.assign(str.c_str());
    return test;
  }

  template<class String>
  String assign_number_char(String & test)
  {
    // 21.3.5
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    test.assign(number, value);
    return test;
  }

  template<class String>
  String assign_iterators(String & test)
  {
    // 21.3.5
    std::list<typename String::value_type> lst(RandomList<String>(MaxString<String>::value));
    test.assign(lst.begin(), lst.end());
    return test;
  }

  template<class String>
  String insert_position_string(String & test)
  {
    // 21.3.5
    const typename String::size_type position = random(0, test.size());
    String str(RandomString<String>(MaxString<String>::value));
    test.insert(position, str);
    return test;
  }

  template<class String>
  String insert_position_string_start_end(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, test.size());
    const typename String::size_type start = random(0, str.size());
    const typename String::size_type end = random(0, MaxString<String>::value);
    test.insert(position, str, start, end);
    return test;
  }

  template<class String>
  String insert_position_cstr_size(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type position = random(0, test.size());
    const typename String::size_type size = random(0, str.size());
    test.insert(position, str.c_str(), size);
    return test;
  }

  template<class String>
  String insert_position_cstr(String & test)
  {
    // 21.3.5
    const typename String::size_type position = random(0, test.size());
    String str(RandomString<String>(MaxString<String>::value));
    test.insert(position, str.c_str());
    return test;
  }

  template<class String>
  String insert_position_number_char(String & test)
  {
    // 21.3.5
    const typename String::size_type position = random(0, test.size());
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    test.insert(position, number, value);
    return test;
  }

  template<class String>
  String insert_iterator_char(String & test)
  {
    // 21.3.5
    const typename String::size_type offset = random(0, test.size());
    const typename String::value_type value = random('a', 'z');
    test.insert(test.begin() + offset, value);
    return test;
  }

  template<class String>
  String insert_iterator_iterator_range(String & test)
  {
    // 21.3.5
    std::list<typename String::value_type> lst(RandomList<String>(MaxString<String>::value));
    const typename String::size_type offset = random(0, test.size());
    test.insert(test.begin() + offset, lst.begin(), lst.end());
    return test;
  }

  template<class String>
  String erase_position_position(String & test)
  {
    // 21.3.5
    const typename String::size_type start = random(0, test.size());
    const typename String::size_type end = random(0, MaxString<String>::value);
    test.erase(start, end);
    return test;
  }

  template<class String>
  String erase_iterator(String & test)
  {
    // 21.3.5
    if(!test.empty())
    {
      const typename String::size_type offset = random(0, test.size());
      test.erase(test.begin() + offset);
    }
    return test;
  }

  template<class String>
  String erase_iterator_iterator(String & test)
  {
    // 21.3.5
    const typename String::size_type offset = random(0, test.size());
    const typename String::iterator i = test.begin() + offset;
    const typename String::size_type endOffset = random(0, test.end() - i);
    test.erase(i, i + endOffset);
    return test;
  }

  template<class String>
  String replace_start_end_copyself(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type end = random(0, test.size() - pos);
    test.replace(pos, end, String(test));
    return test;
  }

  template<class String>
  String replace_start_end_self(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type end = random(0, test.size() - pos);
    test.replace(pos, end, test);
    return test;
  }

  template<class String>
  String replace_start_end_string(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type end = random(0, test.size() - pos);
    String str(RandomString<String>(MaxString<String>::value));
    test.replace(pos, pos + end, str);
    return test;
  }

  template<class String>
  String replace_start_end_selfcopy_start_end(String & test)
  {
    // 21.3.5
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    const typename String::size_type offset1 = random(0, test.size() - pos1);
    const typename String::size_type offset2 = random(0, test.size() - pos2);
    test.replace(pos1, pos1 + offset1, String(test), pos2, pos2 + offset2);
    return test;
  }

  template<class String>
  String replace_start_end_self_start_end(String & test)
  {
    // 21.3.5
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    const typename String::size_type offset1 = random(0, test.size() - pos1);
    const typename String::size_type offset2 = random(0, test.size() - pos2);
    test.replace(pos1, pos1 + offset1, test, pos2, pos2 + offset2);
    return test;
  }

  template<class String>
  String replace_start_end_string_start_end(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, str.size());
    const typename String::size_type offset1 = random(0, test.size() - pos1);
    const typename String::size_type offset2 = random(0, str.size() - pos2);
    test.replace(pos1, pos1 + offset1, str, pos2, pos2 + offset2);
    return test;
  }

  template<class String>
  String replace_start_end_selfcopycstr_size(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size() - pos);
    test.replace(pos, pos2, String(test).c_str(), test.size());
    return test;
  }

  template<class String>
  String replace_start_end_selfcstr_size(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size() - pos);
    test.replace(pos, pos2, test.c_str(), test.size());
    return test;
  }

  template<class String>
  String replace_start_end_stringcstr_size(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    test.replace(pos, pos + offset, str.c_str(), str.size());
    return test;
  }

  template<class String>
  String replace_start_end_stringcstr(String & test)
  {
    // 21.3.5
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    test.replace(pos, pos + offset, str.c_str());
    return test;
  }

  template<class String>
  String replace_start_end_number_char(String & test)
  {
    // 21.3.5
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size() - pos1);
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    test.replace(pos1, pos2, number, value);
    return test;
  }

  template<class String>
  String replace_iterator_iterator_selfcopy(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    test.replace(test.begin() + pos, test.begin() + pos + offset, String(test));
    return test;
  }

  template<class String>
  String replace_iterator_iterator_self(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    test.replace(test.begin() + pos, test.begin() + pos + offset, test);
    return test;
  }

  template<class String>
  String replace_iterator_iterator_selfcopycstr_size(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    const typename String::size_type size = random(0, test.size());
    test.replace(test.begin() + pos, test.begin() + pos + offset, String(test).c_str(), test.size() - size);
    return test;
  }

  template<class String>
  String replace_iterator_iterator_selfcstr_size(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type offset = random(0, test.size() - pos);
    const typename String::size_type size = random(0, test.size());
    test.replace(test.begin() + pos, test.begin() + pos + offset, test.c_str(), test.size() - size);
    return test;
  }

  template<class String>
  String replace_iterator_iterator_stringcstr(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type n = random(0, test.size() - pos);
    typename String::iterator b = test.begin();
    const String str(RandomString<String>(MaxString<String>::value));
    const typename String::value_type* s = str.c_str();
    test.replace(b + pos, b + pos + n, s);
    return test;
  }

  template<class String>
  String replace_iterator_iterator_number_char(String & test)
  {
    // 21.3.5
    const typename String::size_type pos = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size() - pos);
    const typename String::size_type number = random(0, MaxString<String>::value);
    const typename String::value_type value = random('a', 'z');
    test.replace(test.begin() + pos, test.begin() + pos + pos2, number, value);
    return test;
  }

  template<class String>
  String copy_pointer_size_position(String & test)
  {
    // 21.3.5
    std::vector<typename String::value_type> vec(random(1, MaxString<String>::value));
    const typename String::size_type pos = random(0, test.size());
    test.copy(&vec[0], vec.size(), pos);
    return test;
  }

  template<class String>
  String member_swap(String & test)
  {
    // 21.3.5
    String s(RandomString<String>(MaxString<String>::value));
    s.swap(test);
    return test + " -- " + s;
  }

  template<class String>
  String member_swap2(String & test)
  {
    // 21.3.5
    String s(RandomString<String>(MaxString<String>::value));
    test.swap(s);
    return test + " -- " + s;
  }

  template<class String>
  String member_self_swap(String & test)
  {
    // 21.3.5
    test.swap(test);
    return test;
  }

  template<class String>
  String member_selfcopy_swap(String & test)
  {
    // 21.3.5
    String(test).swap(test);
    return test;
  }

  template<class String>
  String member_selfcopy_swap2(String & test)
  {
    // 21.3.5
    String s(test);
    test.swap(s);
    return test + " -- " + s;
  }

  template<class String>
  String swap(String & test)
  {
    using std::swap;
    String s(RandomString<String>(MaxString<String>::value));
    swap(test, s);
    return test + " -- " + s;
  }

  template<class String>
  String swap2(String & test)
  {
    using std::swap;
    String s(RandomString<String>(MaxString<String>::value));
    swap(s, test);
    return test + " -- " + s;
  }

  template<class String>
  String swap_self(String & test)
  {
    using std::swap;
    swap(test, test);
    return test;
  }

  template<class String>
  String swap_selfcopy(String & test)
  {
    using std::swap;
    String s(test);
    swap(test, s);
    return test + " -- " + s;
  }

  template<class String>
  String swap_selfcopy2(String & test)
  {
    using std::swap;
    String s(test);
    swap(s, test);
    return test + " -- " + s;
  }

  template<class String>
  String cstr_data_getallocator(String & test)
  {
    // 21.3.6
    String result;
    String str(RandomString<String>(MaxString<String>::value));
    result += Num2String<String>(test.c_str() == test.data()) + " -- ";
    result += Num2String<String>(test.get_allocator() == str.get_allocator()) + " -- ";
    return result;
  }

  template<class String>
  String find_string_index(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find(str, index));
    return test;
  }

  template<class String>
  String find_stringcstr_index_length(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.find(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String find_stringcstr_index(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find(str.c_str(), index));
    return test;
  }

  template<class String>
  String find_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find(value, index));
    return test;
  }

  template<class String>
  String find_overflow(String & test)
  {
    // This tests bug 2536490.

    test = String();

    String result;

    // Validate that find with a single character doesn't overflow.
    result += Num2String<String>(test.find('6'));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.find('6'));
    result += " -- ";
    test = "123456";
    result += Num2String<String>(test.find('6'));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.find('6'));
    result += " -- ";

    // Validate that find with multiple characters doesn't overflow.
    test = "12345";
    result += Num2String<String>(test.find("123"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.find("12345"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.find("345"));
    result += " -- ";
    test = "123456";
    result += Num2String<String>(test.find("456"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.find("456"));
    result += " -- ";

    return result;
  }

  template<class String>
  String rfind_overflow(String & test)
  {
    test = String();

    String result;

    // Validate that rfind with a single character doesn't overflow.
    result += Num2String<String>(test.rfind('6'));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.rfind('6'));
    result += " -- ";
    test = "123456";
    result += Num2String<String>(test.rfind('6'));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.rfind('6'));
    result += " -- ";

    // Validate that rfind with multiple characters doesn't overflow.
    test = "12345";
    result += Num2String<String>(test.rfind("123"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.rfind("12345"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.rfind("345"));
    result += " -- ";
    test = "123456";
    result += Num2String<String>(test.rfind("456"));
    result += " -- ";
    test = "12345";
    result += Num2String<String>(test.rfind("456"));
    result += " -- ";

    return result;
  }

  template<class String>
  String rfind_string_index(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.rfind(str, index));
    return test;
  }

  template<class String>
  String rfind_stringcstr_index_length(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.rfind(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String rfind_stringcstr_index(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    String str = test.substr(pos1, pos2);
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.rfind(str.c_str(), index));
    return test;
  }

  template<class String>
  String rfind_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.rfind(value, index));
    return test;
  }

  template<class String>
  String find_first_of_string_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_of(str, index));
    return test;
  }

  template<class String>
  String find_first_of_stringcstr_index_length(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.find_first_of(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String find_first_of_stringcstr_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_of(str.c_str(), index));
    return test;
  }

  template<class String>
  String find_first_of_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_of(value, index));
    return test;
  }

  template<class String>
  String find_last_of_string_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_of(str, index));
    return test;
  }

  template<class String>
  String find_last_of_stringcstr_index_length(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.find_last_of(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String find_last_of_stringcstr_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_of(str.c_str(), index));
    return test;
  }

  template<class String>
  String find_last_of_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_of(value, index));
    return test;
  }

  template<class String>
  String find_first_not_of_string_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_not_of(str, index));
    return test;
  }

  template<class String>
  String find_first_not_of_stringcstr_index_length(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.find_first_not_of(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String find_first_not_of_stringcstr_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_not_of(str.c_str(), index));
    return test;
  }

  template<class String>
  String find_first_not_of_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_first_not_of(value, index));
    return test;
  }

  template<class String>
  String find_last_not_of_string_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_not_of(str, index));
    return test;
  }

  template<class String>
  String find_last_not_of_stringcstr_index_length(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, str.size());
    test = Num2String<String>(test.find_last_not_of(str.c_str(), index, length));
    return test;
  }

  template<class String>
  String find_last_not_of_stringcstr_index(String & test)
  {
    // 21.3.6
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_not_of(str.c_str(), index));
    return test;
  }

  template<class String>
  String find_last_not_of_char_index(String & test)
  {
    // 21.3.6
    const typename String::value_type value = random('a', 'z');
    const typename String::size_type index = random(0, test.size());
    test = Num2String<String>(test.find_last_not_of(value, index));
    return test;
  }

  template<class String>
  String substr_index_length(String & test)
  {
    // 21.3.6
    const typename String::size_type pos1 = random(0, test.size());
    const typename String::size_type pos2 = random(0, test.size());
    test = test.substr(pos1, pos2);
    return test;
  }

  template<class String>
  String compare_selfcopy(String & test)
  {
    int tristate = test.compare(String(test));
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_string(String & test)
  {
    String str(RandomString<String>(MaxString<String>::value));
    int tristate = test.compare(str);
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_index_length_selfcopy(String & test)
  {
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, test.size());
    int tristate = test.compare(index, length, String(test));
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_index_length_string(String & test)
  {
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, test.size());
    String str(RandomString<String>(MaxString<String>::value));
    int tristate = test.compare(index, length, str);
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_index_length_selfcopy_index_length(String & test)
  {
    String str = test;
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, test.size());
    const typename String::size_type index2 = random(0, str.size());
    const typename String::size_type length2 = random(0, str.size());
    int tristate = test.compare(index, length, str, index2, length2);
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_index_length_string_index_length(String & test)
  {
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, test.size());
    const typename String::size_type index2 = random(0, str.size());
    const typename String::size_type length2 = random(0, str.size());
    int tristate = test.compare(index, length, str, index2, length2);
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_stringcstr(String & test)
  {
    String str(RandomString<String>(MaxString<String>::value));
    int tristate = test.compare(str.c_str());
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String compare_index_length_stringcstr_length(String & test)
  {
    String str(RandomString<String>(MaxString<String>::value));
    const typename String::size_type index = random(0, test.size());
    const typename String::size_type length = random(0, test.size());
    const typename String::size_type index2 = random(0, str.size());
    int tristate = test.compare(index, length, str.c_str(), index2);
    if (tristate > 0) tristate = 1;
    else if (tristate < 0) tristate = 2;
    test = Num2String<String>(tristate);
    return test;
  }

  template<class String>
  String operator_plus(String & test)
  {
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    test = random1 + random2;
    return test;
  }

  template<class String>
  String operator_plus_lhs_cstr(String & test)
  {
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    test = random1.c_str() + random2;
    return test;
  }

  template<class String>
  String operator_plus_lhs_char(String & test)
  {
    const typename String::value_type value = random('a', 'z');
    String random1(RandomString<String>(MaxString<String>::value));
    test = value + random1;
    return test;
  }

  template<class String>
  String operator_plus_rhs_cstr(String & test)
  {
    String random1(RandomString<String>(MaxString<String>::value));
    String random2(RandomString<String>(MaxString<String>::value));
    test = random1 + random2.c_str();
    return test;
  }

  template<class String>
  String operator_plus_rhs_char(String & test)
  {
    const typename String::value_type value = random('a', 'z');
    String random1(RandomString<String>(MaxString<String>::value));
    test = random1 + value;
    return test;
  }

}


template<class String>
class TestFunctions
{
public:
  typedef String (*TestFunction)(String &);

  TestFunctions()
  {
    using namespace Tests;

#define ADD_TEST(test) (testFunctions.push_back(std::make_pair((#test), (test<String>))))
    ADD_TEST(default_constructor);
    ADD_TEST(copy_constructor);
    ADD_TEST(copy_constructor_with_size_and_range);
    ADD_TEST(constructor_with_cstr_and_size);
    ADD_TEST(constructor_with_cstr);
    ADD_TEST(assignment);
    ADD_TEST(assignment_with_cstr);
    ADD_TEST(assignment_aliased);
    ADD_TEST(assignment_non_aliased);
    ADD_TEST(assignment_from_char);
    ADD_TEST(iterators_call);
    ADD_TEST(iterators_equality);
    ADD_TEST(capacity);
    ADD_TEST(capacity_values);
    ADD_TEST(resize);
    ADD_TEST(resize_with_1_argument);
    ADD_TEST(reserve);
    ADD_TEST(clear);
    ADD_TEST(empty1);
    ADD_TEST(empty2);
    ADD_TEST(element_access);
    ADD_TEST(operator_plus_equal);
    ADD_TEST(operator_plus_equal_aliasing);
    ADD_TEST(operator_plus_equal_with_cstr);
    ADD_TEST(operator_plus_equal_no_aliasing);
    ADD_TEST(operator_plus_equal_aliasing_cstr);
    ADD_TEST(operator_plus_equal_char);
    ADD_TEST(append_string);
    ADD_TEST(append_string_start_range);
    ADD_TEST(append_cstr_size);
    ADD_TEST(append_cstr);
    ADD_TEST(append_count_char);
    ADD_TEST(append_iterators);
    ADD_TEST(push_back_char);
    ADD_TEST(assign_string);
    ADD_TEST(assign_string_start_size);
    ADD_TEST(assign_cstr_size);
    ADD_TEST(assign_cstr);
    ADD_TEST(assign_number_char);
    ADD_TEST(assign_iterators);
    ADD_TEST(insert_position_string);
    ADD_TEST(insert_position_string_start_end);
    ADD_TEST(insert_position_cstr_size);
    ADD_TEST(insert_position_cstr);
    ADD_TEST(insert_position_number_char);
    ADD_TEST(insert_iterator_char);
    ADD_TEST(insert_iterator_iterator_range);
    ADD_TEST(erase_position_position);
    ADD_TEST(erase_iterator);
    ADD_TEST(erase_iterator_iterator);
    ADD_TEST(replace_start_end_copyself);
    ADD_TEST(replace_start_end_self);
    ADD_TEST(replace_start_end_string);
    ADD_TEST(replace_start_end_selfcopy_start_end);
    ADD_TEST(replace_start_end_self_start_end);
    ADD_TEST(replace_start_end_string_start_end);
    ADD_TEST(replace_start_end_selfcopycstr_size);
    ADD_TEST(replace_start_end_selfcstr_size);
    ADD_TEST(replace_start_end_stringcstr_size);
    ADD_TEST(replace_start_end_stringcstr);
    ADD_TEST(replace_start_end_number_char);
    ADD_TEST(replace_iterator_iterator_selfcopy);
    ADD_TEST(replace_iterator_iterator_self);
    ADD_TEST(replace_iterator_iterator_selfcopycstr_size);
    ADD_TEST(replace_iterator_iterator_selfcstr_size);
    ADD_TEST(replace_iterator_iterator_stringcstr);
    ADD_TEST(replace_iterator_iterator_number_char);
    ADD_TEST(copy_pointer_size_position);
    ADD_TEST(member_swap);
    ADD_TEST(member_swap2);
    ADD_TEST(member_self_swap);
    ADD_TEST(member_selfcopy_swap);
    ADD_TEST(member_selfcopy_swap2);
    ADD_TEST(swap);
    ADD_TEST(swap2);
    ADD_TEST(swap_self);
    ADD_TEST(swap_selfcopy);
    ADD_TEST(swap_selfcopy2);
    ADD_TEST(cstr_data_getallocator);
    ADD_TEST(find_string_index);
    ADD_TEST(find_stringcstr_index_length);
    ADD_TEST(find_stringcstr_index);
    ADD_TEST(find_char_index);
    ADD_TEST(find_overflow);
    ADD_TEST(rfind_overflow);
    ADD_TEST(rfind_string_index);
    ADD_TEST(rfind_stringcstr_index_length);
    ADD_TEST(rfind_stringcstr_index);
    ADD_TEST(rfind_char_index);
    ADD_TEST(find_first_of_string_index);
    ADD_TEST(find_first_of_stringcstr_index_length);
    ADD_TEST(find_first_of_stringcstr_index);
    ADD_TEST(find_first_of_char_index);
    ADD_TEST(find_last_of_string_index);
    ADD_TEST(find_last_of_stringcstr_index_length);
    ADD_TEST(find_last_of_stringcstr_index);
    ADD_TEST(find_last_of_char_index);
    ADD_TEST(find_first_not_of_string_index);
    ADD_TEST(find_first_not_of_stringcstr_index_length);
    ADD_TEST(find_first_not_of_stringcstr_index);
    ADD_TEST(find_first_not_of_char_index);
    ADD_TEST(find_last_not_of_string_index);
    ADD_TEST(find_last_not_of_stringcstr_index_length);
    ADD_TEST(find_last_not_of_stringcstr_index);
    ADD_TEST(find_last_not_of_char_index);
    ADD_TEST(substr_index_length);
    ADD_TEST(compare_selfcopy);
    ADD_TEST(compare_string);
    ADD_TEST(compare_index_length_selfcopy);
    ADD_TEST(compare_index_length_string);
    ADD_TEST(compare_index_length_selfcopy_index_length);
    ADD_TEST(compare_index_length_string_index_length);
    ADD_TEST(compare_stringcstr);
    ADD_TEST(compare_index_length_stringcstr_length);
    ADD_TEST(operator_plus);
    ADD_TEST(operator_plus_lhs_cstr);
    ADD_TEST(operator_plus_lhs_char);
    ADD_TEST(operator_plus_rhs_cstr);
    ADD_TEST(operator_plus_rhs_char);
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
    String test(RandomString<String>(Tests::MaxString<String>::value));
    String result(testFunctions.getTest(count)(test));

    return result;
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
    TestFunctions<std::string>                  testFunctions_std;
    TestFunctions<my_string_SimpleStorage>      testFunctions_SimpleStorage;
    TestFunctions<my_string_AllocatorStorage>   testFunctions_AllocatorStorage;
    TestFunctions<my_string_MallocatorStorage>  testFunctions_MallocatorStorage;
    TestFunctions<my_string_VectorStorage>      testFunctions_VectorStorage;
    TestFunctions<my_string_SmallStringSimple>  testFunctions_SmallStringSimple;
    TestFunctions<my_string_SmallStringVector>  testFunctions_SmallStringVector;
    TestFunctions<my_string_CowSimple>          testFunctions_CowSimple;
    TestFunctions<my_string_CowAllocator>       testFunctions_CowAllocator;

    for (;;)
    {
      std::cout << ++count << '\r';

      const unsigned int seedForThisIteration = rand();
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
  const time_t initialSeed(1236182091 /*time(0)*/);
  srand(unsigned(initialSeed));

  std::cout << "initial seed = " << initialSeed << "\n\n";

  Compare();

  return 0;
}
