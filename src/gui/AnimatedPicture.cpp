
#include "AnimatedPicture.hpp"
#include <iostream>


namespace gui
{

AnimatedPicture::AnimatedPicture( int x, int y, std::vector < BITMAP * > frames, double delay, std::string name ) :
        Widget( x, y )
        , animation( frames )
        , delayBetweenFrames( delay )
        , theFrame( 0 )
        , nameOfAnimation( name )
{
        animationTimer = new Timer();
        animationTimer->go();
}

AnimatedPicture::~AnimatedPicture()
{
        /// std::cout << "forget animation with name \"" << nameOfAnimation << "\"" << std::endl ;

        for ( std::vector< BITMAP * >::iterator bb = animation.begin (); bb != animation.end (); ++bb )
        {
                destroy_bitmap( *bb );
        }
        animation.clear();

        delete animationTimer;
}

void AnimatedPicture::draw( BITMAP* where )
{
        if ( animation.size() > 0 )
        {
                draw_sprite( where, animation[ theFrame ], getX(), getY() );
        }

        if ( animationTimer->getValue() > delayBetweenFrames )
        {
                theFrame++;
                if ( theFrame >= animation.size() ) theFrame = 0;
                animationTimer->reset();
        }
}

unsigned int AnimatedPicture::getWidth () const
{
        if ( animation.size() > 0 )
                return ( * animation.begin () )->w ;
        else
                return 0 ;
}

unsigned int AnimatedPicture::getHeight () const
{
        if ( animation.size() > 0 )
                return ( * animation.begin () )->h ;
        else
                return 0 ;
}

}
