// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Screen_hpp_
#define Screen_hpp_

#include "pointers.hpp"

#include "WrappersAllegro.hpp"

#include "Widget.hpp"
#include "Picture.hpp"
#include "Action.hpp"

#include <vector>

#include <typeinfo>

class Color ;


namespace gui
{

class AnimatedPictureWidget ;

/**
 * A container for user interface elements with associated action
 */

class Screen : public Widget
{

public:

        /**
         * @param action The associated action
         */
        Screen( Action & action ) ;

        virtual ~Screen( ) ;

        void draw () ;

        void refresh () const ;

        void drawOnGlobalScreen () ;

        void handleKey ( const std::string& key ) ;

        void addWidget ( Widget* widget ) ;

        /**
         * @return true if it is found and removed
         */
        bool removeWidget ( Widget* widget ) ;

        size_t countWidgets () {  return widgets.size () ;  }

        void freeWidgets () ;

        void addPictureOfHeadAt ( int x, int y ) ;

        void addPictureOfHeelsAt ( int x, int y ) ;

        void placeHeadAndHeels ( bool imagesToo, bool copyrightsToo ) ;

        static void refreshBackground () ;

        static void toBlackBackground () ;

        void refreshPicturesOfHeadAndHeels () ;

        const Picture & getImageOfScreen () const {  return * this->imageOfScreen ;  }

        void drawSpectrumColorBoxes( bool draw ) {  this->drawSpectrumColors = draw ;  }

        /* ////// Action & getActionOfScreen () const {  return this->actionOfScreen ;  } */

        std::string getNameOfAction () const {  return typeid( this->actionOfScreen ).name () ;  }

        void setEscapeAction ( Action * action )
        {
                delete this->escapeAction ;
                this->escapeAction = action ;
        }

        Action * getEscapeAction () const {  return this->escapeAction ;  }

        /**
         * the next widget in the chain of handling typed keys
         */
        void setNextKeyHandler ( Widget* widget ) {  this->nextKeyHandler = widget ;  }

        Widget * getNextKeyHandler () const {  return this->nextKeyHandler ;  }

        static Picture * loadPicture ( const std::string& nameOfPicture ) ;

        static std::vector < allegro::Pict * > loadAnimation ( const char * nameOfGif ) ;

        static void scrollHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void wipeHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void barWipeHorizontally ( const Screen & oldScreen, const Screen & newScreen, bool rightToLeft ) ;

        static void randomPixelFadeIn( const Color & fadeFrom, const Screen & screen ) {  randomPixelFade( true, screen, fadeFrom ) ;  }

        static void randomPixelFadeOut( const Screen & screen, const Color & fadeTo ) {  randomPixelFade( false, screen, fadeTo ) ;  }

        static void draw2x8colors ( const Screen & slide ) ;

private:

        /**
         * Image of this screen
         */
        PicturePtr imageOfScreen ;

        /**
         * What to draw on this screen
         */
        std::vector < Widget * > widgets ;

        bool drawSpectrumColors ;

        Action & actionOfScreen ;

        Action * escapeAction ;

        Widget * nextKeyHandler ;

        AnimatedPictureWidget * pictureOfHead ;

        AnimatedPictureWidget * pictureOfHeels ;

        static void randomPixelFade( bool fadeIn, const Screen & slide, const Color & color ) ;

protected:

        static Picture * backgroundPicture ;

};

typedef multiptr < Screen > ScreenPtr ;

}

#endif
