// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TheListOfSavedGames_hpp_
#define TheListOfSavedGames_hpp_

#include "Menu.hpp"


namespace gui
{

class Label ;


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


class TheListOfSavedGames : public Menu
{

public :

        /**
         * @param forLoading true for loading a saved game, false for saving the current progress
         */
        TheListOfSavedGames( bool forLoading ) ;

        virtual ~TheListOfSavedGames( ) {}

        bool isForLoading () {  return this->isListForLoading ;  }

        void updateTheList () ;

private :

        /**
         * Read the number of visited rooms and the number of liberated planets from a saved game file to show
         */
        static SavedGameInfo readSomeInfoFromASavedGame( const std::string & fileName ) ;

        const bool isListForLoading ;

        static const unsigned int how_many_slots = 10 ;

} ;

}

#endif
