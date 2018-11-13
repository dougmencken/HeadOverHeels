// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef pointers_hpp_
#define pointers_hpp_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>

#include "util.hpp"

#if defined( DEBUG ) && DEBUG
# define DEBUG_POINTERS         0
# define SHOW_MEMORY_LEAKS      1
#endif

#if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
    #define IF_DEBUG_POINTERS( line )   line ;
#else
    #define IF_DEBUG_POINTERS( line )
#endif


template < typename Type >
class justptr
{

private:

        Type * pointer ;

public:

        justptr( ) : pointer( nilPointer ) { }
        justptr( Type * ptr ) : pointer( ptr ) { }
        justptr( const justptr < Type > & copy ) : pointer( copy.pointer ) { }

        ~justptr( ) { }

        operator Type * () const {  return pointer ;  }
        Type * operator -> () const {  return pointer ;  }
        Type & operator * () const {  return *pointer ;  }

        bool operator < ( const justptr < Type > & that ) const
                {  return ( pointer != nilPointer && that.pointer != nilPointer ) ? *pointer < *that.pointer : false ;  }

} ;


class references
{

public:

        static references & instance () ;

        void addreference ( void * pointee, const char * type, void * pointer ) ;

        void binreference ( void * pointee, void * pointer ) ;

        size_t countreferences ( void * pointee ) ;

private:

        static references me ;

        references ( ) : entries ()  { }

        ~references ( ) ;

        std::map < void *, std::vector < void * > > entries ;

        std::map < void *, std::string > types ;

} ;


template < typename Type >
class safeptr
{

private:

        Type * pointer ;

        inline void addreference ()
        {
                if ( pointer != nilPointer )
                        references::instance().addreference( pointer, typeid( *pointer ).name (), this ) ;
        }

        inline void binreference ()
        {
                references::instance().binreference( pointer, this ) ;
        }

        inline size_t countreferences ()
        {
                return references::instance().countreferences( pointer ) ;
        }

        static size_t allocations ;

        static size_t deallocations ;

public:

