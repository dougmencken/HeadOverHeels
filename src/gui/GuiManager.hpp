// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GuiManager_hpp_
#define GuiManager_hpp_

#include <string>
#include <list>
#include <map>

#include <WrappersAllegro.hpp>
#include <loadpng.h>

#include "Gui.hpp"
#include "Font.hpp"


namespace gui
{

class Screen ;
class Action ;
class LanguageManager ;


class GuiManager
{

private:

        GuiManager( ) ;

        void readPreferences () ;

public:

        ~GuiManager( ) ;

        static GuiManager* getInstance () ;

       /**
        * Begin with the menu for this game
        */
        void begin () ;

        void changeScreen ( Screen * newScreen, bool dive ) ;

       /*
        * Search in list of screens for the one associated with this action
        * When there’s no such screen found, create a new one
        */
        Screen * findOrCreateScreenForAction ( Action * action ) ;

        void freeScreens () ;

        void refreshScreens () ;

        void redraw () ;

        std::string getPathToPicturesOfGui () ;

        void suspend () {  this->active = false ;  }

        bool isAtFullScreen () ;

       /**
        * Use it to toggle video at full screen & video in window
        */
        void toggleFullScreenVideo () ;

        void addFont ( Font * font ) {  fonts.push_back( font ) ;  }

        Font * findFontByFamilyAndColor ( const std::string& family, const std::string& color ) ;

protected:

        void allegroSetup () ;

private:

       /**
        * Unique object of this class for the whole game
        */
        static GuiManager * instance ;

       /**
        * Screen to display by interface
        */
        Screen * activeScreen ;

        std::map < std::string, Screen * > listOfScreens;

        allegro::Pict * picture ;

       /**
        * Language for user interface
        */
        std::string language ;

        LanguageManager * languageManager ;

       /**
        * When active draw interface and handle keyboard
        */
        bool active ;

       /**
        * Draw graphics at full screen when true or in window when false
        */
        bool atFullScreen ;

        bool preferencesRead ;

        std::list < Font * > fonts ;

        std::multimap< unsigned int, unsigned int > sizesOfScreen ;

private:

        void assignLanguage ( const std::string& language ) ;

public:

       /**
        * @return A string of characters in the LLL_CC format
        *         where LLL is a language code according to ISO 639
        *         and CC is a country code according to ISO 3166
        */
        std::string getLanguage () const {  return this->language ;  }

        void setLanguage ( const std::string& language )
        {
                this->language = language ;
                assignLanguage( language );
        }

        LanguageManager* getLanguageManager () const {  return this->languageManager ;  }

        Screen * getActiveScreen () const {  return this->activeScreen ;  }

};

}

#endif
