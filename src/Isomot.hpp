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

#include "WrappersAllegro.hpp"
#include "Picture.hpp"

#include <list>
#include <string>

#include <tinyxml2.h>

#include "Timer.hpp"


class Color ;

namespace isomot
{

class ItemDataManager ;
class MapManager ;
class Room ;


/**
 * El motor isométrico
 */

class Isomot
{

public:

        Isomot( ) ;

        virtual ~Isomot( ) ;

        void prepare () ;

        void beginNewGame () ;

        void continueSavedGame ( tinyxml2::XMLElement * characters ) ;

        /**
         * Detiene el motor isométrico
         */
        void pause () ;

        void resume () ;

        /**
         * Prepara el motor para iniciar una nueva partida
         */
        void reset () ;

        /**
         * Update isometric engine
         */
        Picture * update () ;

        MapManager * getMapManager () const {  return mapManager ;  }

        static void drawMiniatureOfRoom ( Room * room, MapManager * mapManager, const Picture * where, const unsigned int sizeOfTile ) ;

        static void fillIsoTile ( const allegro::Pict & where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color & color ) ;

private:

        void playTuneForScenery ( const std::string& scenery ) ;

        void offRecording () ;

        void offVidasInfinitas () ;

        void offInviolability () ;

        void updateFinalRoom () ;

        /**
         * Where to draw isometric view
         */
        Picture * view ;

        MapManager * mapManager ;

        Timer * finalRoomTimer ;

        bool finalRoomBuilt ;

};

}

#endif
