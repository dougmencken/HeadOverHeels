// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Slide_hpp_
#define Slide_hpp_

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

class Slide : public Widget
{

public:

        Slide( ) ;

        virtual ~Slide( ) ;

        void draw () ;

        void refresh () const ;

        void drawOnGlobalScreen () ;

        void handleKey ( const std::string & key ) ;

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

        const Picture & getImageOfSlide () const {  return * this->imageOfSlide ;  }

        void drawSpectrumColorBoxes( bool draw ) {  this->drawSpectrumColors = draw ;  }

        bool isTransitionFromThisSlideOff () const {  return this->noTransitionFrom ;  }
        void setTransitionFromThisSlideOff () {  this->noTransitionFrom = true ;  }

        bool isTransitionToThisSlideOff () const {  return this->noTransitionTo ;  }
        void setTransitionToThisSlideOff () {  this->noTransitionTo = true ;  }

        Action * getEscapeAction () const {  return this->escapeAction ;  }

        void setEscapeAction ( Action * action )
        {
                delete this->escapeAction ;
                this->escapeAction = action ;
        }

        // assigns the handler of typed keys
        void setKeyHandler ( KeyHandler* handler ) {  this->keyHandler = handler ;  }

        KeyHandler * getKeyHandler () const {  return this->keyHandler ;  }

        static void scrollHorizontally ( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft ) ;

        static void wipeHorizontally ( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft ) ;

        static void barWipeHorizontally ( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft ) ;

        static void randomPixelFadeIn( const Color & fadeFrom, const Slide & toSlide ) {  randomPixelFade( true, toSlide, fadeFrom ) ;  }

        static void randomPixelFadeOut( const Slide & fromSlide, const Color & fadeTo ) {  randomPixelFade( false, fromSlide, fadeTo ) ;  }

        static void draw2x8colors ( const Slide & where ) ;

private:

        /**
         * Image of this slide
         */
        PicturePtr imageOfSlide ;

        /**
         * What to draw on this slide
         */
        std::vector < Widget * > widgets ;

        bool noTransitionFrom ;
        bool noTransitionTo ;

        bool drawSpectrumColors ;

        Action * escapeAction ;

        KeyHandler * keyHandler ;

        AnimatedPictureWidget * pictureOfHead ;

        AnimatedPictureWidget * pictureOfHeels ;

        static void randomPixelFade( bool fadeIn, const Slide & slide, const Color & color ) ;

protected:

        static Picture * backgroundPicture ;
        static Picture * scaledBackgroundPicture ;

};

typedef multiptr < Slide > SlidePtr ;

}

#endif
