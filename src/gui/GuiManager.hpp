// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GuiManager_hpp_
#define GuiManager_hpp_

#include <string>
#include <list>
#include <map>

#include "Gui.hpp"
#include "Font.hpp"
#include "Screen.hpp"


namespace gui
{

class Action ;
class LanguageManager ;


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
        static const unsigned int TheFinalScreen = 4 ;  /* both characters reached Freedom with all the crowns */
        static const unsigned int GameOver = 5 ;        /* the user quit the game or all lives are lost */

public:

        WhyPaused() : whyPaused( Nevermind ) { }

        bool isPaused() const
        {
                return ( whyPaused == LiberatePlanet
                                || whyPaused == SaveGame
                                    || whyPaused == InFreedom
                                        || whyPaused == TheFinalScreen
                                            || whyPaused == GameOver ) ;
        }

        void nevermind () {  whyPaused = Nevermind ;  }

        void forNewlyLiberatedPlanet() {  whyPaused = LiberatePlanet ;  }
        void forSaving() {  whyPaused = SaveGame ;  }
        void forArrivingInFreedom() {  whyPaused = InFreedom ;  }
        void forThatFinalScreen() {  whyPaused = TheFinalScreen ;  }
        void forGameOver() {  whyPaused = GameOver ;  }

        bool isPlanetLiberated() const {  return whyPaused == LiberatePlanet ;  }
        bool isTimeToSaveTheGame() const {  return whyPaused == SaveGame ;  }
        bool isThatFinalSuccessScreen() const {  return whyPaused == TheFinalScreen ;  }
        bool isInFreedomWithoutAllTheCrowns() const {  return whyPaused == InFreedom ;  }
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

        void begin () ;

        void changeScreen ( const Screen & newScreen, bool dive /* it becomes rightToLeft for barWipeHorizontally */ ) ;

       /*
        * Search in the list of screens for the one associated with this action
        * When there’s no such screen found, create a new one
        */
        ScreenPtr findOrCreateScreenForAction ( Action * action ) ;

        void freeScreens () ;

        void refreshScreens () ;

        void redraw () ;

        std::string getPathToThesePictures () const ;

        void suspend () {  this->active = false ;  }

        bool isAtFullScreen () ;

       /**
        * Use it to toggle video at full screen & video in window
        */
        void toggleFullScreenVideo () ;

        void addFont ( Font * font ) {  fonts.push_back( font ) ;  }

        Font * getOrCreateFontByFamilyAndColor ( const std::string& family, const std::string& color ) ;

       /**
        * @return A string of characters in the LLL_CC format
        *         where LLL is a language code according to ISO 639
        *         and CC is a country code according to ISO 3166
        */
        std::string getLanguage () const {  return this->language ;  }

        void setLanguage ( const std::string& language )
        {
                this->language = language ;
                assignLanguage( language );
        }

        LanguageManager* getLanguageManager () const {  return languageManager ;  }

        const ScreenPtr & getActiveScreen () const {  return activeScreen ;  }

        void setActiveScreen ( const ScreenPtr newScreen ) {  activeScreen = newScreen ;  }

        const WhyPaused & getWhyTheGameIsPaused () const {  return whyTheGameIsPaused ;  }
        WhyPaused & getWhyTheGameIsPausedToAlter () {  return whyTheGameIsPaused ;  }

        void resetWhyTheGameIsPaused () {  whyTheGameIsPaused.nevermind () ;  }

private:

       /**
        * Unique object of this class for the whole game
        */
        static GuiManager * instance ;

        // the screen to draw by the user interface
        ScreenPtr activeScreen ;

        std::map < std::string, ScreenPtr > listOfScreens;

        // a language of the user interface in the LLL_CC format
        std::string language ;

        LanguageManager * languageManager ;

        // when true draw the user interface and handle the keyboard
        bool active ;

        // a reason why the game is paused
        WhyPaused whyTheGameIsPaused ;

       /**
        * Draw graphics at full screen when true or in window when false
        */
        bool atFullScreen ;

        std::list < Font * > fonts ;

        void assignLanguage ( const std::string& language ) ;

};

}

#endif
