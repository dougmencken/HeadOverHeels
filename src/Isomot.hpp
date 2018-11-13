// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Isomot_hpp_
#define Isomot_hpp_

#include "Ism.hpp"
#include "Timer.hpp"
#include "Picture.hpp"

#include <list>
#include <string>

#include <tinyxml2.h>

#include "MapManager.hpp"
#include "ItemDataManager.hpp"


class Color ;
class Picture ;

namespace iso
{

class Room ;


/**
 * El motor isométrico
 */

class Isomot
{

public:

        Isomot( ) ;

        virtual ~Isomot( ) ;

        void binView () ;

        void prepare () ;

        void fillItemDataManager () ;

        void beginNewGame () ;

        void continueSavedGame ( tinyxml2::XMLElement * characters ) ;

        void pause () ;

        void resume () ;

        Picture * updateMe () ;

        static void drawMiniatureOfRoom ( const Room & room, const Picture * where, const unsigned int sizeOfTile ) ;

        static void drawEastDoorOnMiniature( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color ) ;
        static void drawSouthDoorOnMiniature( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color ) ;
        static void drawNorthDoorOnMiniature( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color ) ;
        static void drawWestDoorOnMiniature( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color ) ;

        static void drawIsoSquare( const allegro::Pict & where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color ) ;

        static void drawIsoTile ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color & color, bool loX, bool loY, bool hiX, bool hiY ) ;

        static void fillIsoTile ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color & color ) ;

        static void fillIsoTileInside ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color & color, bool fullFill ) ;

        MapManager & getMapManager () {  return mapManager ;  }

        const ItemDataManager & getItemDataManager () const {  return itemDataManager ;  }

        bool doesCameraFollowCharacter () const {  return cameraFollowsCharacter ;  }

private:

        void handleMagicKeys () ;

        void playTuneForScenery ( const std::string& scenery ) ;

        void offRecording () ;

        void offVidasInfinitas () ;

        void offInviolability () ;

        void updateFinalRoom () ;

        /**
         * Where to draw isometric view
         */
        Picture * view ;

        MapManager mapManager ;

        ItemDataManager itemDataManager ;

        bool paused ;

        autouniqueptr < Timer > finalRoomTimer ;

        bool finalRoomBuilt ;

        unsigned int sizeOfTileForMiniature ;

        bool cameraFollowsCharacter ;

#if defined( DEBUG ) && DEBUG
        PicturePtr chequerboard ;
#endif

};

}

#endif
