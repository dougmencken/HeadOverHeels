// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameManager_hpp_
#define GameManager_hpp_

#include <string>

#include "TheKeyMoments.hpp"
#include "GameInfo.hpp"
#include "Picture.hpp"
#include "GameSaverAndLoader.hpp"
#include "Isomot.hpp"
#include "Timer.hpp"

#include <pthread.h>

class Room ;
class AvatarItem ;


/**
 * Manages the game, and nothing more. Doesn't play solitaire. Doesn't smoke. Anything at all. Really.
 */

class GameManager
{

private:

        GameManager( ) ;

        GameManager( const GameManager & ) { }

protected:

        /////// here?
        static PicturePtr refreshPicture ( const std::string & nameOfPicture ) ;

public:

        static GameManager & getInstance () {  return GameManager::instance ;  }

        virtual ~GameManager( ) ;

        void cleanUp () ;

        // if *pointerToBoolean is true it does resume(), otherwise begin()
        // it’s for use with pthread_create
        static void * beginOrResume ( void * pointerToBoolean ) ;

        static pthread_t * getThread () {  return & GameManager::theThread ;  }

        static bool isThreadRunning () {  return GameManager::threadIsRunning ;  }

        /**
         * The game begins here
         */
        void begin () ;

        /**
         * Pause the game in progress
         */
        void pause () ;

        void resume () ;

        /**
         * Eat fish, that is, begin the process to save the game
         */
        void eatFish ( const AvatarItem & character, const std::string & inRoom ) ;

        void saveTheGame () ;

        void loseLifeAndContinue( const std::string & nameOfCharacter, const std::string & inRoom );

        GameInfo & getGameInfo() {  return theInfo ;  }

        const std::string & getHeadRoom () const {  return headRoom ;  }

        void setHeadRoom( const std::string& room ) {  headRoom = room ;  }

        const std::string & getHeelsRoom () const {  return heelsRoom ;  }

        void setHeelsRoom( const std::string& room ) {  heelsRoom = room ;  }

        /**
         * Draw the ambiance, that is info about the game like lives, tools, donus, thing in bag
         */
        void drawAmbianceOfGame ( const allegro::Pict& where ) ;

        /**
         * copy a view's image buffer to the allegro's screen
         */
        void drawOnScreen ( const allegro::Pict& view ) ;

        bool loadGame ( const std::string & fileName ) ;

        bool saveGame ( const std::string & fileName ) ;

        void emptyHandbag () {  setImageOfItemInBag( PicturePtr () ) ;  }

        /**
         * Image of an item inside the hand bag
         */
        void setImageOfItemInBag ( const PicturePtr & pic ) {  this->imageOfItemInBag = pic ;  }

        void resetPlanets () ;

        /**
         * Libera un planeta
         * @param planet the planet to liberate
         * @param previously true if restoring a game where that planet was already liberated
         */
        void liberatePlanet ( const std::string & planet, bool previously = false ) ;

        /**
         * Indica si un planeta es libre
         * @param planet the planet in question
         * @return true if you took planet’s crown or false otherwise
         */
        bool isFreePlanet ( const std::string& planet ) const ;

        /**
         * How many planets are already liberated
         */
        unsigned short howManyFreePlanets () const ;

        const std::string & getChosenGraphicsSet () const {  return chosenGraphicsSet ;  }

        void setChosenGraphicsSet ( const std::string & newSet ) {  chosenGraphicsSet = newSet ;  }

        bool isPresentGraphicsSet () const {  return ( chosenGraphicsSet == "gfx" ) ;  }

        bool isSimpleGraphicsSet () const {  return ( chosenGraphicsSet == "gfx.simple" ) ;  }

        Isomot & getIsomot () {  return isomot ;  }

        // at the end of the game it's time to count the crowns
        void inFreedomWithSoManyCrowns( unsigned int crowns ) ;

        static const unsigned int updatesPerSecond = 50 ;

private:

        /**
         * Update the game periodically
         */
        void update () ;

        void keyMoment () ;

        void refreshAmbianceImages () ;

        void refreshSceneryBackgrounds () ;

        const TheKeyMoments & getKeyMoments () const {  return this->keyMoments ;  }

        void resetKeyMoments () {  this->keyMoments.resetAll() ;  }

public:

        bool areLivesInexhaustible () const {  return vidasInfinitas ;  }
        void toggleInfiniteLives () {  vidasInfinitas = ! vidasInfinitas ;  }

        bool isImmuneToCollisionsWithMortalItems () const {  return immunityToCollisions ;  }
        void toggleImmunityToCollisionsWithMortalItems () {  immunityToCollisions = ! immunityToCollisions ;  }

        bool charactersFly () const {  return noFallingDown ;  }
        void setCharactersFly ( bool fly ) {  noFallingDown = fly ;  }

        bool playMelodyOfScenery () const {  return playTuneOfScenery ;  }
        void togglePlayMelodyOfScenery () {  playTuneOfScenery = ! playTuneOfScenery ;  }

        bool getCastShadows () const {  return castShadows ;  }
        void toggleCastShadows () {  castShadows = ! castShadows ;  }

        bool drawSceneryDecor () const {  return drawSceneryBackgrounds ;  }
        void toggleSceneryDecor () {  drawSceneryBackgrounds = ! drawSceneryBackgrounds ;  }

        bool drawRoomMiniatures () const {  return drawMiniatures ;  }
        void toggleRoomMiniatures () {  drawMiniatures = ! drawMiniatures ;  }

        bool getDrawingOfWalls () const {  return drawWalls ;  }
        void toggleDrawingOfWalls () {  drawWalls = ! drawWalls ;  }

        bool recordingCaptures () const {  return recordCaptures ;  }

        void toggleRecordingCaptures ()
        {
                recordCaptures = ! recordCaptures ;

                if ( recordCaptures ) {
                        prefixOfCaptures = util::makeRandomString( 10 );
                        numberOfCapture = 0 ;
                }

                recordingTimer->go () ;
        }

private:

        static GameManager instance ;

        static pthread_t theThread ;

        static bool threadIsRunning ;

        GameInfo theInfo ;

        std::string roomWhereLifeWasLost ;

        std::string headRoom ;

        std::string heelsRoom ;

        unsigned int numberOfCapture ;

        std::string prefixOfCaptures ;

        std::string capturePath ;

        std::string chosenGraphicsSet ;

        bool vidasInfinitas ;

        bool immunityToCollisions ;

        bool noFallingDown ;

        bool playTuneOfScenery ;

        bool castShadows ;

        bool drawSceneryBackgrounds ;

        bool drawMiniatures ;

        bool drawWalls ;

        bool recordCaptures ;

        autouniqueptr < Timer > recordingTimer ;

        Isomot isomot ;

        GameSaverAndLoader saverAndLoader ; // to save and restore games

        TheKeyMoments keyMoments ;

        PicturePtr imageOfItemInBag ;

        std::map < std::string /* the planet's name */, bool /* is free or not */ > planets ;

        std::map < std::string, PicturePtr > sceneryBackgrounds ;

        std::map < std::string, PicturePtr > ambiancePictures ;

} ;

#endif
