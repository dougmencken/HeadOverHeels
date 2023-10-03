// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Switch_hpp_
#define Switch_hpp_

#include <vector>
#include "Behavior.hpp"


namespace iso
{

class Switch : public Behavior
{

public:

        Switch( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Switch( ) ;

        virtual bool update () ;

private:

       /**
        * Check if there are items near switch
        * @param Where to store items
        * @return true if any item was found or false otherwise
        */
        bool checkSideItems ( std::vector< ItemPtr > & sideItems ) ;

private:

        bool isItemAbove ;

       /**
        * Items that triggered this switch
        */
        std::vector< ItemPtr > triggerItems ;

};

}

#endif
