// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerItem_hpp_
#define PlayerItem_hpp_

#include <vector>

#include "FreeItem.hpp"
#include "Timer.hpp"
#include "Picture.hpp"


namespace iso
{

class DescriptionOfItem ;
class GameManager ;


/**
 * Item of character controlled by the user
 */

class PlayerItem : public FreeItem
{

public:

        PlayerItem( const DescriptionOfItem* description, int x, int y, int z, const std::string& orientation ) ;

        PlayerItem( const PlayerItem& playerItem ) ;

        virtual ~PlayerItem( ) { }

        virtual std::string whichKindOfItem () const {  return "player item" ;  }

        /**
         * Change activity of player by way of entry to room
         */
        void autoMoveOnEntry ( const std::string& wayOfEntry ) ;

        virtual void behave () ;

        /**
         * Updates behavior of item
         */
        virtual bool update () ;

        void wait () ;

        void fillWithData ( const GameManager & data ) ;

        bool isActiveCharacter () const ;

        void addLives( unsigned char lives ) ;

        void loseLife () ;

        /**
         * Player takes magic item
         * @label Label of item
         */
        void takeTool( const std::string& label ) ;

        void addDoughnuts( const unsigned short howMany ) ;

        void useDoughnut () ;

        /**
         * Activa el movimiento a doble velocidad del personaje
         */
        void activateHighSpeed () ;

        /**
         * El jugador consume una unidad de tiempo del movimiento a doble velocidad
         */
        void decreaseHighSpeed () ;

        /**
         * Añade un número de grandes saltos al jugador
         */
        void addHighJumps ( unsigned char howMany ) ;

        /**
         * El jugador consume un gran salto
         */
        void decreaseHighJumps () ;

        /**
         * Activa la inmunidad del personaje
         */
        void activateShield () ;

        /**
         * El jugador consume una unidad de tiempo de inmunidad
         */
        void decreaseShield () ;

        /**
         * El jugador libera un planeta
         */
        void liberatePlanet() ;

        /**
         * Save game when player meets reincarnation fish
         */
        void save () ;

        void saveAt ( int x, int y, int z ) ;

        virtual bool addToPosition ( int x, int y, int z ) ;

        void placeItemInBag ( const std::string & labelOfItem, const std::string & behavior ) ;

        const DescriptionOfItem * getDescriptionOfTakenItem () const {  return descriptionOfTakenItem ;  }

        const std::string & getBehaviorOfTakenItem () const {  return this->behaviorOfTakenItem ;  }

        unsigned char getLives () const {  return this->lives ;  }

        /**
         * Returns remaining steps at double speed between 0 and 99
         */
        unsigned int getHighSpeed () const {  return this->highSpeed ;  }

        /**
         * Returns number of high jumps between 0 and 10
         */
        unsigned int getHighJumps () const {  return this->highJumps ;  }

        /**
         * Character has its magic item, horn or bag, or not
         */
        bool hasTool ( const std::string& label ) const ;

        unsigned short getDoughnuts () const {  return this->howManyDoughnuts ;  }

        bool hasShield () const {  return shieldRemaining > 0 ;  }

        void setWayOfExit ( const std::string& way ) ;

        const std::string& getWayOfExit () const {  return wayOfExit ;  }

        const std::string& getWayOfEntry () const {  return wayOfEntry ;  }

        void setWayOfEntry ( const std::string& way ) {  wayOfEntry = way ;  }

protected:

        /**
         * See if player crosses limits of room, if yes then change rooms
         */
        bool isCollidingWithLimitOfRoom( const std::string& onWhichWay ) ;

        void setLives ( unsigned char lives ) {  this->lives = lives ;  }

        void setHighSpeed ( unsigned int highSpeed ) {  this->highSpeed = highSpeed ;  }

        void setHighJumps ( unsigned int highJumps ) {  this->highJumps = highJumps ;  }

        void setDoughnuts ( const unsigned short howMany ) {  this->howManyDoughnuts = howMany ;  }

        void setShieldTime ( float seconds ) ;

private:

        /**
         * Número de vidas del jugador
         */
        unsigned char lives ;

        /**
         * Tiempo restante de movimiento a doble velocidad. Si el jugador no posee esta habilidad
         * entonces vale cero. Por observación se concluye que:
         * 1. Si el jugador va paso a paso no gasta
         * 2. Si el jugador se mueve pero colisiona, no gasta
         * 3. Consume una unidad de tiempo cada 8 pasos, siempre y cuando se den 4 seguidos
         * 4. Cuando salta no consume
         */
        unsigned int highSpeed ;

        /**
         * Número de grandes saltos del jugador
         */
        unsigned int highJumps ;

        std::vector< std::string > tools ;

        unsigned short howManyDoughnuts ;

        /**
         * Way by which player leaves room
         */
        std::string wayOfExit ;

        /**
         * How player enters room: through door, or via teleport, or going below floor or above ceiling
         */
        std::string wayOfEntry ;

        /**
         * Timer for immunity, player gets 25 seconds of immunity when rabbit is caught
         */
        autouniqueptr < Timer > shieldTimer ;

        /**
         * How many seconds are remaining of time when character has immunity
         */
        float shieldRemaining ;

        const DescriptionOfItem * descriptionOfTakenItem ;

        std::string behaviorOfTakenItem ;

};

typedef safeptr < PlayerItem > PlayerItemPtr ;

}

#endif
