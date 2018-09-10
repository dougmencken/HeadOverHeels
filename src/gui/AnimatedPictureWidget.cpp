
#include "AnimatedPictureWidget.hpp"
#include <iostream>


namespace gui
{

AnimatedPictureWidget::AnimatedPictureWidget( int x, int y, std::vector < allegro::Pict * > frames, double delay, std::string name ) :
        Widget( x, y )
        , animation( frames )
        , delayBetweenFrames( delay )
        , theFrame( 0 )
        , nameOfAnimation( name )
{
        animationTimer = new Timer();
        animationTimer->go();
}

AnimatedPictureWidget::~AnimatedPictureWidget()
{
        for ( std::vector< allegro::Pict * >::iterator bb = animation.begin (); bb != animation.end (); ++bb )
        {
                allegro::binPicture( *bb );
        }
        animation.clear();

        delete animationTimer;
}

void AnimatedPictureWidget::draw( allegro::Pict* where )
{
        if ( animation.size() > 0 )
        {
                allegro::drawSprite( where, animation[ theFrame ], getX(), getY() );
        }

        if ( animationTimer->getValue() > delayBetweenFrames )
        {
                theFrame++;
                if ( theFrame >= animation.size() ) theFrame = 0;
                animationTimer->reset();
        }
}

unsigned int AnimatedPictureWidget::getWidth () const
{
        if ( animation.size() > 0 )
                return ( * animation.begin () )->w ;
        else
                return 0 ;
}

unsigned int AnimatedPictureWidget::getHeight () const
{
        if ( animation.size() > 0 )
                return ( * animation.begin () )->h ;
        else
                return 0 ;
}

}
