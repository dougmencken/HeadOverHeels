// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Switch_hpp_
#define Switch_hpp_

#include <set>
#include <string>

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

class Switch : public Behavior
{

public:

        Switch( FreeItem & item, const std::string & behavior ) ;

        virtual ~Switch( ) {}

        virtual bool update () ;

private:

        void toggleIt () ;

        /**
         * See if there are items near the switch
         * @param where to collect items
         */
        void lookForItemsNearby ( std::set< std::string > & itemsNearby ) ;

private:

        bool switchedFromAbove ;

        /**
         * Items that triggered this switch and will not re~switch it again
         */
        std::set< std::string > switchers ;

        autouniqueptr < Timer > whenToggledTimer ;

};

}

#endif
