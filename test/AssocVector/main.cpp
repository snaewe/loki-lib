////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2011 by Rich Sposato
//
// This code does not accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <loki/AssocVector.h>

#include <cassert>

#include <string>
#include <iostream>

using namespace std;

typedef ::std::map< ::std::string, unsigned int > StudentGradeMap;
typedef StudentGradeMap::iterator StudentGradeMapIter;
typedef StudentGradeMap::const_iterator StudentGradeMapCIter;

typedef ::Loki::AssocVector< ::std::string, unsigned int > StudentGrades;
typedef StudentGrades::iterator StudentGradeIter;
typedef StudentGrades::const_iterator StudentGradeCIter;

typedef ::std::pair< ::std::string, unsigned int > GradeInfo;


GradeInfo oneStudent = ::std::make_pair( "Anne", 100 );

GradeInfo noDuplicates[] =
{
    ::std::make_pair( "Anne", 100 ),
    ::std::make_pair( "Bill",  88 ),
    ::std::make_pair( "Clay",  91 ),
    ::std::make_pair( "Dina",  62 ),
    ::std::make_pair( "Evan",  77 ),
    ::std::make_pair( "Fran",  84 ),
    ::std::make_pair( "Greg",  95 )
};


GradeInfo hasDuplicates[] =
{
    ::std::make_pair( "Anne", 100 ),
    ::std::make_pair( "Anne",  73 ),
    ::std::make_pair( "Bill",  88 ),
    ::std::make_pair( "Clay",  91 ),
    ::std::make_pair( "Dina",  62 ),
    ::std::make_pair( "Evan",  77 ),
    ::std::make_pair( "Fran",  74 ),
    ::std::make_pair( "Fran",  84 ),
    ::std::make_pair( "Greg",  95 )
};

// ----------------------------------------------------------------------------

void TestEmptyAssocVector( void )
{
    cout << "Starting TestEmptyAssocVector" << endl;

    StudentGrades grades;
    const StudentGrades & cGrades = grades;
    assert( grades.empty() );
    assert( grades.size() == 0 );
    assert( cGrades.empty() );
    assert( cGrades.size() == 0 );

    StudentGradeIter it1( grades.begin() );
    assert( it1 == grades.end() );

    const StudentGradeIter it2( grades.begin() );
    assert( it2 == grades.end() );
    assert( it2 == it1 );
    assert( it1 == it2 );

    StudentGradeCIter cit1( grades.begin() );
    assert( cit1 == grades.end() );
    assert( cit1 == it1 );
    assert( cit1 == it2 );
    assert( it1 == cit1 );
    assert( it2 == cit1 );

    const StudentGradeCIter cit2( grades.begin() );
    assert( cit2 == grades.end() );
    assert( cit1 == cit2 );
    assert( cit2 == cit1 );
    assert( cit2 == it1 );
    assert( cit2 == it2 );
    assert( it1 == cit2 );
    assert( it2 == cit2 );

    StudentGradeCIter cit3( cGrades.begin() );
    assert( cit3 == cGrades.end() );
    assert( cit3 == it1 );
    assert( cit3 == it2 );
    assert( it1 == cit3 );
    assert( it2 == cit3 );

    const StudentGradeCIter cit4( cGrades.begin() );
    assert( cit4 == cGrades.end() );
    assert( cit1 == cit4 );
    assert( cit4 == cit1 );
    assert( cit4 == it1 );
    assert( cit4 == it2 );
    assert( it1 == cit4 );
    assert( it2 == cit4 );

    cout << "Finished TestEmptyAssocVector" << endl;
}

// ----------------------------------------------------------------------------

void TestAssocVectorCtor( void )
{
    cout << "Starting TestAssocVectorCtor" << endl;

    static const unsigned int  noDuplicateCount = ( sizeof(noDuplicates)  / sizeof(noDuplicates[0])  );
    static const unsigned int hasDuplicateCount = ( sizeof(hasDuplicates) / sizeof(hasDuplicates[0]) );

    {
        // This test demonstrates the iterator constructor does not allow any duplicate elements.
        StudentGrades grades1(  noDuplicates,  noDuplicates +  noDuplicateCount );
        StudentGrades grades2( hasDuplicates, hasDuplicates + hasDuplicateCount );
        assert( grades1.size() != 0 );
        assert( grades2.size() != 0 );
        assert( grades1.size() == noDuplicateCount );
        assert( grades2.size() == noDuplicateCount );
        assert( grades1.size() == grades2.size() );
    }

    {
        // This test demonstrates copy construction.
        StudentGrades grades1( noDuplicates, noDuplicates + noDuplicateCount );
        const StudentGrades grades2( grades1 );
        assert( grades1.size() != 0 );
        assert( grades2.size() != 0 );
        assert( grades1.size() == noDuplicateCount );
        assert( grades2.size() == noDuplicateCount );
        assert( grades1.size() == grades2.size() );
        assert( grades1 == grades2 );

        StudentGrades grades3;
        grades3 = grades1;
        assert( grades3.size() != 0 );
        assert( grades3.size() == noDuplicateCount );
        assert( grades3.size() == grades1.size() );
        assert( grades1 == grades3 );
    }

    cout << "Finished TestAssocVectorCtor" << endl;
}

// ----------------------------------------------------------------------------

int main( int argc, const char * const argv[] )
{
    (void)argc;
    (void)argv;
    char ender;

    TestEmptyAssocVector();
    TestAssocVectorCtor();

    cout << "Press <Enter> key to finish. " << endl;
    cin.get( ender );

    return 0;
}

// ----------------------------------------------------------------------------
