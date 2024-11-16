
#include "AbstractItem.hpp"

#include "Behavior.hpp"
#include "CreatorOfBehaviors.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Way.hpp"

#include <util.hpp>

#include <algorithm> // std::for_each

#include <iostream>


AbstractItem::AbstractItem( )
        : Mediated()
        , uniqueName( util::makeRandomString( 22 ) )
        , currentFrame( firstFrame() )
        , behavior( nilPointer )
        , carrier( "" )
        , transparency( 0 )
{}

AbstractItem::AbstractItem( const AbstractItem & itemToCopy )
        : Mediated( itemToCopy )
        , uniqueName( itemToCopy.uniqueName + " copy" )
        , currentFrame( itemToCopy.currentFrame )
        , behavior( nilPointer )
        , carrier( itemToCopy.carrier )
        , transparency( itemToCopy.transparency )
{
        for ( std::vector< PicturePtr >::const_iterator it = itemToCopy.frames.begin (); it != itemToCopy.frames.end (); ++ it )
        {
                this->frames.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }

        for ( std::vector< PicturePtr >::const_iterator it = itemToCopy.shadows.begin (); it != itemToCopy.shadows.end (); ++ it )
        {
                this->shadows.push_back( PicturePtr( new Picture( ( *it )->getAllegroPict () ) ) );
        }
}

AbstractItem::~AbstractItem( )
{
        clearFrames() ;
}

bool AbstractItem::updateItem ()
{
        return ( this->behavior != nilPointer ) ? this->behavior->update() : true ;
}

void AbstractItem::changeFrame( unsigned int newFrame )
{
        if ( newFrame == this->currentFrame ) return ;

        if ( newFrame < howManyFrames() )
        {
                this->currentFrame = newFrame ;

                updateImage() ;

                if ( ! shadows.empty() ) updateShadow ();
        }
}

bool AbstractItem::doGraphicsOverlapAt( const AbstractItem & item, int x, int y ) const
{
        int thisImageWidth = this->getRawImage().getWidth() ;
        int thisImageHeight = this->getRawImage().getHeight() ;
        int thatImageWidth = item.getRawImage().getWidth() ;
        int thatImageHeight = item.getRawImage().getHeight() ;

        return  ( this->getImageOffsetX() < x + thatImageWidth ) && ( x < this->getImageOffsetX() + thisImageWidth )
                        && ( this->getImageOffsetY() < y + thatImageHeight ) && ( y < this->getImageOffsetY() + thisImageHeight ) ;
}

void AbstractItem::setBehaviorOf( const std::string & nameOfBehavior )
{
        this->behavior = behaviors::CreatorOfBehaviors::createBehaviorByName( * this , nameOfBehavior );
}

