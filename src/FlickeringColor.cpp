
#include "FlickeringColor.hpp"
#include "Ism.hpp"


const FlickeringColor * FlickeringColor::whiteAndTransparent = new FlickeringColor( Color::whiteColor(), Color() ) ;

const FlickeringColor * FlickeringColor::gray75AndTransparent = new FlickeringColor( Color::byName( "gray75" ), Color() ) ;


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
                        thiz->changeColorTimer->reset ();
                }

                // no te comas la CPU
                // do not eat the CPU
                milliSleep( thiz->period * 0.99 * 1000 );
        }

        pthread_exit( nilPointer );
}

void FlickeringColor::initTimer()
{
        changeColorTimer = new Timer();
        changeColorTimer->go();

        pthread_create( &thread, nilPointer, FlickeringColor::flicker, reinterpret_cast< void * >( this ) );
}
