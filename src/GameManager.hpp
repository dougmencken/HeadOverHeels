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

#include "csxml/SaveGameXML.hpp"
#include "Ism.hpp"
#include "Room.hpp"
#include "ColorCyclingLabel.hpp"
#include "Timer.hpp"


namespace isomot
{

class Isomot ;
class GameFileManager ;
class PlayerItem ;

/**
 * Why the game was paused
 */
enum WhyPause
{
        Nevermind,
        FreePlanet,
        SaveGame,
        Freedom,
        Sucess,
        GameOver
};


/**
 * Manages user interface and isometric engine. Plus, holds data for the game
 * such as how many lives left for characters, which planets are now free, et cetera
 */

class GameManager
{

private:

        GameManager( ) ;

protected:

        static BITMAP * refreshPicture ( const char * nameOfPicture ) ;

public:

        /**
         * Único objeto de esta clase para toda la aplicación
         * @return Un puntero al objeto único
         */
        static GameManager * getInstance () ;

        inline static std::string numberToString ( int number )
        {
                return static_cast< std::ostringstream * >( &( std::ostringstream() << std::dec << number ) )->str () ;
        }

        virtual ~GameManager( ) ;

        /**
         * Read preferences of game from XML file
         */
        static bool readPreferences ( const std::string& fileName ) ;

        /**
         * Write preferences of game to XML file
         */
        static bool writePreferences ( const std::string& fileName ) ;

        /**
         * Todo empieza aquí
         * @return Razón por la que se ha detenido el juego
         */
        WhyPause begin () ;

        /**
         * Reanuda la partida tras una interrupción. La interrupción viene dada porque se ha liberado
         * un planeta o porque se acaba de grabar la partida
         * @return Razón por la que se ha interrumpido el juego
         */
        WhyPause resume () ;

        /**
         * Dibuja en pantalla la información relativa al juego actual compuesta de:
         * un marco, vidas disponibles, herramientas disponibles, objeto guardado en el
         * bolso, tiempo restante de inmunidad, saltos largos disponibles y rosquillas
         * disponibles
         * @param where Imagen donde se dibujará toda la información
         */
        void drawAmbianceOfGame ( BITMAP* where ) ;

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
         * @param player Name of character
         * @param lives Número de vidas a sumar
         */
        void addLives ( const std::string& player, unsigned char lives ) ;

        /**
         * Resta una vida a un jugador
         * @param player Name of character
         */
        void loseLife ( const std::string& player ) ;

        /**
         * Player takes magic item
         * @param Label of item
         */
        void takeMagicItem ( const std::string& label ) ;

        /**
         * Resta una rosquilla a Head
         */
        void consumeDonut () {  this->donuts-- ;  }

        /**
         * Añade velocidad doble a un jugador
         * @param player Player taking high speed
         * @param highSpeed Un número entre 0 y 99
         */
        void addHighSpeed ( const std::string& player, unsigned int highSpeed ) ;

        /**
         * Resta una unidad al tiempo restante de doble velocidad
         * @param Player with high speed
         */
        void decreaseHighSpeed ( const std::string& player ) ;

        /**
         * Añade un número de grandes saltos a un jugador
         * @param player Player taking high jumps
         * @param highJumps Un número entre 0 y 10
         */
        void addHighJumps ( const std::string& player, unsigned int highJumps ) ;

        /**
         * Resta un gran salto al jugador
         * @param player Player with high jumps
         */
        void decreaseHighJumps ( const std::string& player ) ;

        /**
         * Añade inmunidad a un jugador
         * @param player Player taking immunity
         * @param shield Un número de milisegundos
         */
        void addShield ( const std::string& player, double shield ) ;

        /**
         * Actualiza el tiempo restante de inmunidad
         * @param player Player with immunity
         * @param shield Un número de milisegundos
         */
        void modifyShield ( const std::string& player, double shield ) ;

        /**
         * Elimina el elemento que contiene el bolso para no mostrarlo en la interfaz
         * @param player Player with handbag
         */
        void emptyHandbag ( const std::string& player ) ;

        /**
         * Establece todos los planetas como miembros del Imperio Blacktooth
         */
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
        void eatFish ( PlayerItem* character, Room* room ) ;

        void eatFish ( PlayerItem* character, Room* room, int x, int y, int z ) ;

        /**
         * End game because characters have lost all their lives
         */
        void end () {  this->gameOver = true ;  }

        /**
         * End game because characters have reached Freedom
         */
        void arriveFreedom () {  this->freedom = true ;  }

        /**
         * End game because characters have been proclaimed emperors
         */
        void success () {  this->emperator = true ;  }

        const char* getChosenGraphicSet () const {  return chosenGraphicSet.c_str () ;  }

        void setChosenGraphicSet ( const char* newSet ) {  chosenGraphicSet = newSet ;  }

        bool isSimpleGraphicSet () const {  return ( chosenGraphicSet == "gfx.simple" ) ;  }

private:

        /**
         * Update game periodically by redrawing isometric view and updating items
         * @return reason why update was paused
         */
        WhyPause update () ;

        /**
         * Pause game in progress
         * @return reason why update was paused
         */
        WhyPause pause () ;

        void refreshAmbianceImages () ;

        void refreshBackgroundFrames () ;

public:

        bool areLivesInexhaustible () const {  return vidasInfinitas ;  }

        void toggleInfiniteLives () {  vidasInfinitas = ! vidasInfinitas ;  }

        bool isImmuneToCollisionsWithMortalItems () const {  return immunityToCollisions ;  }

