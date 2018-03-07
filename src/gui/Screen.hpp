// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Screen_hpp_
#define Screen_hpp_

#include <list>
#include <vector>
#include <allegro.h>

#include "Widget.hpp"
#include "Color.hpp"


namespace gui
{

class Action ;
class AnimatedPicture ;

/**
 * Container for elements of user interface
 */

class Screen : public Widget
{

public:

        /**
         * Constructor
         * @param action Action for which this screen is created
         */
        Screen( Action * action ) ;

        virtual ~Screen( ) ;

        /**
         * Draw parts of screen
         */
        void draw ( BITMAP* where ) ;

        void redraw () ;

        void drawOnGlobalScreen () ;

        void handleKey ( int key ) ;

        void addWidget ( Widget* widget ) ;

        bool removeWidget ( Widget* widget ) ;

        size_t countWidgets () {  return widgets.size () ;  }

        void freeWidgets () ;

        void addPictureOfHeadAt ( int x, int y ) ;

        void addPictureOfHeelsAt ( int x, int y ) ;

        void placeHeadAndHeels ( bool imagesToo, bool copyrightsToo ) ;

        static void refreshBackground () ;

        static BITMAP * loadPicture ( const char * nameOfPicture ) ;

        static std::vector < BITMAP * > loadAnimation ( const char * nameOfGif ) ;

        static void scrollHorizontally ( Screen * oldScreen, Screen * newScreen, bool rightToLeft ) ;

        static void wipeHorizontally ( Screen * oldScreen, Screen * newScreen, bool rightToLeft ) ;

        static void barScrollHorizontally ( Screen * oldScreen, Screen * newScreen, bool rightToLeft ) ;

        static void barWipeHorizontally ( Screen * oldScreen, Screen * newScreen, bool rightToLeft ) ;

        static void randomPixelFadeIn( Color * fadeFrom, Screen * screen ) {  randomPixelFade( true, screen, fadeFrom ) ;  }

        static void randomPixelFadeOut( Screen * screen, Color * fadeTo ) {  randomPixelFade( false, screen, fadeTo ) ;  }

private:

        /**
         * Image of this screen
         */
        BITMAP * imageOfScreen ;

        /**
         * Elements of interface to draw on screen
         */
        std::list < Widget * > widgets ;

        Action * actionOfScreen ;

        Action * escapeAction ;

        Widget * keyHandler ;

        AnimatedPicture * pictureOfHead ;

        AnimatedPicture * pictureOfHeels ;

        static void randomPixelFade( bool fadeIn, Screen * slide, Color * color ) ;

protected:

        /**
         * Imagen de fondo de la pantalla
         */
        static BITMAP * backgroundPicture ;

public:

        void refreshPicturesOfHeadAndHeels () ;

        Action * getActionOfScreen () const {  return actionOfScreen ;  }

        void setEscapeAction ( Action * action ) ;

        Action * getEscapeAction () const {  return escapeAction ;  }

        /**
         * Assign successor of this component in chain to handle typing of key
         */
        void setKeyHandler ( Widget* widget ) {  keyHandler = widget ;  }

        Widget * getKeyHandler () const {  return keyHandler ;  }

};

}

#endif
