
#include "pointers.hpp"

#define AUTO_COMPACT_REFERENCES         1

#if defined( DEBUG ) && DEBUG
# define DEBUG_REFERENCES       1
# define SHOW_MEMORY_LEAKS      1
#endif


references references::me ;

references & references::instance( )
{
        return references::me ;
}

void references::addreference( void * pointee, const std::string & type, void * pointer )
{
        if ( pointer != nilPointer && pointee != nilPointer ) // ignore references to nil
        {
                lockmutex ();

                std::vector< void * >& references = entries[ pointee ] ;
                if ( std::find( references.begin (), references.end (), pointer ) == references.end () )
                {
                        references.push_back( pointer ) ;
                }
                else
                {
                # if defined( DEBUG_REFERENCES ) && DEBUG_REFERENCES
                        std::cout << TERMINAL_COLOR_BOLD_BLUE << "pointer " << TERMINAL_COLOR_BOLD_RED << util::pointer2string( pointer ) <<
                                        TERMINAL_COLOR_BOLD_BLUE << " to pointee " << TERMINAL_COLOR_BOLD_BLACK << util::pointer2string( pointee ) <<
                                        TERMINAL_COLOR_BOLD_BLUE << " is already here" <<
                                        TERMINAL_COLOR_OFF << std::endl ;
                # endif
                }

                types[ pointee ] = ( ! type.empty () ) ? type : "void" ;

                unlockmutex ();
        }
}

void references::binreference( void * pointee, void * pointer )
{
# if defined( AUTO_COMPACT_REFERENCES ) && AUTO_COMPACT_REFERENCES
        static unsigned int needscompacting = 0 ;
# endif

        if ( entries.find( pointee ) != entries.end () )
        {
                lockmutex ();

                std::vector< void * >& references = entries[ pointee ] ;

                references.erase( std::remove( references.begin (), references.end (), pointer ), references.end () );

                unlockmutex ();

# if defined( AUTO_COMPACT_REFERENCES ) && AUTO_COMPACT_REFERENCES
                needscompacting ++ ;
# endif
        }

# if defined( AUTO_COMPACT_REFERENCES ) && AUTO_COMPACT_REFERENCES
        if ( needscompacting >= /* threshold of compacting */ 1048576 )
        {
                needscompacting = 0 ;
                compactreferences ();
        }
# endif
}

void references::compactreferences ()
{
        lockmutex ();

# if defined( DEBUG_REFERENCES ) && DEBUG_REFERENCES
        size_t sizebefore = entries.size() ;
# endif

        for ( std::map< void *, std::vector< void * > >::iterator e = entries.begin () ; e != entries.end () ; )
        {
                if ( e->second.empty () )
                {
                # if defined( DEBUG_REFERENCES ) && ( DEBUG_REFERENCES > 1 )
                        std::cout << TERMINAL_COLOR_BLUE << "there’s no more pointers to pointee " <<
                                        TERMINAL_COLOR_BOLD_BLUE << util::demangle( types[ e->first ] ) << " * " <<
                                        TERMINAL_COLOR_BOLD_GREEN << util::pointer2string( e->first ) <<
                                        TERMINAL_COLOR_OFF << std::endl ;
                # endif

                        types.erase( e->first );

                        std::map< void *, std::vector< void * > >::iterator en = e ;
                        ++ en ;
                        entries.erase( e );
                        e = en ;
                }
                else
                {
                        ++ e ;
                }
        }

# if defined( DEBUG_REFERENCES ) && DEBUG_REFERENCES
        std::cout << TERMINAL_COLOR_BLUE << "map of references had " <<
                        TERMINAL_COLOR_BOLD_RED << sizebefore << TERMINAL_COLOR_BLUE << " entries before compacting, and has " <<
                        TERMINAL_COLOR_BOLD_GREEN << entries.size() << TERMINAL_COLOR_BLUE << " entries after compacting"
                        TERMINAL_COLOR_OFF << std::endl ;
# endif

        unlockmutex ();
}

size_t references::countreferences( void * pointee )
{
        size_t count = 0 ;

        lockmutex ();

        if ( entries.find( pointee ) != entries.end () )
                count = entries[ pointee ].size ();

        unlockmutex ();

        return count ;
}

references::~references( )
{
#if defined( SHOW_MEMORY_LEAKS ) && SHOW_MEMORY_LEAKS

        unsigned int howMany = 0 ;

        lockmutex ();

        for ( std::map< void *, std::vector< void * > >::const_iterator e = entries.begin () ; e != entries.end () ; ++ e )
        {
                if ( ! e->second.empty () )
                {
                        std::cout << TERMINAL_COLOR_RED << "leaking " <<
                                        TERMINAL_COLOR_BOLD_BLUE << util::demangle( types[ e->first ] ) << " * " <<
                                        TERMINAL_COLOR_BOLD_RED << util::pointer2string( e->first ) << TERMINAL_COLOR_OFF ;

                        size_t refs = e->second.size () ;
                        const std::string times = ( refs == 1 ) ? "once" : util::number2string( refs ) + " times" ;

                        std::cout << TERMINAL_COLOR_RED << " referenced " << TERMINAL_COLOR_BLUE << times << TERMINAL_COLOR_RED << " by" ;

                        for ( size_t i = 0 ; i < refs ; ++ i )
                        {
                                std::cout << " " << TERMINAL_COLOR_BOLD_BLACK << util::pointer2string( e->second[ i ] ) ;
                        }

                        std::cout << TERMINAL_COLOR_OFF << std::endl ;

                        ++ howMany ;
                }
        }

        unlockmutex ();

        std::cout << TERMINAL_COLOR_BOLD_RED << howMany << TERMINAL_COLOR_RED << " objects are leaking"
                        TERMINAL_COLOR_OFF << std::endl ;
#endif

        pthread_mutex_destroy( &mutex );
}
