// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Menu_hpp_
#define Menu_hpp_

#include <list>
#include <string>
#include <allegro.h>
#include "Widget.hpp"


namespace gui
{

class Label ;

/**
 * Menu of user interface where options are chosen by Up Arrow / Down Arrow keys
 * and are selected by using Enter key
 */

class Menu : public Widget
{

public:

        Menu( );

        virtual ~Menu( );

        void draw ( BITMAP* where ) ;

        void redraw () ;

        void handleKey ( int key ) ;

        /**
         * Add option to menu
         */
        void addOption ( Label* label ) ;

        Label* getActiveOption () const {  return this->activeOption ;  }

        void setActiveOption ( Label* option ) ;

        /**
         * Make the first option active
         */
        void resetActiveOption () ;

        std::list < Label * > getEveryOption () {  return this->options ;  }

        void setVerticalOffset ( int offset ) ;

        virtual unsigned int getWidthOfMenu () const ;

        virtual unsigned int getHeightOfMenu () const ;

private:

        Label* handlerOfKeys ;

protected:

        /**
         * Choose the previous option of this menu
         */
        void previousOption () ;

        /**
         * Choose the next option of this menu
         */
        void nextOption () ;

        void refreshPictures () ;

protected:

        /**
         * Options that make up the menu
         */
        std::list < Label * > options ;

        /**
         * The chosen option
         */
        Label* activeOption ;

        BITMAP * whereToDraw ;

        /**
         * Image before each option of menu
         */
        BITMAP* optionImage ;

        /**
         * Image for the chosen option which is double sized
         */
        BITMAP* chosenOptionImage ;

        /**
         * Image for the chosen option which is single sized
         */
        BITMAP* chosenOptionImageMini ;

};

}

#endif
