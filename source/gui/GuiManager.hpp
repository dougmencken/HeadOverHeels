// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GuiManager_hpp_
#define GuiManager_hpp_

#include <string>
#include <map>

#include "Screen.hpp"
#include "LanguageStrings.hpp"


namespace gui
{

class LanguageStrings ;


class GuiManager
{

private:

        GuiManager( ) ;

        GuiManager( const GuiManager & ) { }

public:

        ~GuiManager( ) ;

        static GuiManager & getInstance () ;

        void firstMenu () ;

        /**
         * Draw the user interface and handle keys
         */
        void loop () ;

        void changeSlide ( const std::string & newAction, bool dive /* it becomes rightToLeft for barWipeHorizontally */ ) ;

        /**
         * Find among all the slides the one that is associated with this action.
         * And if there’s no such slide, create a new one
         */
        ScreenPtr findOrCreateSlideForAction ( const std::string & nameOfAction ) ;

        void freeSlides () ;

        void refreshSlides () ;

        void redraw () const ;

        void exit () {  this->looping = false ;  }

        bool isInFullScreen () const {  return this->inFullScreen ;  }

       /**
        * Switches between full screen video & windowed video
        */
        void toggleFullScreenVideo () ;

       /**
        * @return A string of characters in the LLL_CC format
        *         where LLL is a language code according to ISO 639
        *         and CC is a country code according to ISO 3166
        */
        const std::string & getLanguage () const {  return this->chosenLanguage ;  }

        void setLanguage ( const std::string & language )
        {
                if ( language != this->chosenLanguage ) {
                        if ( this->languageStrings != nilPointer ) {
                                delete this->languageStrings ;
                                this->languageStrings = nilPointer ;
                        }

                        this->chosenLanguage = language ;
                }
        }

        LanguageStrings & getOrMakeLanguageStrings ()
        {
                if ( this->languageStrings == nilPointer ) {
                        this->languageStrings = ( ! this->chosenLanguage.empty() )
                                                        ? new LanguageStrings( this->chosenLanguage + ".xml", "en_US.xml" )
                                                        : new LanguageStrings( "en_US.xml" ) ;
                }

                return * this->languageStrings ;
        }

        const ScreenPtr & getActiveSlide () const {  return this->activeSlide ;  }

        void setActiveSlide ( ScreenPtr newSlide ) {  this->activeSlide = newSlide ;  }

        unsigned int countSlides () const {  return this->slides.size() ;  }

private:

        /**
         * Unique object of this class for the whole game
         */
        static GuiManager * instance ;

        // the slide that is currently being drawn
        ScreenPtr activeSlide ;

        std::map < /* name of action */ std::string, ScreenPtr > slides ;

        // a language of the user interface in the LLL_CC format
        std::string chosenLanguage ;

        LanguageStrings * languageStrings ;

        // when true draw the user interface and handle the keyboard
        bool looping ;

        /**
         * Draw graphics in full screen when true or in a window when false
         */
        bool inFullScreen ;

};

}

#endif
