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

#include "pointers.hpp"

#include "WrappersAllegro.hpp"

#include "Widget.hpp"
#include "Picture.hpp"

class Color ;


namespace gui
{

class Action ;
class AnimatedPictureWidget ;

/**
 * Container for elements of user interface
 */

class Screen : public Widget
{

public:

        /**
         * @param action Action for which this screen is created
         */
        Screen( Action * action ) ;

        virtual ~Screen( ) ;

        void draw () ;

        void refresh () const ;

        void drawOnGlobalScreen () ;

        void handleKey ( const std::string& key ) ;

        void addWidget ( Widget* widget ) ;

        bool removeWidget ( Widget* widget ) ;

        size_t countWidgets () {  return widgets.size () ;  }

        void freeWidgets () ;

        void addPictureOfHeadAt ( int x, int y ) ;

        void addPictureOfHeelsAt ( int x, int y ) ;

        void placeHeadAndHeels ( bool imagesToo, bool copyrightsToo ) ;

        static void refreshBackground () ;

        void refreshPicturesOfHeadAndHeels () ;

        const Picture & getImageOfScreen () const {  return *imageOfScreen ;  }

        Action * getActionOfScreen () const {  return actionOfScreen ;  }

        void setEscapeAction ( Action * action ) ;

        Action * getEscapeAction () const {  return escapeAction ;  }

        /**
         * Assign successor of this component in chain to handle typing of key
         */
        void setKeyHandler ( Widget* widget ) {  keyHandler = widget ;  }

        Widget * getKeyHandler () const {  return keyHandler ;  }

        static Picture * loadPicture ( const std::string& nameOfPicture ) ;

        static std::vector < allegro::Pict * > loadAnimation ( const char * nameOfGif ) ;

        static void scrollHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void wipeHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void barWipeHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void randomPixelFadeIn( const Color & fadeFrom, const Screen & screen ) {  randomPixelFade( true, screen, fadeFrom ) ;  }

        static void randomPixelFadeOut( const Screen & screen, const Color & fadeTo ) {  randomPixelFade( false, screen, fadeTo ) ;  }

private:

        /**
         * Image of this screen
         */
        PicturePtr imageOfScreen ;

        /**
         * Elements of interface to draw on screen
         */
        std::list < Widget * > widgets ;

        Action * actionOfScreen ;

        Action * escapeAction ;

        Widget * keyHandler ;

        AnimatedPictureWidget * pictureOfHead ;

        AnimatedPictureWidget * pictureOfHeels ;

        static void randomPixelFade( bool fadeIn, const Screen & slide, const Color & color ) ;

protected:

        /**
         * Imagen de fondo de la pantalla
         */
        static Picture * backgroundPicture ;

};

typedef safeptr < Screen > ScreenPtr ;

}

#endif
