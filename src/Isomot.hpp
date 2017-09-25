// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Isomot_hpp_
#define Isomot_hpp_

#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif

#include "csxml/SaveGameXML.hpp"


namespace isomot
{

class ItemDataManager;
class MapManager;


/**
 * El motor isométrico
 */

class Isomot
{

public:

        Isomot( ) ;

        virtual ~Isomot( ) ;

        void prepare () ;

        void beginNewGame () ;

        void continueSavedGame ( const sgxml::players::player_sequence& playerSequence ) ;

        /**
         * Detiene el motor isométrico
         */
        void pause () ;

        void resume () ;

        /**
         * Prepara el motor para iniciar una nueva partida
         */
        void reset () ;

        /**
         * Actualiza el motor isométrico. La actualización se realiza en dos subprocesos diferenciados:
         * Por un lado se actualizan todas las maquinas de estados de los elementos del juegos y por otro
         * lado se dibuja la sala activa
         * @return La imagen donde se ha dibujado la vista isométrica
         */
        BITMAP* update () ;

private:

        void offVidasInfinitas () ;

        /**
         * Gestiona la sala final del juego. Es una sala muy especial porque el usuario no tiene el
         * control de ningún jugador
         */
        void updateEndRoom () ;

private:

        /**
         * Indica si el motor está gestionando la sala final del juego
         */
        bool isEndRoom ;

        /**
         * Imagen donde se dibuja la vista isométrica
         */
        BITMAP* view ;

        ItemDataManager* itemDataManager ;

        MapManager* mapManager ;

public:

        ItemDataManager* getItemDataManager () const ;

        MapManager* getMapManager () const ;

};

}

#endif
