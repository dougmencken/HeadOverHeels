
#include "AbstractItem.hpp"

#include "Behavior.hpp"
#include "CreatorOfBehaviors.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Way.hpp"

#include "util.hpp"
#include "NoSuchPictureException.hpp"

#include <algorithm> // std::for_each

#include <iostream>


AbstractItem::AbstractItem( )
        : Mediated()
        , uniqueName( util::makeRandomString( 22 ) )
        , currentFrame( firstFrame() )
        , backwardsMotion( false )
        , behavior( nilPointer )
        , carrier( "" )
        , transparency( 0 )
{}

AbstractItem::AbstractItem( const AbstractItem & itemToCopy )
        : Mediated( itemToCopy )
        , uniqueName( itemToCopy.uniqueName + " copy" )
        , currentFrame( itemToCopy.currentFrame )
        , backwardsMotion( itemToCopy.backwardsMotion )
        , behavior( nilPointer )
        , carrier( itemToCopy.carrier )
        , transparency( itemToCopy.transparency )
{
        IF_DEBUG( std::cout << "copying item \"" << itemToCopy.uniqueName << "\"" << std::endl )

        for ( std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator mi = itemToCopy.frames.begin() ; mi != itemToCopy.frames.end() ; ++ mi )
                for ( std::vector< NamedPicturePtr >::const_iterator pi = mi->second.begin() ; pi != mi->second.end() ; ++ pi )
                        this->frames[ mi->first ].push_back( NamedPicturePtr( new NamedPicture( ( * pi )->getAllegroPict () ) ) );

        for ( std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator mi = itemToCopy.shadows.begin(); mi != itemToCopy.shadows.end() ; ++ mi )
                for ( std::vector< NamedPicturePtr >::const_iterator pi = mi->second.begin() ; pi != mi->second.end() ; ++ pi )
                        this->shadows[ mi->first ].push_back( NamedPicturePtr( new NamedPicture( ( * pi )->getAllegroPict () ) ) );
}

AbstractItem::~AbstractItem( )
{
        clearFrames() ;
}

bool AbstractItem::updateItem ()
{
        return ( this->behavior != nilPointer ) ? this->behavior->update() : true ;
}

NamedPicture & AbstractItem::getNthFrameIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */
{
        std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator it = this->frames.begin() ;
        for ( ; it != this->frames.end() ; ++ it )
                if ( it->first == sequence && n < it->second.size() )
                        return *( it->second[ n ] );

        std::string message = "there’s no " + util::toStringWithOrdinalSuffix( n ) + " frame in \"" + sequence + "\" for \"" + getUniqueName() + "\"" ;
        std::cerr << message << std::endl ;
        throw NoSuchPictureException( message );
}

NamedPicture & AbstractItem::getNthShadowIn ( const std::string & sequence, unsigned int n ) const /* throws NoSuchPictureException */
{
        std::map< std::string, std::vector< NamedPicturePtr > >::const_iterator it = this->shadows.begin() ;
        for ( ; it != this->shadows.end() ; ++ it )
                if ( it->first == sequence && n < it->second.size() )
                        return *( it->second[ n ] );

        std::string message = "there’s no " + util::toStringWithOrdinalSuffix( n ) + " shadow in \"" + sequence + "\" for \"" + getUniqueName() + "\"" ;
        std::cerr << message << std::endl ;
        throw NoSuchPictureException( message );
}

void AbstractItem::changeFrame( unsigned int newFrame )
{
        if ( newFrame == this->currentFrame ) return ;

        if ( newFrame < howManyFramesInTheCurrentSequence() )
        {
                this->currentFrame = newFrame ;

                updateImage() ;

                if ( hasShadow() ) updateShadow ();
        }
}

bool AbstractItem::doGraphicsOverlapAt( const AbstractItem & item, int x, int y ) const
{
        int thisImageWidth = this->getCurrentRawImage().getWidth() ;
        int thisImageHeight = this->getCurrentRawImage().getHeight() ;
        int thatImageWidth = item.getCurrentRawImage().getWidth() ;
        int thatImageHeight = item.getCurrentRawImage().getHeight() ;

        return  ( this->getImageOffsetX() < x + thatImageWidth ) && ( x < this->getImageOffsetX() + thisImageWidth )
                        && ( this->getImageOffsetY() < y + thatImageHeight ) && ( y < this->getImageOffsetY() + thisImageHeight ) ;
}

void AbstractItem::setBehaviorOf( const std::string & nameOfBehavior )
{
        this->behavior = behaviors::CreatorOfBehaviors::createBehaviorByName( * this , nameOfBehavior );
}

