////////////////////////////////////////////////////////////////////////////////
//
// This is a test program for Safe-Bit-Fields in the Loki Library.
// Copyright (c) 2009 by Fedor Pikus & Rich Sposato
// The copyright on this file is protected under the terms of the MIT license.
//
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no claims about the suitability of this software for any
//     purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$


#include <loki/SafeBits.h>

#include <iostream>

using namespace std;
using namespace Loki;


LOKI_BIT_FIELD( unsigned int ) Cat_state;
LOKI_BIT_CONST( Cat_state, CAT_NO_STATE, 0 );                        // 0x0000 - no bit is set
LOKI_BIT_CONST( Cat_state, CAT_SLEEPING, 1 );                        // 0x0001 - 1st bit is set
LOKI_BIT_CONST( Cat_state, CAT_PURRING,  2 );                        // 0x0002 - 2nd bit is set
LOKI_BIT_CONST( Cat_state, CAT_PLAYING,  3 );                        // 0x0004 - 3rd bit is set
LOKI_BIT_FIELD( unsigned int ) Dog_state;
LOKI_BIT_CONST( Dog_state, DOG_BARKING,  1 );
LOKI_BIT_CONST( Dog_state, DOG_CHEWING,  2 );
LOKI_BIT_CONST( Dog_state, DOG_DROOLING, 3 );
LOKI_BIT_CONST( Dog_state, DOG_SLEEPING, 4 );
LOKI_BIT_CONST( Dog_state, DOG_GROWLING, 5 );
LOKI_BIT_CONST( Dog_state, DOG_TIRED,    6 );
LOKI_BIT_CONST( Dog_state, DOG_DEAD,     7 );
LOKI_BIT_CONST( Dog_state, DOG_GROWING,  8 );
LOKI_BIT_CONST( Dog_state, DOG_THINKING, 9 );
LOKI_BIT_CONST( Dog_state, DOG_YELPING,  10 );
LOKI_BIT_CONST( Dog_state, DOG_QUIET,    11 );
LOKI_BIT_CONST( Dog_state, DOG_FETCHING, 12 );
LOKI_BIT_CONST( Dog_state, DOG_HOWLING,  13 );
LOKI_BIT_CONST( Dog_state, DOG_SICK,     14 );
LOKI_BIT_CONST( Dog_state, DOG_JUMPING,  15 );
LOKI_BIT_CONST( Dog_state, DOG_SWIMMING, 16 );
LOKI_BIT_CONST( Dog_state, DOG_BATHING,  17 );
LOKI_BIT_CONST( Dog_state, DOG_EATING,   18 );
LOKI_BIT_CONST( Dog_state, DOG_DRINKING, 19 );
LOKI_BIT_CONST( Dog_state, DOG_PLAYING,  20 );
LOKI_BIT_CONST( Dog_state, DOG_RUNNING,  21 );
LOKI_BIT_CONST( Dog_state, DOG_BITING,   22 );
LOKI_BIT_CONST( Dog_state, DOG_BEGGING,  23 );
LOKI_BIT_CONST( Dog_state, DOG_WHINING,  24 );
LOKI_BIT_CONST( Dog_state, DOG_WALKING,  25 );
LOKI_BIT_CONST( Dog_state, DOG_SINGING,  26 );
LOKI_BIT_CONST( Dog_state, DOG_FIGHTING, 27 );
LOKI_BIT_CONST( Dog_state, DOG_MATING,   28 );
LOKI_BIT_CONST( Dog_state, DOG_FEEDING,  29 );
LOKI_BIT_CONST( Dog_state, DOG_BIRTHING, 30 );
LOKI_BIT_CONST( Dog_state, DOG_SHEDDING, 31 );
LOKI_BIT_CONST( Dog_state, DOG_TALKING,  32 );

