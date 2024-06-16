// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Menu_hpp_
#define Menu_hpp_

#include <string>
#include <vector>

#include <WrappersAllegro.hpp>

#include "Picture.hpp"
#include "Widget.hpp"


namespace gui
{

class Label ;

/**
 * The menu presents the user with a list of options to choose from
 *
 * options are chosen by the Up Arrow or Q / Down Arrow or A keys
 * and are selected by using the Enter or Space key
 */

class Menu : public Widget
{

public:

        Menu( );

        virtual ~Menu( );

        virtual void draw () ;

        void redraw () ;

        virtual void handleKey ( const std::string & key ) ;

        /**
         * Add option to menu
         */
        void addOption ( Label * label ) ;

        unsigned int howManyOptions () const {  return this->options.size() ;  }

        const std::vector < Label * > & getEveryOption () const {  return this->options ;  }

        Label * getActiveOption () const {  return this->activeOption ;  }

        void setNthOptionAsActive ( unsigned int n )
        {
                if ( n < howManyOptions () )
                        this->activeOption = this->options[ n ];
        }

        void setActiveOptionByText ( const std::string & textOfOption ) ;

        /**
         * Make the first option active
         */
        void resetActiveOption () {  setNthOptionAsActive( 0 ) ;  }

        void setVerticalOffset ( int offset ) ;

        virtual unsigned int getWidthOfMenu () const ;

        virtual unsigned int getHeightOfMenu () const ;

protected:

        virtual unsigned int getIndexOfActiveOption () const ;

        /**
         * Choose the previous option of this menu
         */
        virtual void previousOption () ;

        /**
         * Choose the next option of this menu
         */
        virtual void nextOption () ;

private:

        /**
         * Options that make up the menu
         */
        std::vector < Label * > options ;

        /**
         * The chosen option
         */
        Label * activeOption ;

        /**
         * The picture to show before a menu option
         */
        static Picture * pictureBeforeOption ;

        /**
         * The picture to show before a chosen menu option
         */
        static Picture * pictureBeforeChosenOption ;

        /**
         * The picture to show before a chosen but not double height menu option
         */
        static Picture * pictureBeforeChosenOptionSingle ;

        static void makePicturesBeforeOptions ( const int offsetForTintX = 1, const int offsetForTintY = 1 ) ;

protected:

        static const Picture & getPictureBeforeOption () ;
        static const Picture & getPictureBeforeChosenOption () ;
        static const Picture & getPictureBeforeChosenOptionSingle () ;

};

}

#endif
