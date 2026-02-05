// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PresentTheListOfSavedGames_hpp_
#define PresentTheListOfSavedGames_hpp_

#include "Action.hpp"

#include "TheListOfSavedGamesSlide.hpp"


namespace gui
{

/**
 * Creates the menu to pick a saved game slot to load or save to
 */

class PresentTheListOfSavedGames : public Action
{

public:

        /**
         * @param forLoading true for loading, false for saving
         */
        explicit PresentTheListOfSavedGames( bool forLoading )
                : Action( )
                , isForLoading( forLoading )
        {}

protected:

        /**
         * Present the menu of saved games
         */
        virtual void act () ;

private:

        const bool isForLoading ;

        static TheListOfSavedGamesSlide * saved_games_for_loading ;
        static TheListOfSavedGamesSlide * saved_games_for_saving ;

} ;

}

#endif
