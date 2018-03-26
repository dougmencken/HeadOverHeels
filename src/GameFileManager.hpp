// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameFileManager_hpp_
#define GameFileManager_hpp_

#include <string>
#include "Ism.hpp"
#include "Way.hpp"
#include "csxml/SaveGameXML.hpp"


namespace isomot
{

class GameManager ;
class Isomot ;
class MapManager ;

/**
 * Deals with saving and restoring of game
 */

class GameFileManager
{

public:

        GameFileManager( const GameManager * gameManager, const Isomot * isomot ) ;

        ~GameFileManager( ) ;

       /**
        * Guarda los datos necesarios para grabar la partida
        * @param room Room where reincarnation fish is caught
        * @param label Character who caught the fish
        */
        void assignFishData ( const std::string& room, const std::string& label, int x, int y, int z, const Way& way ) ;

       /**
        * Carga una partida de un archivo XML
        */
        void loadGame ( const std::string& fileName ) ;

       /**
        * Guarda una partida en un archivo XML
        */
        void saveGame ( const std::string& fileName ) ;

private:

       /**
        * Updates attributes like number of lives, horn with donuts, bag
        * @param players Data for players from loaded game file
        */
        void updateAttributesOfPlayers ( const sgxml::players::player_sequence& players ) ;

private:

       /**
        * Room where reincarnation fish is caught
        */
        std::string room ;

       /**
        * Name of character who caught the fish
        */
        std::string nameOfCharacterWhoCaughtTheFish ;

        int xFish ;

        int yFish ;

        int zFish ;

        Way catchFishWay ;

        GameManager * gameManager ;

        Isomot * isomot ;

};

}

#endif
