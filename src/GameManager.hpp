// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameManager_hpp_
#define GameManager_hpp_

#include <string>

#include "Ism.hpp"
#include "Picture.hpp"
#include "Isomot.hpp"
#include "Room.hpp"
#include "GameSaverAndLoader.hpp"
#include "ColorCyclingLabel.hpp"
#include "Timer.hpp"


namespace iso
{

class Isomot ;


/**
 * Why the game was paused
 */

class WhyPaused
{

public:

        WhyPaused( unsigned int why ) : whyPaused( why ) { }
        operator unsigned int() const {  return whyPaused ;  }

        static const unsigned int Nevermind = 0 ;
        static const unsigned int FreePlanet = 1 ;
        static const unsigned int SaveGame = 2 ;
        static const unsigned int Freedom = 3 ;
        static const unsigned int Success = 4 ;
        static const unsigned int GameOver = 5 ;

private:

        unsigned int whyPaused ;

};


/**
 * Manages user interface and isometric engine. Plus, holds data for the game
 * such as how many lives left for characters, which planets are now free, et cetera
 */

class GameManager
{

private:

        GameManager( ) ;

        GameManager( const GameManager & ) { }

protected:

        static PicturePtr refreshPicture ( const std::string & nameOfPicture ) ;

public:

        static const unsigned int updatesPerSecond = 50 ;

        static const unsigned int spaceForAmbiance = 100 ;

        static GameManager & getInstance () ;

        virtual ~GameManager( ) ;

        void cleanUp () ;

        /**
         * Read preferences of game from XML file
         */
        static bool readPreferences ( const std::string& fileName ) ;

        /**
         * Write preferences of game to XML file
         */
        static bool writePreferences ( const std::string& fileName ) ;

        /**
         * Game begins here
         * @return Reason why the game is paused
         */
        WhyPaused begin () ;

        /**
         * Pause game in progress
         */
        WhyPaused pause () ;

        /**
         * Resume game after a pause
         * @return Reason why the game was paused
         */
        WhyPaused resume () ;

        std::string getHeadRoom () const {  return headRoom ;  }

        void setHeadRoom( const std::string& room ) {  headRoom = room ;  }

        std::string getHeelsRoom () const {  return heelsRoom ;  }

        void setHeelsRoom( const std::string& room ) {  heelsRoom = room ;  }

        /**
         * Draw ambiance of game, that is info about game like lives, tools, donus, thing in bag
         */
        void drawAmbianceOfGame ( const allegro::Pict& where ) ;

        void drawOnScreen ( const allegro::Pict& view ) ;

        /**
         * Carga una partida grabada en disco
         */
        void loadGame ( const std::string& fileName ) ;

        /**
         * Guarda en disco la partida actual
         */
        void saveGame ( const std::string& fileName ) ;

        /**
         * Añade un número de vidas a un jugador
         */
        void addLives ( const std::string& player, unsigned char lives ) ;

        /**
         * Resta una vida a un jugador
         */
        void loseLife ( const std::string& player ) ;

        /**
         * Player takes magic item
         */
        void takeMagicItem ( const std::string& label ) ;

        /**
         * Resta una rosquilla a Head
         */
        void consumeDonut () {  this->donuts-- ;  }

        /**
         * Añade velocidad doble a un jugador
         */
        void addHighSpeed ( const std::string& player, unsigned int highSpeed ) ;

        /**
         * Resta una unidad al tiempo restante de doble velocidad
         */
        void decreaseHighSpeed ( const std::string& player ) ;

        /**
         * Añade un número de grandes saltos a un jugador
         */
        void addHighJumps ( const std::string& player, unsigned int highJumps ) ;

        /**
         * Resta un gran salto al jugador
         */
        void decreaseHighJumps ( const std::string& player ) ;

        void addShield ( const std::string& player, float shield ) ;

        void modifyShield ( const std::string& player, float shield ) ;

        void emptyHandbag () {  setImageOfItemInBag( PicturePtr () ) ;  }

        /**
         * Image of the item inside hand bag
         */
        void setImageOfItemInBag ( const PicturePtr & pic ) {  this->imageOfItemInBag = pic ;  }

        void resetPlanets () ;

        /**
         * Libera un planeta
         * @param planet Planet to liberate
         * @param now Whether is it just liberated or was already liberated in previous game
         */
        void liberatePlanet ( const std::string& planet, bool now = true ) ;

        /**
         * Indica si un planeta es libre
         * @param planet Planet in question
         * @return true if you took planet’s crown or false otherwise
         */
        bool isFreePlanet ( const std::string& planet ) const ;

        /**
         * How many planets are already liberated
         */
        unsigned short countFreePlanets () const ;

        /**
         * Eat fish, that is, begin process to save the game
         */
        void eatFish ( const PlayerItem & character, Room * room ) ;

        void eatFish ( const PlayerItem & character, Room * room, int x, int y, int z ) ;

        /**
         * End game because characters have lost all their lives
         */
        void endGame () {  this->gameOver = true ;  }

        /**
         * End game because characters have reached Freedom
         */
        void arriveInFreedom () {  this->freedom = true ;  }

        /**
         * End game because characters have been proclaimed emperors
         */
        void success () {  this->emperator = true ;  }

        const char* getChosenGraphicSet () const {  return chosenGraphicSet.c_str () ;  }

        void setChosenGraphicSet ( const char* newSet ) {  chosenGraphicSet = newSet ;  }

        bool isPresentGraphicSet () const {  return ( chosenGraphicSet == "gfx" ) ;  }

        bool isSimpleGraphicSet () const {  return ( chosenGraphicSet == "gfx.simple" ) ;  }

private:

