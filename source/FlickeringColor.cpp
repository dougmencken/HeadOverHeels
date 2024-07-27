
#include "FlickeringColor.hpp"

#include "sleep.hpp" // for milliSleep


/* static */ const FlickeringColor * FlickeringColor::whiteAndTransparent = nilPointer ;

/* static */ const FlickeringColor * FlickeringColor::gray75AndTransparent = nilPointer ;

/* static */
const FlickeringColor & FlickeringColor::flickerWhiteAndTransparent ()
{
        if ( FlickeringColor::whiteAndTransparent == nilPointer )
                FlickeringColor::whiteAndTransparent = new FlickeringColor( Color::whiteColor(), Color::keyColor() );

        assert( FlickeringColor::whiteAndTransparent != nilPointer );
        return * FlickeringColor::whiteAndTransparent ;
}

/* static */
const FlickeringColor & FlickeringColor::flickerGray75AndTransparent ()
{
        if ( FlickeringColor::gray75AndTransparent == nilPointer )
                FlickeringColor::gray75AndTransparent = new FlickeringColor( Color::byName( "gray75" ), Color::keyColor() ) ;

        assert( FlickeringColor::gray75AndTransparent != nilPointer );
        return * FlickeringColor::gray75AndTransparent ;
}

FlickeringColor::~FlickeringColor( )
{
        pthread_join( thread, nilPointer );
}

std::string FlickeringColor::toString() const
{
        return "flickering color: first " + first.toString() + " second " + second.toString() ;
}

/* static */ void* FlickeringColor::flicker( void* flickeringColor )
{
        FlickeringColor* thiz = reinterpret_cast< FlickeringColor* >( flickeringColor );

        while ( true )
        {
                if ( thiz->changeColorTimer->getValue() > thiz->period )
                {
                        thiz->isFirst = ! thiz->isFirst ;
                        thiz->changeColorTimer->go ();
                }

                // no te comas la CPU
                // do not eat the CPU
                somn::milliSleep( thiz->period * 0.99 * 1000 );
        }

        pthread_exit( nilPointer );
}

void FlickeringColor::initTimer()
{
        changeColorTimer = new Timer();
        changeColorTimer->go();

        pthread_create( &thread, nilPointer, FlickeringColor::flicker, reinterpret_cast< void * >( this ) );
}
