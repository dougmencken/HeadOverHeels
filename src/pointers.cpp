
#include "pointers.hpp"


references references::me ;

references & references::instance( )
{
        return references::me ;
}

void references::addreference( void * pointee, const char * type, void * pointer )
{
        if ( pointer != nilPointer && pointee != nilPointer )
        {
                entries[ pointee ].push_back( pointer ) ;
                types[ pointee ] = ( type != nilPointer ) ? type : "void" ;
        }
}

void references::binreference( void * pointee, void * pointer )
{
        if ( entries.find( pointee ) != entries.end () )
                entries[ pointee ].erase( std::remove( entries[ pointee ].begin (), entries[ pointee ].end (), pointer ), entries[ pointee ].end() );
}

size_t references::countreferences( void * pointee )
{
        return ( entries.find( pointee ) != entries.end () ) ? entries[ pointee ].size () : 0 ;
}

references::~references( )
{
#if defined( SHOW_MEMORY_LEAKS ) && SHOW_MEMORY_LEAKS
        for ( std::map< void *, std::vector< void * > >::const_iterator e = entries.begin () ; e != entries.end () ; ++ e )
        {
                if ( ! e->second.empty () )
                {
                        size_t refs = e->second.size () ;

                        std::cout << "** leaking " <<
                                        util::demangle( types[ e->first ] ) << " * " << util::pointer2string( e->first ) <<
                                        " referenced " << refs << " times by" ;

                        for ( size_t i = 0 ; i < refs ; ++ i )
                        {
                                std::cout << " " << util::pointer2string( e->second[ i ] ) ;
                        }

                        std::cout << std::endl ;
                }
        }
#endif
}