        /**
         * Update game periodically by redrawing isometric view and updating items
         * @return reason why update was paused
         */
        WhyPaused update () ;

        void refreshAmbianceImages () ;

        void refreshSceneryBackgrounds () ;

public:

        bool areLivesInexhaustible () const {  return vidasInfinitas ;  }

        void toggleInfiniteLives () {  vidasInfinitas = ! vidasInfinitas ;  }

        bool isImmuneToCollisionsWithMortalItems () const {  return immunityToCollisions ;  }

        void toggleImmunityToCollisionsWithMortalItems () {  immunityToCollisions = ! immunityToCollisions ;  }

        bool charactersFly () const {  return noFallingDown ;  }

        void setCharactersFly ( bool fly ) {  noFallingDown = fly ;  }

        bool playMelodyOfScenery () const {  return playTuneOfScenery ;  }

        void togglePlayMelodyOfScenery () {  playTuneOfScenery = ! playTuneOfScenery ;  }

        bool getDrawShadows () const {  return drawShadows ;  }

        void toggleDrawShadows () {  drawShadows = ! drawShadows ;  }

        bool hasBackgroundPicture () const {  return drawBackgroundPicture ;  }

        void toggleBackgroundPicture () {  drawBackgroundPicture = ! drawBackgroundPicture ;  }

        bool recordingCaptures () const {  return recordCaptures ;  }

        void toggleRecordingCaptures ()
        {
                recordingTimer->reset () ;
                recordCaptures = ! recordCaptures ;

                if ( recordCaptures )
                        prefixOfCaptures = makeRandomString( 10 );
                else
                        numberOfCapture += 1000 ;
        }

private:

        static GameManager instance ;

        std::string headRoom ;

        std::string heelsRoom ;

        gui::ColorCyclingLabel * freedomLabel ;

        unsigned int numberOfCapture ;

        std::string prefixOfCaptures ;

        std::string capturePath ;

        std::string chosenGraphicSet ;

        bool vidasInfinitas ;

        bool immunityToCollisions ;

        bool noFallingDown ;

        bool playTuneOfScenery ;

        bool drawShadows ;

        bool drawBackgroundPicture ;

        bool recordCaptures ;

        autouniqueptr < Timer > recordingTimer ;

        Isomot isomot ;

        /**
         * To save and restore games
         */
        GameSaverAndLoader saverAndLoader ;

        /**
         * Vidas de Head
         */
        unsigned char headLives ;

        /**
         * Vidas de Heels
         */
        unsigned char heelsLives ;

        /**
         * Tiempo restante para consumir la doble velocidad de Head
         */
        unsigned int highSpeed ;

        /**
         * Número de grandes saltos que le restan a Heels
         */
        unsigned int highJumps ;

        /**
         * Tiempo restante para consumir la inmunidad de Head
         */
        float headShield ;

        /**
         * Tiempo restante para consumir la inmunidad de Heels
         */
        float heelsShield ;

        /**
         * Indica si Head tiene la bocina
         */
        bool horn ;

        /**
         * Indica si Heels tiene el bolso
         */
        bool handbag ;

        /**
         * Número de rosquillas que tiene Head
         */
        unsigned short donuts ;

        PicturePtr imageOfItemInBag ;

        /**
         * Stores name of planet with boolean of its liberation
         */
        std::map < std::string, bool > planets ;

        bool takenCrown ;

        bool eatenFish ;

        bool gameOver ;

        bool freedom ;

        bool emperator ;

        std::map < std::string, PicturePtr > sceneryBackgrounds ;

        PicturePtr pictureOfHead ;

        PicturePtr grayPictureOfHead ;

        PicturePtr pictureOfHeels ;

        PicturePtr grayPictureOfHeels ;

        PicturePtr pictureOfBag ;

        PicturePtr grayPictureOfBag ;

        PicturePtr pictureOfHorn ;

        PicturePtr grayPictureOfHorn ;

        PicturePtr pictureOfDonuts ;

        PicturePtr grayPictureOfDonuts ;

        PicturePtr pictureOfGrandesSaltos ;

        PicturePtr grayPictureOfGrandesSaltos ;

        PicturePtr pictureOfGranVelocidad ;

        PicturePtr grayPictureOfGranVelocidad ;

        PicturePtr pictureOfEscudo ;

        PicturePtr grayPictureOfEscudo ;

public:

        Isomot & getIsomot () {  return isomot ;  }

        /**
         * Returns the number of lives left for player
         */
        unsigned char getLives ( const std::string& player ) const ;

        void setHeadLives ( unsigned char lives ) {  this->headLives = lives ;  }

        void setHeelsLives ( unsigned char lives ) {  this->heelsLives = lives ;  }

        void setHighSpeed ( unsigned int highSpeed ) {  this->highSpeed = highSpeed ;  }

        unsigned int getHighSpeed () const {  return this->highSpeed;  }

        void setHighJumps ( unsigned int highJumps ) {  this->highJumps = highJumps ;  }

        unsigned int getHighJumps () const {  return this->highJumps ;  }

        void setHeadShield ( unsigned int shield ) {  this->headShield = shield ;  }

        void setHeelsShield ( unsigned int shield ) {  this->heelsShield = shield ;  }

        float getShield ( const std::string& player ) const ;

        void setHorn ( bool hasHorn ) {  this->horn = hasHorn ;  }

        void setHandbag ( bool hasHandbag ) {  this->handbag = hasHandbag ;  }

        void fillToolsOwnedByCharacter ( const std::string& player, std::vector < std::string > & tools ) const ;

        void setDonuts ( unsigned short number ) {  this->donuts = number ;  }

        unsigned short getDonuts ( const std::string& player ) const ;

        unsigned int getVisitedRooms () ;

};

}

#endif
