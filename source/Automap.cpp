
#include "Automap.hpp"

#include "Color.hpp"
#include "Picture.hpp"
#include "Miniature.hpp"

#include "GameMap.hpp"
#include "GamePreferences.hpp"
#include "Room.hpp"


Automap::Automap( )
        : automapImage( new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) )
        , sizeOfMiniatureTile( Miniature::the_default_size_of_tile )
        , miniatures()
        , automapOffsetX( 0 )
        , automapOffsetY( 0 )
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
        if ( allegro::isKeyPushed( "Left" ) ) {
                this->automapOffsetX -- ;
                allegro::releaseKey( "Left" );
        }
        if ( allegro::isKeyPushed( "Right" ) ) {
                this->automapOffsetX ++ ;
                allegro::releaseKey( "Right" );
        }
        if ( allegro::isKeyPushed( "Up" ) ) {
                this->automapOffsetY -- ;
                allegro::releaseKey( "Up" );
        }
        if ( allegro::isKeyPushed( "Down" ) ) {
                this->automapOffsetY ++ ;
                allegro::releaseKey( "Down" );
        }
        if ( allegro::isKeyPushed( "Space" ) ) {
                this->automapOffsetX = 0 ;
                this->automapOffsetY = 0 ;
                allegro::releaseKey( "Space" );
        }

        if ( allegro::isKeyPushed( "Pad -" ) || allegro::isKeyPushed( "," ) )
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
        if ( allegro::isKeyPushed( "Pad =" ) || allegro::isKeyPushed( "/" ) )
        {
                this->sizeOfMiniatureTile = Miniature::the_default_size_of_tile ;

                if ( allegro::isKeyPushed( "/" ) ) allegro::releaseKey( "/" );
                if ( allegro::isKeyPushed( "Pad =" ) ) allegro::releaseKey( "Pad =" );
        }
}

void Automap::drawMiniature ()
{
        Miniature * ofThisRoom = this->miniatures.getMiniatureByName( "this" );
        if ( ofThisRoom == nilPointer
                        || ofThisRoom->getSizeOfTile() != this->sizeOfMiniatureTile )
        {
                Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
                if ( activeRoom != nilPointer ) {
                        ofThisRoom = new Miniature( * activeRoom, this->sizeOfMiniatureTile, /* with room info */ true );
                        this->miniatures.setMiniatureForName( "this", ofThisRoom );
                }
        }

        unsigned int miniatureWidth = ( ofThisRoom != nilPointer ) ? ofThisRoom->getImageWidth() : 0 ;
        unsigned int miniatureHeight = ( ofThisRoom != nilPointer ) ? ofThisRoom->getImageHeight() : 0 ;

        drawMiniature( ( GamePreferences::getScreenWidth() - miniatureWidth ) >> 1,
                       ( GamePreferences::getScreenHeight() - miniatureHeight ) >> 1,
                       this->sizeOfMiniatureTile );
}

void Automap::drawMiniature ( int leftX, int topY, unsigned int sizeOfTile )
{
        GameMap & map = GameMap::getInstance () ;
        Room * activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return ;

        bool sameRoom = true ;
        Miniature * ofThisRoom = this->miniatures.getMiniatureByName( "this" );
        if ( ofThisRoom == nilPointer ||
                        ofThisRoom->getRoom().getNameOfRoomDescriptionFile() != activeRoom->getNameOfRoomDescriptionFile()
                        || ofThisRoom->getSizeOfTile() != sizeOfTile )
        {
                ofThisRoom = new Miniature( *activeRoom, sizeOfTile, /* with room info */ true );
                this->miniatures.setMiniatureForName( "this", ofThisRoom );
                sameRoom = false ;
        }

        const std::pair< int, int > & currentOffset = ofThisRoom->getOffsetOnScreen();
        if ( currentOffset.first != this->automapOffsetX + leftX || currentOffset.second != this->automapOffsetY + topY )
                ofThisRoom->setOffsetOnScreen( this->automapOffsetX + leftX, this->automapOffsetY + topY );

        const std::vector< std::string > & ways = activeRoom->getConnections()->getConnectedWays () ;
        for ( unsigned int n = 0 ; n < ways.size() ; ++ n ) {
                const std::string & way = ways[ n ] ;
                const std::string & roomThere = activeRoom->getConnections()->getConnectedRoomAt( way );
                if ( ! roomThere.empty () )
                {
                        if ( ! sameRoom ) {
                                std::cout << "hey there’s a miniature connected"
                                                << ( way.find( "via" ) != std::string::npos ? " " : " on " ) << way
                                                << std::endl ;

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

        // draw the miniature
        if ( ofThisRoom != nilPointer ) ofThisRoom->draw() ;
}
