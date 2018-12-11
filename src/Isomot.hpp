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
#include "ItemDescriptions.hpp"


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

        void fillItemDescriptions () ;

        void beginNewGame () ;

        void continueSavedGame ( tinyxml2::XMLElement * characters ) ;

        void pause () ;

        void resume () ;

        Picture * updateMe () ;

        MapManager & getMapManager () {  return mapManager ;  }

        const ItemDescriptions & getItemDescriptions () const {  return itemDescriptions ;  }

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

        ItemDescriptions itemDescriptions ;

        bool paused ;

        autouniqueptr < Timer > finalRoomTimer ;

        bool finalRoomBuilt ;

        unsigned int sizeOfTileForMiniature ;

        bool cameraFollowsCharacter ;

        PicturePtr chequerboard ;

        bool drawOnChequerboard ;

};

}

#endif
