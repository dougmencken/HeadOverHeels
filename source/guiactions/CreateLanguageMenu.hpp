// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateLanguageMenu_hpp_
#define CreateLanguageMenu_hpp_

#include <map>

#include "Action.hpp"


namespace gui
{

/**
 * Creates a menu for language selection
 */

class CreateLanguageMenu : public Action
{

public:

        CreateLanguageMenu( ) ;

        ~CreateLanguageMenu( ) ;

        // std::map < std::string /* iso */, std::string /* linguonym */ >
        static void makeMapOfLanguages ( const std::string & languagesFolder, std::map < std::string, std::string > & languages ) ;

protected:

        // show the language menu
        virtual void act () ;

private:

        std::map < std::string /* iso */, std::string /* linguonym */ > languages ;

};

}

#endif
