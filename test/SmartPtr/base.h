////////////////////////////////////////////////////////////////////////////////
// Test program for The Loki Library
// Copyright (c) 2006 Richard Sposato
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The authors make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Header$


// ----------------------------------------------------------------------------

class BaseClass
{
public:
    BaseClass( void )
    {
        s_constructions++;
    }

    virtual ~BaseClass( void )
    {
        s_destructions++;
    }

    // These 2 functions are so we can pretend we have a COM object.
    void AddRef( void ) {}
    void Release( void ) {}

    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass();
    }

    static inline bool AllDestroyed( void )
    {
        return ( s_constructions == s_destructions );
    }

    static inline bool ExtraConstructions( void )
    {
        return ( s_constructions > s_destructions );
    }

    static inline bool ExtraDestructions( void )
    {
        return ( s_constructions < s_destructions );
    }

    static inline unsigned int GetCtorCount( void )
    {
        return s_constructions;
    }

    static inline unsigned int GetDtorCount( void )
    {
        return s_destructions;
    }

private:
    /// Not implemented.
    BaseClass( const BaseClass & );
    /// Not implemented.
    BaseClass & operator = ( const BaseClass & );

    static unsigned int s_constructions;
    static unsigned int s_destructions;
};


// ----------------------------------------------------------------------------

class PublicSubClass : public BaseClass
{
public:
    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass;
    }
};

// ----------------------------------------------------------------------------

class PrivateSubClass : private BaseClass
{
public:
    // This function is used only for the DeepCopy policy.
    virtual BaseClass * Clone( void ) const
    {
        return new BaseClass;
    }
};

// ----------------------------------------------------------------------------

// $Log$
// Revision 1.1  2006/03/20 21:14:16  rich_sposato
// Adding base.h to CVS.
//
