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


namespace gui
{

class LanguageStrings ;


/**
 * Why the game was paused
 */

class WhyPaused
{
private:

        unsigned int whyPaused ;

        static const unsigned int Nevermind = 99 ;

        static const unsigned int LiberatePlanet = 1 ;  /* a crown is taken and one more planet was liberated this way */
        static const unsigned int SaveGame = 2 ;        /* save the current progress */
        static const unsigned int InFreedom = 3 ;       /* at least one character reached Freedom (not with all crowns) */
        static const unsigned int TheFinal = 4 ;        /* both characters reached Freedom with all the crowns */
        static const unsigned int GameOver = 5 ;        /* the user quit the game or all lives are lost */

public:

        WhyPaused() : whyPaused( Nevermind ) { }

        bool isPaused() const
        {
                return ( whyPaused == LiberatePlanet
                                || whyPaused == SaveGame
                                    || whyPaused == InFreedom
                                        || whyPaused == TheFinal
                                            || whyPaused == GameOver ) ;
        }

        void nevermind () {  whyPaused = Nevermind ;  }

        void forNewlyLiberatedPlanet() {  whyPaused = LiberatePlanet ;  }
        void forSaving() {  whyPaused = SaveGame ;  }
        void forArrivingInFreedom() {  whyPaused = InFreedom ;  }
        void forFinalSuccess() {  whyPaused = TheFinal ;  }
        void forGameOver() {  whyPaused = GameOver ;  }

        bool isPlanetLiberated() const {  return whyPaused == LiberatePlanet ;  }
        bool isTimeToSaveTheGame() const {  return whyPaused == SaveGame ;  }
        bool isInFreedomWithoutAllTheCrowns() const {  return whyPaused == InFreedom ;  }
        bool isFinalSuccess() const {  return whyPaused == TheFinal ;  }
        bool isGameOver() const {  return whyPaused == GameOver ;  }
} ;


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
                useLanguage( language );
                this->chosenLanguage = language ;
        }

        LanguageStrings* getLanguageStrings () const {  return this->languageStrings ;  }

        const ScreenPtr & getActiveSlide () const {  return this->activeSlide ;  }

        void setActiveSlide ( ScreenPtr newSlide ) {  this->activeSlide = newSlide ;  }

        unsigned int countSlides () const {  return this->slides.size() ;  }

        const WhyPaused & getWhyTheGameIsPaused () const {  return whyTheGameIsPaused ;  }
        WhyPaused & getWhyTheGameIsPausedToAlter () {  return whyTheGameIsPaused ;  }

        void resetWhyTheGameIsPaused () {  whyTheGameIsPaused.nevermind () ;  }

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

        // a reason why the game is paused
        WhyPaused whyTheGameIsPaused ;

        /**
         * Draw graphics in full screen when true or in a window when false
         */
        bool inFullScreen ;

        void useLanguage ( const std::string & language ) ;

};

}

#endif
