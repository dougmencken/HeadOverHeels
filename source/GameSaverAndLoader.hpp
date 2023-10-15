// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameSaverAndLoader_hpp_
#define GameSaverAndLoader_hpp_

#include <string>

#include <tinyxml2.h>


namespace game
{

/**
 * Deals with saving and restoring of a game
 */

class GameSaverAndLoader
{

public:

        GameSaverAndLoader( ) ;

        ~GameSaverAndLoader( ) { }

       /**
        * Set the data needed to record a game
        * @param room Room where reincarnation fish is caught
        * @param label Character who caught the fish
        */
        void ateFish ( const std::string & room, const std::string & label, int x, int y, int z, const std::string & orientation ) ;

        bool loadGame ( const std::string & file ) ;

        bool saveGame ( const std::string & file ) ;

        static const std::string Current_Save_Version () {  return "2" ;  }

private:

        void continueSavedGame ( tinyxml2::XMLElement * characters ) ;

       /**
        * The room where the reincarnation fish is caught
        */
        std::string fishRoom ;

       /**
        * The name of character who caught the fish
        */
        std::string nameOfCharacterWhoCaughtTheFish ;

        int xFish ;

        int yFish ;

        int zFish ;

        std::string catchFishWay ;

};

}

#endif
