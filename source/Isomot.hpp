// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Isomot_hpp_
#define Isomot_hpp_

#include "Timer.hpp"
#include "Picture.hpp"

#include <string>

#include <tinyxml2.h>


class Color ;
class Picture ;

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

        void beginNewGame () ;

        void pause () ;

        void resume () ;

        Picture * updateMe () ;

        bool doesCameraFollowCharacter () const {  return cameraFollowsCharacter ;  }

        void toggleCameraFollowsCharacter () {  cameraFollowsCharacter = ! cameraFollowsCharacter ;  }

        static const unsigned int updatesPerSecond = 50 ;

private:

        void handleMagicKeys () ;

        void playTuneForScenery ( const std::string & scenery ) ;

        void offRecording () ;

        void offVidasInfinitas () ;

        void offInviolability () ;

        void updateFinalRoom () ;

        /**
         * Where to draw isometric view
         */
        Picture * view ;

        bool paused ;

        autouniqueptr < Timer > finalRoomTimer ;

        bool finalRoomBuilt ;

        unsigned int sizeOfTileForMiniature ;

        bool cameraFollowsCharacter ;

        bool softCenteringOnRoom ;

        PicturePtr chequerboard ;

        bool drawOnChequerboard ;

} ;

#endif