#ifdef ERROR0
    LOKI_BIT_CONST( Dog_state, DOG_TALKING,  20 ); // Can't have two values with same name.
    LOKI_BIT_CONST( Dog_state, DOG_BLOGGING, 33 ); // Can't set bit 33 in a 32 bit-sized object.
#endif

int main( void )
{
    cout << "Running tests on Loki safe bit fields." << endl;

    Cat_state cat_state = CAT_SLEEPING;
    assert( cat_state );
    Dog_state dog_state = DOG_DROOLING;
    assert( dog_state );
    bool happy = cat_state & ( CAT_SLEEPING | CAT_PURRING );    // OK
    assert( happy );

    assert( CAT_SLEEPING <  CAT_PURRING  );
    assert( CAT_SLEEPING <= CAT_SLEEPING );
    assert( CAT_SLEEPING <= CAT_PURRING  );
    assert( CAT_SLEEPING != CAT_PURRING  );
    assert( CAT_SLEEPING == CAT_SLEEPING );
    assert( CAT_PURRING  >= CAT_SLEEPING );
    assert( CAT_PURRING  >= CAT_PURRING  );

#ifdef ERROR1
    assert( DOG_DROOLING != CAT_SLEEPING );                   // Can't compare different types.
#endif
#ifdef ERROR2
    if ( cat_state & DOG_BARKING ) {}                         // Wrong bit type
#endif
#ifdef ERROR3
    if ( cat_state & CAT_SLEEPING == 0 ) {}                   // Operator precedence
#endif
#ifdef ERROR4
    if ( dog_state && DOG_BARKING ) {}                        // Logical &&
#endif
    if ( dog_state & DOG_BARKING ) {}                         // OK
#ifdef ERROR5
    Cat_state state0 = 0;                                     // Conversion from non-safe bits
#endif

    Cat_state state = CAT_NO_STATE;                           // OK
    assert( !state );
    state = ~state;
    assert( state );
    assert( state != CAT_NO_STATE );
    assert( state & CAT_SLEEPING );
    assert( state & CAT_PURRING );
    assert( state & CAT_PLAYING );
    state = CAT_SLEEPING;
    assert( state == cat_state );
    assert( state.size() == ( 8 * sizeof(unsigned int) ) );
    assert( sizeof(Cat_state) == sizeof(unsigned int) );

    dog_state = DOG_BARKING;
#ifdef ERROR6
    if ( dog_state == cat_state ) {}                          // Don't allow comparison of different types.
#endif


/// @note All These assertions are inside #ifdef sections because they
/// compare either SafeBitField or SafeBitConst to literal integers.
/// If you compile any of these assertions they should generate errors.
/// These #ifdef sections exhaustively demonstrate that all possible
/// operations and comparisons with literal values are forbidden.

#ifdef ERROR7
    assert( dog_state == 1 );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR8
    assert( dog_state != 2 );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR9
    assert( dog_state < 2 );                                  // Don't allow comparisons to integers.
#endif
#ifdef ERROR10
    assert( dog_state > 0 );                                  // Don't allow comparisons to integers.
#endif
#ifdef ERROR11
    assert( dog_state <= 1 );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR12
    assert( dog_state >= 1 );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR13
    assert( DOG_BARKING == 1 );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR14
    assert( DOG_BARKING != 2 );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR15
    assert( DOG_BARKING < 2 );                                // Don't allow comparisons to integers.
#endif
#ifdef ERROR16
    assert( DOG_BARKING > 0 );                                // Don't allow comparisons to integers.
#endif
#ifdef ERROR17
    assert( DOG_BARKING <= 1 );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR18
    assert( DOG_BARKING >= 1 );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR19
    assert( ( dog_state | 1 ) != 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR20
    assert( ( dog_state & 2 ) == 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR21
    assert( ( dog_state ^ 1 ) == 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR22
    assert( ( dog_state |= 2 ) == 3 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR23
    assert( ( dog_state &= 3 ) == 2 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR24
    assert( ( dog_state ^= 1 ) == 0 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR25
    assert( ( DOG_BARKING | 1 ) != 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR26
    assert( ( DOG_BARKING & 2 ) == 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR27
    assert( ( DOG_BARKING ^ 1 ) == 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR28
    assert( ( DOG_BARKING |= 2 ) == 3 );                      // Don't allow operations with integers.
#endif
#ifdef ERROR29
    assert( ( DOG_BARKING &= 3 ) == 2 );                      // Don't allow operations with integers.
#endif
#ifdef ERROR30
    assert( ( DOG_BARKING ^= 1 ) == 0 );                      // Don't allow operations with integers.
#endif


/// @note All These assertions are inside #ifdef sections because they
/// compare either SafeBitField or SafeBitConst to an int variable.
/// If you compile any of these assertions they should generate errors.
/// These #ifdef sections exhaustively demonstrate that all possible
/// operations and comparisons with integers are forbidden.

    int value = 1;
    (void)value;
#ifdef ERROR31
    assert( dog_state == value );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR32
    value = 2;
    assert( dog_state != value );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR33
    value = 2;
    assert( dog_state < value );                                  // Don't allow comparisons to integers.
#endif
#ifdef ERROR34
    value = 0;
    assert( dog_state > value );                                  // Don't allow comparisons to integers.
#endif
#ifdef ERROR35
    value = 1;
    assert( dog_state <= value );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR36
    value = 1;
    assert( dog_state >= value );                                 // Don't allow comparisons to integers.
#endif
#ifdef ERROR37
    value = 1;
    assert( DOG_BARKING == value );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR38
    value = 2;
    assert( DOG_BARKING != value );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR39
    value = 2;
    assert( DOG_BARKING < value );                                // Don't allow comparisons to integers.
#endif
#ifdef ERROR40
    value = 0;
    assert( DOG_BARKING > value );                                // Don't allow comparisons to integers.
#endif
#ifdef ERROR41
    value = 1;
    assert( DOG_BARKING <= value );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR42
    value = 1;
    assert( DOG_BARKING >= value );                               // Don't allow comparisons to integers.
#endif
#ifdef ERROR43
    value = 1;
    assert( ( dog_state | value ) != 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR44
    value = 2;
    assert( ( dog_state & value ) == 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR45
    value = 1;
    assert( ( dog_state ^ value ) == 0 );                         // Don't allow operations with integers.
#endif
#ifdef ERROR46
    value = 2;
    assert( ( dog_state |= value ) == 3 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR47
    value = 3;
    assert( ( dog_state &= value ) == 2 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR48
    value = 1;
    assert( ( dog_state ^= value ) == 0 );                        // Don't allow operations with integers.
#endif
#ifdef ERROR49
    value = 1;
    assert( ( DOG_BARKING | value ) != 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR50
    value = 2;
    assert( ( DOG_BARKING & value ) == 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR51
    value = 1;
    assert( ( DOG_BARKING ^ value ) == 0 );                       // Don't allow operations with integers.
#endif
#ifdef ERROR52
    value = 2;
    assert( ( DOG_BARKING |= value ) == 3 );                      // Don't allow operations with integers.
#endif
#ifdef ERROR53
    value = 3;
    assert( ( DOG_BARKING &= value ) == 2 );                      // Don't allow operations with integers.
#endif
#ifdef ERROR54
    value = 1;
    assert( ( DOG_BARKING ^= value ) == 0 );                      // Don't allow operations with integers.
#endif


    dog_state |= DOG_CHEWING;
    assert( dog_state & ( DOG_CHEWING | DOG_BARKING ) );
    dog_state &= DOG_CHEWING;
    assert( dog_state == DOG_CHEWING );
    dog_state = ~dog_state;
    assert( dog_state != DOG_CHEWING );
    dog_state = ~dog_state;
    assert( dog_state == DOG_CHEWING );

    cout << "If nothing asserted, then all tests passed!" << endl;
    return 0;
}
