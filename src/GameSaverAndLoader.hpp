// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameSaverAndLoader_hpp_
#define GameSaverAndLoader_hpp_

#include <string>
#include "Ism.hpp"
#include "Way.hpp"


namespace iso
{

/**
 * Deals with saving and restoring of game
 */

class GameSaverAndLoader
{

public:

        GameSaverAndLoader( ) ;

        ~GameSaverAndLoader( ) ;

       /**
        * Set data needed to record game
        * @param room Room where reincarnation fish is caught
        * @param label Character who caught the fish
        */
        void ateFish ( const std::string& room, const std::string& label, int x, int y, int z, const Way& way ) ;

        bool loadGame ( const std::string& fileName ) ;

        bool saveGame ( const std::string& fileName ) ;

private:

       /**
        * Room where reincarnation fish is caught
        */
        std::string fishRoom ;

       /**
        * Name of character who caught the fish
        */
        std::string nameOfCharacterWhoCaughtTheFish ;

        int xFish ;

        int yFish ;

        int zFish ;

        Way catchFishWay ;

};

}

#endif
