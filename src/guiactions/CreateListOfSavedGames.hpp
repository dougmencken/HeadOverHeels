// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateListOfSavedGames_hpp_
#define CreateListOfSavedGames_hpp_

#include "Action.hpp"


namespace gui
{

/**
 * Crea el menú para seleccionar la partida a cargar
 */

class CreateListOfSavedGames : public gui::Action
{

public:

        /**
         * @param isLoadMenu true for "load game", false for "save game"
         */
        explicit CreateListOfSavedGames( bool isLoadMenu ) ;

        virtual std::string getNameOfAction () const {  return "CreateListOfSavedGames" ;  }

        bool isLoadMenu () {  return this->isMenuForLoad ;  }

protected:

        /**
         * Show the saved games
         */
        virtual void doAction () ;

private:

        /**
         * Read some information from a file of saved game to display it
         */
        bool readSomeInfoFromGamefile( const std::string& fileName, unsigned short * visitedRooms, unsigned short * freePlanets ) ;

        bool isMenuForLoad ;

        static const unsigned int howManySaves = 10 ;

};

}

#endif
