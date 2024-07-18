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
 * A container for user interface elements
 */

class Screen : public Widget
{

public:

        Screen( ) ;

        virtual ~Screen( ) ;

        void draw () ;

        void refresh () const ;

        void drawOnGlobalScreen () ;

        void handleKey ( const std::string& key ) ;

        void addWidget( Widget* widget ) ;

        /**
         * @return true if it is found and removed
         */
        bool removeWidget( Widget* widget ) ;

        /* ////// size_t countWidgets () {  return this->widgets.size () ;  } */

        bool isNewAndEmpty () {  return this->widgets.size() == 0 ;  }

        void freeWidgets () ;

        void placeHeadAndHeels ( bool imagesToo, bool copyrightsToo ) ;

        static void refreshBackground () ;

        static void toBlackBackground () ;

        void addPictureOfHeadAt ( int x, int y ) ;

        void addPictureOfHeelsAt ( int x, int y ) ;

        void refreshPicturesOfHeadAndHeels () ;

        const Picture & getImageOfScreen () const {  return * this->imageOfScreen ;  }

        void drawSpectrumColorBoxes( bool draw ) {  this->drawSpectrumColors = draw ;  }

        bool isTransitionOff () const {  return this->noTransition ;  }
        void setTransitionOff ( bool transitionOff = true ) {  this->noTransition = transitionOff ;  }

        Action * getEscapeAction () const {  return this->escapeAction ;  }

        void setEscapeAction ( Action * action )
        {
                delete this->escapeAction ;
                this->escapeAction = action ;
        }

        // assigns the handler of typed keys
        void setKeyHandler ( KeyHandler* handler ) {  this->keyHandler = handler ;  }

        KeyHandler * getKeyHandler () const {  return this->keyHandler ;  }

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

        bool noTransition ;

        bool drawSpectrumColors ;

        Action * escapeAction ;

        KeyHandler * keyHandler ;

        AnimatedPictureWidget * pictureOfHead ;

        AnimatedPictureWidget * pictureOfHeels ;

        static void randomPixelFade( bool fadeIn, const Screen & slide, const Color & color ) ;

protected:

        static Picture * backgroundPicture ;

};

typedef multiptr < Screen > ScreenPtr ;

}

#endif
