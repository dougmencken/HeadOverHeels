
#include "FloorTile.hpp"

#include "Color.hpp"
#include "Mediator.hpp"

#include "WrappersAllegro.hpp"


FloorTile::FloorTile( int cellX, int cellY, const Picture & graphicsOfTile )
        : Mediated (), Shady ()
        , uniqueName( "floor tile at cx=" + util::number2string( cellX ) + " cy=" + util::number2string( cellY ) )
        , coordinates( std::pair< int, int >( cellX, cellY ) )
        , offset( std::pair< int, int >( 0, 0 ) )
        , rawImage( new Picture( graphicsOfTile ) )
        , shadyImage( new Picture( graphicsOfTile ) )
{
        rawImage->setName( graphicsOfTile.getName() );
        shadyImage->setName( graphicsOfTile.getName() );

        setWantShadow( true );
}

void FloorTile::calculateOffset ()
{
        assert( getMediator() != nilPointer );

        Room* room = getMediator()->getRoom();
        this->offset.first = room->getX0() + ( ( room->getSizeOfOneTile() * ( getCellX() - getCellY() - 1 ) ) << 1 ) + 1;
        this->offset.second = room->getY0() + room->getSizeOfOneTile() * ( getCellX() + getCellY() );
}

void FloorTile::draw ()
{
        assert( this->shadyImage != nilPointer );

        // draw tile with shadow
        allegro::drawSprite( this->shadyImage->getAllegroPict(), offset.first, offset.second );

        /* else if ( this->rawImage != nilPointer )
                // draw tile, just tile
                allegro::drawSprite( this->rawImage->getAllegroPict(), offset.first, offset.second ); */
}

void FloorTile::setAsShadyImage ( const Picture & toCopy )
{
        shadyImage->fillWithColor( Color::keyColor () );
        allegro::bitBlit( /* from */ toCopy.getAllegroPict(), /* to */ this->shadyImage->getAllegroPict() );
        this->shadyImage->setName( "shaded " + this->rawImage->getName() );
}

void FloorTile::freshShadyImage ()
{
        if ( getWantShadow() || this->shadyImage->getName().find( "fresh copy" ) != std::string::npos )
                return ; // it’s fresh already or in the process of shading this tile

        shadyImage->fillWithColor( Color::keyColor () );
        allegro::bitBlit( /* from */ this->rawImage->getAllegroPict(), /* to */ this->shadyImage->getAllegroPict() );
        this->shadyImage->setName( "fresh copy of " + this->rawImage->getName() );
}

unsigned int FloorTile::getIndexOfColumn () const
{
        assert( getMediator() != nilPointer );
        return getMediator()->getRoom()->getTilesOnX() * getCellY() + getCellX() ;
}

