
#include "Automap.hpp"

#include "Color.hpp"
#include "Picture.hpp"
#include "Miniature.hpp"

#include "GameMap.hpp"
#include "GamePreferences.hpp"
#include "Room.hpp"


Automap::Automap( )
        : automapImage( new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) )
        , sizeOfMiniatureTile( 3 )
{
        updateImage() ;
}

Automap::~Automap( )
{
        if ( this->automapImage != nilPointer ) {
                delete this->automapImage ;
                this->automapImage = nilPointer ;
        }
}

void Automap::updateImage ()
{
        this->automapImage->fillWithColor( Color::blackColor() );

        static const std::string letters = "AUTOMAP" ;
        allegro::textOut( letters, ( this->automapImage->getWidth() - ( letters.length() * 8 ) ) >> 1, 10, Color::whiteColor().toAllegroColor() );

        drawMiniature () ;
}

void Automap::handleKeys ()
{
        if (allegro::isKeyPushed( "Pad -" ) || allegro::isKeyPushed( "," ) )
        {
                if ( this->sizeOfMiniatureTile > 2 ) this->sizeOfMiniatureTile -- ;

                if ( allegro::isKeyPushed( "," ) ) allegro::releaseKey( "," );
                if ( allegro::isKeyPushed( "Pad -" ) ) allegro::releaseKey( "Pad -" );
        }
        if ( allegro::isKeyPushed( "Pad +" ) || allegro::isKeyPushed( "." ) )
        {
                if ( this->sizeOfMiniatureTile < 10 ) this->sizeOfMiniatureTile ++ ;

                if ( allegro::isKeyPushed( "." ) ) allegro::releaseKey( "." );
                if ( allegro::isKeyPushed( "Pad +" ) ) allegro::releaseKey( "Pad +" );
        }
}

void Automap::drawMiniature ()
{
        Miniature * ofThisRoom = this->miniatures.getMiniatureByName( "this" );
        if ( ofThisRoom == nilPointer ) {
                Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
                if ( activeRoom != nilPointer ) {
                        ofThisRoom = new Miniature( * activeRoom, this->sizeOfMiniatureTile );
                        this->miniatures.setMiniatureForName( "this", ofThisRoom );
                }
        }

        unsigned int miniatureWidth = ( ofThisRoom != nilPointer ) ? ofThisRoom->getImageWidth() : 0 ;
        unsigned int miniatureHeight = ( ofThisRoom != nilPointer ) ? ofThisRoom->getImageHeight() : 0 ;

        drawMiniature( ( GamePreferences::getScreenWidth() - miniatureWidth ) >> 1,
                       ( GamePreferences::getScreenHeight() - miniatureHeight ) >> 1,
                       this->sizeOfMiniatureTile );
}

void Automap::drawMiniature ( int screenX, int screenY, unsigned int sizeOfTile )
{
        GameMap & map = GameMap::getInstance () ;
        Room * activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return ;

        // show information about the current room and draw the miniature

        std::ostringstream roomTiles ;
        roomTiles << activeRoom->getTilesOnX() << "x" << activeRoom->getTilesOnY() ;

        const AllegroColor & roomColor = Color::byName( activeRoom->getColor() ).toAllegroColor() ;
        allegro::textOut( activeRoom->getNameOfRoomDescriptionFile(), screenX - 12, screenY - 16, roomColor );
        allegro::textOut( roomTiles.str(), screenX - 12, screenY - 4, roomColor );

        bool sameRoom = true ;
        Miniature * ofThisRoom = this->miniatures.getMiniatureByName( "this" );
        if ( ofThisRoom == nilPointer ||
                        ofThisRoom->getRoom().getNameOfRoomDescriptionFile() != activeRoom->getNameOfRoomDescriptionFile()
                        || ofThisRoom->getSizeOfTile() != sizeOfTile )
        {
                ofThisRoom = new Miniature( *activeRoom, sizeOfTile );
                this->miniatures.setMiniatureForName( "this", ofThisRoom );
                sameRoom = false ;
        }
        ofThisRoom->setOffsetOnScreen( screenX, screenY + ( sizeOfTile << 1 ) );

        const std::vector< std::string > & ways = activeRoom->getConnections()->getConnectedWays () ;
        for ( unsigned int n = 0 ; n < ways.size() ; ++ n ) {
                const std::string & way = ways[ n ] ;
                const std::string & roomThere = activeRoom->getConnections()->getConnectedRoomAt( way );
                if ( ! roomThere.empty () )
                {
                        if ( ! sameRoom ) {
                                std::cout << "hey there’s a miniature connected in " << way << std::endl ;

                                Miniature * ofThatRoom = new Miniature( * map.getOrBuildRoomByFile( roomThere ) );
                                if ( ofThisRoom->connectMiniature( ofThatRoom, way ) )
                                        this->miniatures.setMiniatureForName( way, ofThatRoom );
                                /* else
                                        std::cout << "can’t connect the miniature of room " << ofThisRoom->getRoom().getNameOfRoomDescriptionFile()
                                                        << " and the miniature of adjacent to the \"" << way
                                                        << "\" room " << ofThatRoom->getRoom().getNameOfRoomDescriptionFile()
                                                        << std::endl ; */
                        }

                        Miniature * ofRoomThere = this->miniatures.getMiniatureByName( way ) ;
                        if ( ofRoomThere != nilPointer ) ofRoomThere->draw ();
                }
        }

        if ( ofThisRoom != nilPointer ) ofThisRoom->draw() ;
}
