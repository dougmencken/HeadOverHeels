
#include "AnimatedPictureWidget.hpp"

#include <iostream>
#include <algorithm> // std::for_each


namespace gui
{

AnimatedPictureWidget::AnimatedPictureWidget( int x, int y, const std::vector< allegro::Pict* >& frames, double delay, const std::string& name ) :
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
        std::for_each( animation.begin (), animation.end (), isomot::DeleteIt() );
        animation.clear();

        delete animationTimer;
}

void AnimatedPictureWidget::draw( const allegro::Pict& where )
{
        if ( animation.size() > 0 )
        {
                allegro::drawSprite( where, *animation[ theFrame ], getX(), getY() );
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
                return ( * animation.begin () )->getW() ;
        else
                return 0 ;
}

unsigned int AnimatedPictureWidget::getHeight () const
{
        if ( animation.size() > 0 )
                return ( * animation.begin () )->getH() ;
        else
                return 0 ;
}

}
