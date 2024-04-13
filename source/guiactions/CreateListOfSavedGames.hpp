// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateListOfSavedGames_hpp_
#define CreateListOfSavedGames_hpp_

#include "Action.hpp"


namespace gui
{

class SavedGameInfo
{

private:

        std::string fileName ;

        unsigned short visitedRooms ;

        unsigned short liberatedPlanets ;

public:

        const std::string & getFileName () const {  return fileName ;  }

        unsigned short howManyRoomsVisited () const {  return visitedRooms ;  }

        unsigned short howManyPlanetsLiberated () const {  return liberatedPlanets ;  }

        explicit SavedGameInfo( const std::string & file, unsigned short rooms, unsigned short planets )
                : fileName( file )
                , visitedRooms( rooms )
                , liberatedPlanets( planets )
        {}

        SavedGameInfo( const SavedGameInfo & toCopy )
                : fileName( toCopy.fileName )
                , visitedRooms( toCopy.visitedRooms )
                , liberatedPlanets( toCopy.liberatedPlanets )
        {}

} ;

/**
 * Creates the menu to pick a saved game slot to load or save to
 */

class CreateListOfSavedGames : public gui::Action
{

public:

        /**
         * @param isLoadMenu true for "load game", false for "save game"
         */
        explicit CreateListOfSavedGames( bool isLoadMenu )
                : Action( )
                , isMenuForLoad( isLoadMenu )
        {}

        bool isLoadMenu () {  return this->isMenuForLoad ;  }

protected:

        /**
         * Show the saved games
         */
        virtual void act () ;

private:

        /**
         * Read the number of rooms visited and the number of liberated planets from the saved game file to show
         */
        SavedGameInfo readSomeInfoFromTheSavedGame( const std::string & fileName ) ;

        bool isMenuForLoad ;

        static const unsigned int howManySaves = 10 ;

};

}

#endif
