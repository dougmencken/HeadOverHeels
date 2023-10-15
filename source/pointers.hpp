// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef pointers_hpp_
#define pointers_hpp_

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>

#include <pthread.h>

#include "util.hpp"

#if defined( DEBUG ) && DEBUG
# define DEBUG_POINTERS         0
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

        void addreference ( void * pointee, const std::string & type, void * pointer ) ;

        void binreference ( void * pointee, void * pointer ) ;

        size_t countreferences ( void * pointee ) ;

        void compactreferences () ;

private:

        static references me ;

        references ( ) {  pthread_mutex_init( &mutex, nilPointer ) ;  }

        ~references ( ) ;

        std::map < void *, std::vector < void * > > entries ;

        std::map < void *, std::string > types ;

        void lockmutex () {  pthread_mutex_lock( &mutex ) ;  }
        void unlockmutex () {  pthread_mutex_unlock( &mutex ) ;  }

        pthread_mutex_t mutex ;

} ;


template < typename Type >
class multiptr
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

        multiptr( ) : pointer( nilPointer )
        {
                // don’t count references to 0

                IF_DEBUG_POINTERS( prettyprint( "/* constructor */ multiptr( )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        explicit multiptr( Type * ptr ) : pointer( ptr )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* constructor */ multiptr( TYPE * )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        multiptr( const multiptr < Type > & copy ) : pointer( copy.pointer )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* copy constructor */ multiptr( const multiptr < TYPE > & )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        template < typename ParentType >
        multiptr( const multiptr < ParentType > & copy ) : pointer( copy.getptr() )
        {
                addreference();

                IF_DEBUG_POINTERS( prettyprint( "/* ctor */ multiptr( const multiptr < " + util::demangle( typeid( ParentType ).name () ) + " > & )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )
        }

        ~multiptr( )
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

        multiptr < Type > & operator = ( const multiptr < Type > & that )
        {
                IF_DEBUG_POINTERS( prettyprint( "multiptr < TYPE > & operator = ( const multiptr < TYPE > & that )", "this = that " + that.tostring() ) )

                if ( this == &that ) return *this ;
                if ( this->pointer == that.pointer ) return *this ;

                clear () ;

                pointer = that.pointer ;

                addreference();
                IF_DEBUG_POINTERS( prettyprint( "multiptr < TYPE > & operator = ( const multiptr < TYPE > & that )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )

                return *this ;
        }

        bool operator < ( const multiptr < Type > & that ) const
                {  return ( pointer != nilPointer && that.pointer != nilPointer ) ? *pointer < *that.pointer : false ;  }

        template < typename AnyType >
        bool operator == ( const multiptr< AnyType > & that ) const {  return getptr() == that.getptr() ;  }

        template < typename AnyType >
        bool operator != ( const multiptr< AnyType > & that ) const {  return getptr() != that.getptr() ;  }

        bool operator == ( const void * that ) const {  return pointer == that ;  }

        bool operator != ( const void * that ) const {  return pointer != that ;  }

        /* implicitly static */ void * operator new ( size_t howMany )
        {
                allocations ++ ;

        #if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
                std::cout << "~.*.~ multiptr :: void * operator new ( size_t howMany = " << howMany << " )" ;
                std::cout << " it’s " << util::toStringWithOrdinalSuffix( allocations ) << " allocation" << std::endl ;
        #endif

                return std::malloc( howMany );
        }

        /* implicitly static */ void operator delete ( void * what )
        {
                deallocations ++ ;

        #if defined( DEBUG_POINTERS ) && DEBUG_POINTERS
                std::cout << "~.*.~ multiptr :: static void operator delete ( void * what = " << util::pointer2string( what ) << " )" ;
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

        multiptr < Type > & operator = ( Type * that )
        {
                IF_DEBUG_POINTERS( prettyprint( "multiptr < TYPE > & operator = ( TYPE * that )", "this = that " + ( that != nilPointer ? util::demangle( typeid( *that ).name () ) + " * " + util::pointer2string( that ) : "0" ) ) )

                if ( pointer == that ) return *this ;

                clear () ;

                pointer = that ;

                addreference();
                IF_DEBUG_POINTERS( prettyprint( "multiptr < TYPE > & operator = ( TYPE * that )", util::toStringWithOrdinalSuffix( countreferences() ) + " reference to " + util::pointer2string( pointer ) ) )

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

                out << "~.*.~ multiptr < " << nameotype << " > @" << util::pointer2string( this ) << " :: " << typizedfunction ;

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


template < typename Type > size_t multiptr< Type >::allocations = 0 ;

template < typename Type > size_t multiptr< Type >::deallocations = 0 ;


/*
template < typename ThatType, typename ThisType >
multiptr < ThatType > static_pointer_cast ( const multiptr < ThisType > & castme )
{
        return multiptr< ThatType >( static_cast< ThatType * >( castme.getptr() ) ) ;
}
*/
/*
template < typename ThatType, typename ThisType >
multiptr < ThatType > dynamic_pointer_cast ( const multiptr < ThisType > & castme )
{
        return multiptr< ThatType >( dynamic_cast< ThatType * >( castme.getptr() ) ) ;
}
*/

#endif