        safeptr( ) : pointer( nilPointer )
        {
                // don’t count references to 0

                IF_DEBUG_POINTERS( prettyprint( "/* constructor */ safeptr( )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        explicit safeptr( Type * ptr ) : pointer( ptr )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* constructor */ safeptr( TYPE * )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        safeptr( const safeptr < Type > & copy ) : pointer( copy.pointer )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* copy constructor */ safeptr( const safeptr < TYPE > & )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        template < typename ParentType >
        safeptr( const safeptr < ParentType > & copy ) : pointer( copy.getptr() )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* ctor */ safeptr( const safeptr < " + util::demangle( typeid( ParentType ).name () ) + " > & )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        ~safeptr( )
        {
                clear () ;
        }

        void clear ()
        {
                binreference();

                if ( pointer != nilPointer )
                {
                        size_t howmanyrefs = countreferences();
                        IF_DEBUG_POINTERS( prettyprint( "clear ()", util::number2string( howmanyrefs ) + " remaining references to " + util::pointer2string( pointer ) ) )

                        if ( howmanyrefs == 0 )
                        {
                                IF_DEBUG_POINTERS( prettyprint( "clear ()", "deleting pointer " + util::pointer2string( pointer ) ) )
                                delete pointer ;
                        }

                        pointer = nilPointer ;
                }
        }

        Type * getptr () const {  return pointer ;  }

        Type & operator * () const
        {
                if ( pointer == nilPointer )
                {
                        IF_DEBUG_POINTERS( prettyprint( "TYPE & operator * () const", "dereferencing NULL" ) )
                        std::cerr << "dereferencing NULL" << std::endl ;
                        util::printBacktrace ();
                }

                assert ( pointer != nilPointer ) ;
                return *pointer ;
        }

        Type * operator -> () const
        {
                if ( pointer == nilPointer )
                {
                        IF_DEBUG_POINTERS( prettyprint( "TYPE * operator -> () const", "dereferencing NULL" ) )
                        std::cerr << "dereferencing NULL" << std::endl ;
                        util::printBacktrace ();
                }

                assert ( pointer != nilPointer ) ;
                return pointer ;
        }

        safeptr < Type > & operator = ( const safeptr < Type > & that )
        {
                IF_DEBUG_POINTERS( prettyprint( "safeptr < TYPE > & operator = ( const safeptr < TYPE > & that )", "this = that " + that.tostring() ) )

                if ( this == &that ) return *this ;
                if ( this->pointer == that.pointer ) return *this ;

                clear () ;

                pointer = that.pointer ;

                addreference();
                IF_DEBUG_POINTERS( prettyprint( "safeptr < TYPE > & operator = ( const safeptr < TYPE > & that )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )

                return *this ;
        }

        bool operator < ( const safeptr < Type > & that ) const
                {  return ( pointer != nilPointer && that.pointer != nilPointer ) ? *pointer < *that.pointer : false ;  }

        template < typename AnyType >
        bool operator == ( const safeptr< AnyType > & that ) const {  return getptr() == that.getptr() ;  }

        template < typename AnyType >
        bool operator != ( const safeptr< AnyType > & that ) const {  return getptr() != that.getptr() ;  }

        bool operator == ( const void * that ) const {  return pointer == that ;  }

        bool operator != ( const void * that ) const {  return pointer != that ;  }

        /* implicitly static */ void * operator new ( size_t howMany )
        {
                allocations ++ ;

        #if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
                std::cout << "~.*.~ safeptr :: void * operator new ( size_t howMany = " << howMany << " )" ;
                std::cout << " it’s " << util::toStringWithOrdinalSuffix( allocations ) << " allocation" << std::endl ;
        #endif

                return std::malloc( howMany );
        }

        /* implicitly static */ void operator delete ( void * what )
        {
                deallocations ++ ;

        #if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
                std::cout << "~.*.~ safeptr :: static void operator delete ( void * what = " << util::pointer2string( what ) << " )" ;
                std::cout << " it’s " << util::toStringWithOrdinalSuffix( deallocations ) << " deallocation" << std::endl ;
        #endif

                std::free( what );
        }

        std::string tostring () const
        {
                return "{ pointer " + util::pointer2string( pointer ) + " }" ;
        }

private:

        operator Type * () const
        {
                IF_DEBUG_POINTERS( prettyprint( "operator TYPE * () const" ) )
                return pointer ;
        }

        safeptr < Type > & operator = ( Type * that )
        {
                IF_DEBUG_POINTERS( prettyprint( "safeptr < TYPE > & operator = ( TYPE * that )", "this = that " + ( that != nilPointer ? util::demangle( typeid( *that ).name () ) + " * " + util::pointer2string( that ) : "0" ) ) )

                if ( pointer == that ) return *this ;

                clear () ;

                pointer = that ;

                addreference();
                IF_DEBUG_POINTERS( prettyprint( "safeptr < TYPE > & operator = ( TYPE * that )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )

                return *this ;
        }

#if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
        void prettyprint( std::ostream& out,
                          const std::string& function,
                          const std::string& message = std::string(),
                          bool newline = true ) const
        {
                std::string nameothis = util::demangle( typeid( *this ).name () );
                std::string nameotype = nameothis ;
                if ( nameothis.find( "<" ) != std::string::npos && nameothis.find( ">" ) != std::string::npos )
                        nameotype = nameothis.substr( nameothis.find( "<" ) + 1, nameothis.find( ">" ) - nameothis.find( "<" ) - 1 );

                std::string typizedfunction = function ;
                std::string type = "TYPE" ;
                while ( typizedfunction.find( type ) != std::string::npos )
                        typizedfunction.replace( typizedfunction.find( type ), type.length (), nameotype );

                out << "~.*.~ safeptr < " << nameotype << " > @" << util::pointer2string( this ) << " :: " << typizedfunction ;

                out << " " << tostring () ;

                if ( ! message.empty() ) out << " " << message ;

                if ( newline ) out << std::endl ;
        }

        void prettyprint( const std::string& function,
                          const std::string& message = std::string(),
                          bool newline = true ) const
        {
                prettyprint( std::cout, function, message, newline );
        }
#endif

} ;


template < typename Type > size_t safeptr< Type >::allocations = 0 ;

template < typename Type > size_t safeptr< Type >::deallocations = 0 ;


/*
template < typename ThatType, typename ThisType >
safeptr < ThatType > static_pointer_cast ( const safeptr < ThisType > & castme )
{
        return safeptr< ThatType >( static_cast< ThatType * >( castme.getptr() ) ) ;
}
*/
/*
template < typename ThatType, typename ThisType >
safeptr < ThatType > dynamic_pointer_cast ( const safeptr < ThisType > & castme )
{
        return safeptr< ThatType >( dynamic_cast< ThatType * >( castme.getptr() ) ) ;
}
*/

#endif
