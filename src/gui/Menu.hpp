// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
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
#include <iostream>

#include <WrappersAllegro.hpp>

#include "Picture.hpp"
#include "Widget.hpp"


namespace gui
{

class Label ;

/**
 * Menu of user interface, where options are chosen by Up Arrow or Q / Down Arrow or A keys
 * and are selected by using Enter or Space key
 */

class Menu : public Widget
{

public:

        Menu( );

        virtual ~Menu( );

        virtual bool isMenu () {  return true ;  }

        virtual void draw () ;

        void redraw () ;

        PicturePtr getWhereToDraw () const {  return whereToDraw ;  }

        void setWhereToDraw ( const PicturePtr& where ) {  whereToDraw = where ;  }

        void handleKey ( const std::string& key ) ;

        /**
         * Add option to menu
         */
        void addOption ( Label * label ) ;

        Label * getActiveOption () const {  return this->activeOption ;  }

        void setActiveOption ( Label * option ) ;

        /**
         * Make the first option active
         */
        void resetActiveOption () ;

        std::list < Label * > getEveryOption () {  return this->options ;  }

        void setVerticalOffset ( int offset ) ;

        virtual unsigned int getWidthOfMenu () const ;

        virtual unsigned int getHeightOfMenu () const ;

protected:

        /**
         * Choose the previous option of this menu
         */
        virtual void previousOption () ;

        /**
         * Choose the next option of this menu
         */
        virtual void nextOption () ;

        PicturePtr whereToDraw ;

        /**
         * Options that make up the menu
         */
        std::list < Label * > options ;

        /**
         * The chosen option
         */
        Label * activeOption ;

        /**
         * Image before each option of menu
         */
        static Picture * beforeOption ;

        /**
         * Image for the chosen option which is double height
         */
        static Picture * beforeChosenOption ;

        /**
         * Image for the chosen option which is single height
         */
        static Picture * beforeChosenOptionMini ;

        static void makePicturesForOptions () ;

};

}

#endif