/* static */
PicturePtr FloorTile::fullTileToPartialTile( const Picture & fullTile, const std::string & whichPart, bool darkenPlane )
{
        unsigned int tileWidth = fullTile.getWidth ();
        unsigned int tileHeight = fullTile.getHeight ();

        unsigned int heightOfDepth = ( tileHeight > ( tileWidth >> 1 ) ) ? tileHeight - ( tileWidth >> 1 ) : 0 ;

        AllegroColor transparency = AllegroColor::keyColor() ;

        // decompose image of full tile into image of plane and image of depth

        autouniqueptr< allegro::Pict > planeOfTile( allegro::Pict::newPict( tileWidth, tileHeight - heightOfDepth ) );
        planeOfTile->clearToColor( transparency );

        autouniqueptr< allegro::Pict > depthOfTile( allegro::Pict::asCloneOf( fullTile.getAllegroPict().ptr() ) );

        unsigned int sizeOfPlaneX = tileWidth >> 1 ;
        unsigned int sizeOfPlaneY = tileWidth >> 2 ;

        unsigned int originX = sizeOfPlaneX ;
        unsigned int originY = 0 ;

        unsigned int dxBegin = 0 ;
        unsigned int dxEnd = sizeOfPlaneX ;
        unsigned int dyBegin = 0 ;
        unsigned int dyEnd = sizeOfPlaneY ;

        unsigned int maxHeight = planeOfTile->getH() ;

        fullTile.getAllegroPict().lockReadOnly() ;

        /////depthOfTile->lockWriteOnly() ;

        for ( unsigned int dy = dyBegin ; dy < dyEnd ; ++ dy )
        {
                for ( unsigned int dx = dxBegin ; dx < dxEnd ; dx += 2 )
                {
                        unsigned int x = originX + dx - ( dy << 1 ) ;
                        unsigned int y = originY + dy + ( dx >> 1 ) ;

                        depthOfTile->putPixelAt( x , y , transparency );
                        depthOfTile->putPixelAt( x + 1 , y , transparency );

                        if ( x >= 2 )
                        {
                                depthOfTile->putPixelAt( x - 1 , y , transparency );
                                depthOfTile->putPixelAt( x - 2 , y , transparency );

                                if ( dx == dxBegin && dy == dyEnd - 1 )
                                {
                                        depthOfTile->putPixelAt( x - 1 , y + 1 , transparency );
                                        depthOfTile->putPixelAt( x - 2 , y + 1 , transparency );
                                }
                        }

                        if ( dx + 2 == dxEnd )
                        {
                                depthOfTile->putPixelAt( x , y + 1 , transparency );
                                depthOfTile->putPixelAt( x + 1 , y + 1 , transparency );
                        }

                        if ( dy + 1 == dyEnd && y + 2 < maxHeight )
                        {
                                depthOfTile->putPixelAt( x , y + 2 , transparency );
                                depthOfTile->putPixelAt( x + 1 , y + 2 , transparency );
                        }
                }
        }

        /////depthOfTile->unlock() ;

        if ( whichPart == "north" || whichPart == "ne" || whichPart == "nw" )
        {
                dxBegin = sizeOfPlaneX >> 1 ;
        }
        if ( whichPart == "east" || whichPart == "ne" || whichPart == "se" )
        {
                dyBegin = sizeOfPlaneY >> 1 ;
        }
        if ( whichPart == "south" || whichPart == "se" || whichPart == "sw" )
        {
                dxEnd = sizeOfPlaneX >> 1 ;
                maxHeight -= ( sizeOfPlaneY >> 1 ) ;
        }
        if ( whichPart == "west" || whichPart == "nw" || whichPart == "sw" )
        {
                dyEnd = sizeOfPlaneY >> 1 ;
                maxHeight -= ( sizeOfPlaneY >> 1 ) ;
        }

        /////planeOfTile->lockWriteOnly() ;

        for ( unsigned int dy = dyBegin ; dy < dyEnd ; ++ dy )
        {
                for ( unsigned int dx = dxBegin ; dx < dxEnd ; dx += 2 )
                {
                        unsigned int x = originX + dx - ( dy << 1 ) ;
                        unsigned int y = originY + dy + ( dx >> 1 ) ;

                        planeOfTile->putPixelAt( x , y , fullTile.getPixelAt( x , y ) );
                        planeOfTile->putPixelAt( x + 1 , y , fullTile.getPixelAt( x + 1 , y ) );

                        if ( x >= 2 )
                        {
                                planeOfTile->putPixelAt( x - 1 , y , fullTile.getPixelAt( x - 1 , y ) );
                                planeOfTile->putPixelAt( x - 2 , y , fullTile.getPixelAt( x - 2 , y ) );

                                if ( dx == dxBegin && dy == dyEnd - 1 )
                                {
                                        planeOfTile->putPixelAt( x - 1 , y + 1 , fullTile.getPixelAt( x - 1 , y + 1 ) );
                                        planeOfTile->putPixelAt( x - 2 , y + 1 , fullTile.getPixelAt( x - 2 , y + 1 ) );
                                }
                        }

                        if ( dx + 2 == dxEnd )
                        {
                                planeOfTile->putPixelAt( x , y + 1 , fullTile.getPixelAt( x , y + 1 ) );
                                planeOfTile->putPixelAt( x + 1 , y + 1 , fullTile.getPixelAt( x + 1 , y + 1 ) );
                        }

                        if ( dy + 1 == dyEnd && y + 2 < maxHeight )
                        {
                                planeOfTile->putPixelAt( x , y + 2 , fullTile.getPixelAt( x , y + 2 ) );
                                planeOfTile->putPixelAt( x + 1 , y + 2 , fullTile.getPixelAt( x + 1 , y + 2 ) );
                        }
                }
        }

        /////planeOfTile->unlock() ;

        fullTile.getAllegroPict().unlock() ;

        PicturePtr plane( new Picture( *planeOfTile ) );
        plane->expandOrCropTo( planeOfTile->getW() , maxHeight );
        plane->setName( fullTile.getName() + "-" + whichPart + "~plane" );

        if ( darkenPlane )
                Color::multiplyWithColor( *plane, Color::byName( "gray" ) );

        unsigned int halfOfWidth = depthOfTile->getW() >> 1 ;
        unsigned int quarterOfWidth = halfOfWidth >> 1 ;

        unsigned int deltaHeight = ( planeOfTile->getH() > maxHeight ) ? planeOfTile->getH() - maxHeight : 0 ;

        PicturePtr leftDepth( new Picture( halfOfWidth , depthOfTile->getH() ) );
        allegro::bitBlit( *depthOfTile, leftDepth->getAllegroPict(), 0, deltaHeight, 0, 0, halfOfWidth, leftDepth->getHeight() );

        PicturePtr rightDepth( new Picture( halfOfWidth , depthOfTile->getH() ) );
        allegro::bitBlit( *depthOfTile, rightDepth->getAllegroPict(), halfOfWidth, deltaHeight, 0, 0, halfOfWidth, rightDepth->getHeight() );

        if ( whichPart == "east" || whichPart == "ne" || whichPart == "se" )
        {
                PicturePtr rightDepthEast( new Picture( quarterOfWidth , depthOfTile->getH() ) );
                allegro::bitBlit( rightDepth->getAllegroPict(), rightDepthEast->getAllegroPict(),
                                  0, 0, 0, 0, quarterOfWidth, rightDepthEast->getHeight() );
                allegro::bitBlit( rightDepth->getAllegroPict(), rightDepthEast->getAllegroPict(),
                                  rightDepth->getWidth() - 2, 0, rightDepthEast->getWidth() - 2, quarterOfWidth >> 1, 2, rightDepthEast->getHeight() );

                rightDepth->fillWithColor( Color::keyColor () );
                allegro::bitBlit( rightDepthEast->getAllegroPict(), rightDepth->getAllegroPict(), 0, 0, 0, 0, quarterOfWidth, rightDepth->getHeight() );
        }

        if ( whichPart == "north" || whichPart == "ne" || whichPart == "nw" )
        {
                PicturePtr leftDepthNorth( new Picture( quarterOfWidth , depthOfTile->getH() ) );
                allegro::bitBlit( leftDepth->getAllegroPict(), leftDepthNorth->getAllegroPict(),
                                  quarterOfWidth, 0, 0, 0, quarterOfWidth, leftDepthNorth->getHeight() );
                allegro::bitBlit( leftDepth->getAllegroPict(), leftDepthNorth->getAllegroPict(),
                                  0, 0, 0, quarterOfWidth >> 1, 2, leftDepthNorth->getHeight() );

                leftDepth->fillWithColor( Color::keyColor () );
                allegro::bitBlit( leftDepthNorth->getAllegroPict(), leftDepth->getAllegroPict(), 0, 0, quarterOfWidth, 0, quarterOfWidth, leftDepth->getHeight() );
        }

        if ( whichPart == "south" || whichPart == "se" || whichPart == "sw" )
        {
                PicturePtr leftDepthSouth( new Picture( quarterOfWidth , depthOfTile->getH() ) );
                allegro::bitBlit( leftDepth->getAllegroPict(), leftDepthSouth->getAllegroPict(),
                                  0, 0, 0, quarterOfWidth >> 1, quarterOfWidth, leftDepthSouth->getHeight() );
                allegro::bitBlit( leftDepth->getAllegroPict(), leftDepthSouth->getAllegroPict(),
                                  leftDepth->getWidth() - 2, 0, leftDepthSouth->getWidth() - 2, 0, 2, leftDepthSouth->getHeight() );

                leftDepth->fillWithColor( Color::keyColor () );
                allegro::bitBlit( leftDepthSouth->getAllegroPict(), leftDepth->getAllegroPict(), 0, 0, 0, 0, quarterOfWidth, leftDepth->getHeight() );
        }

        if ( whichPart == "west" || whichPart == "nw" || whichPart == "sw" )
        {
                PicturePtr rightDepthWest( new Picture( quarterOfWidth , depthOfTile->getH() ) );
                allegro::bitBlit( rightDepth->getAllegroPict(), rightDepthWest->getAllegroPict(),
                                  quarterOfWidth, 0, 0, quarterOfWidth >> 1, quarterOfWidth, rightDepthWest->getHeight() );
                allegro::bitBlit( rightDepth->getAllegroPict(), rightDepthWest->getAllegroPict(),
                                  0, 0, 0, 0, 2, rightDepthWest->getHeight() );

                rightDepth->fillWithColor( Color::keyColor () );
                allegro::bitBlit( rightDepthWest->getAllegroPict(), rightDepth->getAllegroPict(), 0, 0, quarterOfWidth, 0, quarterOfWidth, rightDepth->getHeight() );
        }

        PicturePtr result( new Picture( tileWidth, tileHeight ) );
        result->setName( fullTile.getName() + "-" + whichPart );

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw ();
        allegro::Pict::setWhereToDraw( result->getAllegroPict() );

        allegro::bitBlit( plane->getAllegroPict() );

        int offsetForLeftDepth = 0 ;
        int offsetForRightDepth = 0 ;

        if ( whichPart == "west" || whichPart == "nw" || whichPart == "sw" )
                offsetForLeftDepth = quarterOfWidth ;

        if ( whichPart == "south" || whichPart == "se" || whichPart == "sw" )
                offsetForRightDepth = - quarterOfWidth ;

        leftDepth->expandOrCropTo( leftDepth->getWidth() , maxHeight + heightOfDepth );
        rightDepth->expandOrCropTo( rightDepth->getWidth() , maxHeight + heightOfDepth );

        allegro::drawSprite( leftDepth->getAllegroPict(), offsetForLeftDepth, 0 );
        allegro::drawSprite( rightDepth->getAllegroPict(), halfOfWidth + offsetForRightDepth, 0 );

        allegro::Pict::setWhereToDraw( previousWhere );

        return result ;
}