        void toggleImmunityToCollisionsWithMortalItems () {  immunityToCollisions = ! immunityToCollisions ;  }

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

        ItemDataManager * itemDataManager ;

        gui::ColorCyclingLabel * freedomLabel ;

        Timer * recordingTimer ;

        unsigned int numberOfCapture ;

        std::string prefixOfCaptures ;

        std::string capturePath ;

        std::string chosenGraphicSet ;

        bool vidasInfinitas ;

        bool immunityToCollisions ;

        bool playTuneOfScenery ;

        bool drawShadows ;

        bool drawBackgroundPicture ;

        bool recordCaptures ;

        static GameManager * instance ;

        Isomot * isomot ;

        /**
         * To manage saving and restoring of games
         */
        GameFileManager * gameFileManager ;

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
        double headShield ;

        /**
         * Tiempo restante para consumir la inmunidad de Heels
         */
        double heelsShield ;

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

        /**
         * Imagen del elemento que está dentro del bolso
         */
        BITMAP * itemTaken ;

        /**
         * Stores name of planet with boolean of its liberation
         */
        std::map < std::string, bool > planets ;

        bool takenCrown ;

        bool eatenFish ;

        bool gameOver ;

        bool freedom ;

        bool emperator ;

        BITMAP * frameForJail ;

        BITMAP * frameForBlacktooth ;

        BITMAP * frameForMarket ;

        BITMAP * frameForMoon ;

        BITMAP * frameForByblos ;

        BITMAP * frameForSafari ;

        BITMAP * frameForEgyptus ;

        BITMAP * frameForPenitentiary ;

        BITMAP * pictureOfHead ;

        BITMAP * grayPictureOfHead ;

        BITMAP * pictureOfHeels ;

        BITMAP * grayPictureOfHeels ;

        BITMAP * pictureOfBag ;

        BITMAP * grayPictureOfBag ;

        BITMAP * pictureOfHorn ;

        BITMAP * grayPictureOfHorn ;

        BITMAP * pictureOfDonuts ;

        BITMAP * grayPictureOfDonuts ;

        BITMAP * pictureOfGrandesSaltos ;

        BITMAP * grayPictureOfGrandesSaltos ;

        BITMAP * pictureOfGranVelocidad ;

        BITMAP * grayPictureOfGranVelocidad ;

        BITMAP * pictureOfEscudo ;

        BITMAP * grayPictureOfEscudo ;

public:

        /**
         * Returns the number of lives left for player
         */
        unsigned char getLives ( const std::string& player ) const ;

        void setHeadLives ( unsigned char lives ) {  this->headLives = lives ;  }

        void setHeelsLives ( unsigned char lives ) {  this->heelsLives = lives ;  }

        /**
         * Establece el tiempo restante de movimiento a doble velocidad
         * @param Un número entre 0 y 99
         */
        void setHighSpeed ( unsigned int highSpeed ) {  this->highSpeed = highSpeed ;  }

        /**
         * Devuelve el tiempo restante de movimiento a doble velocidad
         * @return Un número entre 0 y 99
         */
        unsigned int getHighSpeed () const {  return this->highSpeed;  }

        /**
         * Establece el número de grandes saltos del jugador
         * @param highJumps Un número entre 0 y 10
         */
        void setHighJumps ( unsigned int highJumps ) {  this->highJumps = highJumps ;  }

        /**
         * Devuelve el número de grandes saltos del jugador
         * @return Un número entre 0 y 10
         */
        unsigned int getHighJumps () const {  return this->highJumps ;  }

        /**
         * Establece el tiempo restante de inmunidad para Head
         * @param Un número de milisegundos
         */
        void setHeadShield ( unsigned int shield ) {  this->headShield = shield ;  }

        /**
         * Establece el tiempo restante de inmunidad para Heels
         * @param Un número de milisegundos
         */
        void setHeelsShield ( unsigned int shield ) {  this->heelsShield = shield ;  }

        /**
         * Devuelve el tiempo restante de inmunidad de un jugador
         * @param player El jugador
         * @return Un número de milisegundos
         */
        double getShield ( const std::string& player ) const ;

        /**
         * Establece la posesión de la bocina por Head
         * @param hasHorn true si tiene la bocina o false en caso contrario
         */
        void setHorn ( bool hasHorn ) {  this->horn = hasHorn ;  }

        /**
         * @return vector with tools owned by the player
         */
        std::vector < std::string > getToolsOwnedByPlayer ( const std::string& player ) const ;

        /**
         * Establece la posesión del bolso por Heels
         * @param hasHorn true si tiene el bolso o false en caso contrario
         */
        void setHandbag ( bool hasHandbag ) {  this->handbag = hasHandbag ;  }

        /**
         * Establece el número de rosquillas que tiene Head
         * @param Un número mayor o igual que 0
         */
        void setDonuts ( unsigned short number ) {  this->donuts = number ;  }

        /**
         * Devuelve el número de rosquillas que tiene Head
         * @return Un número mayor o igual que 0
         */
        unsigned short getDonuts ( const std::string& player ) const ;

        /**
         * Image of the item inside hand bag
         */
        void setItemTaken ( BITMAP* bitmap ) {  this->itemTaken = bitmap ;  }

        /**
         * Número de salas visitadas por los jugadores
         */
        unsigned int getVisitedRooms () const ;

        ItemDataManager * getItemDataManager () const {  return itemDataManager ;  }

        void binItemDataManager () ;

        void setItemDataManager ( ItemDataManager* manager ) {  itemDataManager = manager ;  }

};

}

#endif
