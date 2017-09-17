// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Ism_hpp_
#define Ism_hpp_

#include <string>
#include <utility>
#include <functional>
#include <fstream>
#include <cstdlib>
#include <cassert>

#include <allegro.h>
#ifdef __WIN32
    #include <winalleg.h>
#else
    #include <time.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#ifdef ALLEGRO_BIG_ENDIAN
    # define IS_BIG_ENDIAN 1
#else
    # define IS_BIG_ENDIAN 0
#endif

namespace isomot
{

        typedef std::pair< int, int > JumpMotion;

        /**
         * Detiene un subproceso durante un periodo de tiempo
         * @param miliseconds Número de milisegundos que será detenido el subproceso
         */
        void sleep ( unsigned long miliseconds );

        /**
         * Copia un archivo
         * @param from Ruta y nombre del archivo a copiar
         * @param to Ruta y nombre del archivo copia
         */
        void copyFile ( const std::string& from, const std::string& to ) ;

        /**
         * Devuelve la ruta absoluta a la carpeta personal del usuario
         */
        std::string homePath () ;

        /**
         * Devuelve la ruta absoluta a la carpeta donde residen los datos de la aplicación
         */
        std::string sharePath () ;

        /**
         * Objeto-función para la destrucción de los objetos de un contenedor de la STL
         */
        struct DeleteObject
        {
                template < typename T >
                void operator()( const T* ptr ) const
                {
                        if ( ptr != 0 )
                        {
                                delete ptr;
                        }
                }
        } ;

        /**
         * Dirección en la que puede avanzar un elemento o punto de entrada o salida de una sala
         */
        enum Direction
        {
                NoDirection = 0,   /* Valor para elementos con dirección única */
                South = 0,         /* Sur, parte inferior derecha de la pantalla */
                West,              /* Oeste, parte inferior izquierda de la pantalla */
                North,             /* Norte, parte superior izquierda de la pantalla */
                East,              /* Este, parte superior derecha de la pantalla */
                Northeast,         /* Noreste, parte superior de la pantalla */
                Southeast,         /* Sureste, parte derecha de la pantalla */
                Southwest,         /* Suroeste, parte inferior de la pantalla */
                Northwest,         /* Noroeste, parte izquierda de la pantalla */
                Eastnorth,         /* Puerta situada al este, parte superior */
                Eastsouth,         /* Puerta situada al este, parte inferior */
                Westnorth,         /* Puerta situada al oeste, parte superior */
                Westsouth,         /* Puerta situada al oeste, parte inferior */
                Up,                /* Arriba, incremento en el eje Z. Salida a una sala sin suelo */
                Down,              /* Abajo, decremento en el eje Z. Salida por una sala sin suelo */
                ByTeleport,        /* Telepuerto, salida a otra sala con telepuerto */
                ByTeleportToo,     /* Segundo telepuerto, salida a otra sala con telepuerto */
                NoEntry,           /* No hay entrada a la sala */
                NoExit,            /* No hay salida de la sala */
                Restart,           /* Se sale de la sala, reiniciándola. Se usa cuando el jugador es destruido */
                JustWait           /* Ninguna, espera en la sala */
        } ;

        /**
         * Eje donde puede situarse una pared de la sala
         */
        enum Axis
        {
                AxisX,    /* Eje X, equivalente al muro este */
                AxisY     /* Eje Y, equivalente al muro norte */
        } ;

        /**
         * Estado de un elemento respecto a la proyección de sombras
         */
        enum WhichShade
        {
                NoShadow,
                WantShadow,
                AlreadyShady
        } ;

        /**
         * Estado de un elemento respecto a la creación de su máscara
         */
        enum WhichMask
        {
                NoMask,         /* Sin máscara */
                WantMask,
                AlreadyMasked   /* Enmascarado */
        } ;

        /**
         * Datos de los elementos que pueden cambiarse
         */
        enum Datum
        {
                CoordinateX,    /* La coordenada espacial X */
                CoordinateY,    /* La coordenada espacial Y */
                CoordinateZ,    /* La coordenada espacial Z */
                CoordinatesXYZ, /* La posición espacial */
                WidthX,         /* La anchura en el eje X */
                WidthY,         /* La anchura en el eje Y */
                Height,         /* La altura */
        } ;

        /**
         * Modo en que se cambian los datos de los elementos
         */
        enum WhatToDo
        {
                Change,   /* El valor especificado se cambiará por el valor actual */
                Add       /* El valor especificado se sumará al valor actual */
        } ;

        /**
         * Tipo de suelo de una sala
         */
        enum FloorId
        {
                NoFloor,       /* Sin suelo */
                RegularFloor,  /* Suelo normal */
                MortalFloor    /* Suelo mortal */
        } ;

        /**
         * Identificador predefinido de los elementos especiales
         */
        enum SpecialId
        {
                NoSpecialId,       /* Sin identificador */
                EastWall,          /* El muro este, cualquier segmento */
                NorthWall,         /* El muro norte, cualquier segmento */
                WestWall,          /* El muro oeste, cualquier segmento */
                SouthWall,         /* El muro sur, cualquier segmento */
                Floor,             /* El suelo, cualquier loseta */
                Roof,              /* El techo */
                EastBorder,        /* El límite este de la sala. Alcanzarlo implica el cambio de sala */
                NorthBorder,       /* El límite norte de la sala. Alcanzarlo implica el cambio de sala */
                WestBorder,        /* El límite oeste de la sala. Alcanzarlo implica el cambio de sala */
                SouthBorder,       /* El límite sur de la sala. Alcanzarlo implica el cambio de sala */
                NortheastBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta noreste. Alcanzarlo implica el cambio de sala.*/
                NorthwestBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta noroeste. Alcanzarlo implica el cambio de sala */
                SoutheastBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta sureste. Alcanzarlo implica el cambio de sala.*/
                SouthwestBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta suroeste. Alcanzarlo implica el cambio de sala */
                EastnorthBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta este-norte. Alcanzarlo implica el cambio de sala.*/
                EastsouthBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta este-sur. Alcanzarlo implica el cambio de sala */
                WestnorthBorder,   /* En salas triples o cuádruples, el límite existente tras la puerta oeste-norte. Alcanzarlo implica el cambio de sala.*/
                WestsouthBorder    /* En salas triples o cuádruples, el límite existente tras la puerta oeste-sur. Alcanzarlo implica el cambio de sala */
        } ;

        enum ActivityOfItem
        {
                Wait,                           /* El elemento está quieto */
                Push,                           /* El elemento ha sido empujado por otro */
                Move,                           /* El elemento se mueve. Estado genérico usado por el gestor de sonido */
                MoveNorth = 64,                 /* El elemento se mueve al norte */
                MoveSouth,                      /* El elemento se mueve al sur */
                MoveEast,                       /* El elemento se mueve al este */
                MoveWest,                       /* El elemento se mueve al oeste */
                MoveNortheast,                  /* El elemento se mueve al nordeste */
                MoveSoutheast,                  /* El elemento se mueve al sudeste */
                MoveSouthwest,                  /* El elemento se mueve al sudoeste */
                MoveNorthwest,                  /* El elemento se mueve al noroeste */
                MoveUp,                         /* El elemento se mueve arriba */
                MoveDown,                       /* El elemento se mueve abajo */
                Blink,                          /* El elemento parpadea */
                Jump,                           /* El elemento salta */
                RegularJump,                    /* El elemento salta de modo normal */
                HighJump,                       /* El elemento salta de modo especial, a mayor altura y desplazamiento */
                Fall,                           /* El elemento cae */
                Glide,                          /* El elemento planea */
                TakeItem,                       /* El elemento coge otro elemento */
                TakenItem,                      /* El elemento ha cogido otro elemento */
                DropItem,                       /* El elemento suelta a otro elemento previamente cogido */
                TakeAndJump,                    /* El elemento coge otro elemento y luego salta */
                DropAndJump,                    /* El elemento deja otro elemento y luego salta */
                DisplaceNorth,                  /* El elemento es desplazado al norte por otro elemento */
                DisplaceSouth,                  /* El elemento es desplazado al sur por otro elemento */
                DisplaceEast,                   /* El elemento es desplazado al este por otro elemento */
                DisplaceWest,                   /* El elemento es desplazado al oeste por otro elemento */
                DisplaceNortheast,              /* El elemento es desplazado al noreste por otro elemento */
                DisplaceSoutheast,              /* El elemento es desplazado al sudeste por otro elemento */
                DisplaceSouthwest,              /* El elemento es desplazado al sudoeste por otro elemento */
                DisplaceNorthwest,              /* El elemento es desplazado al noroeste por otro elemento */
                DisplaceUp,                     /* El elemento es desplazado hacia arriba por otro elemento */
                DisplaceDown,                   /* El elemento es desplazado hacia abajo por otro elemento */
                StartWayOutTeletransport,       /* El elemento inicia el teletransporte de ida */
                WayOutTeletransport,            /* El elemento se está teletransportando a otra sala */
                StartWayInTeletransport,        /* El elemento inicia el teletransporte de vuelta */
                WayInTeletransport,             /* El elemento se está teletransportando desde otra sala */
                AutoMove,                       /* El elemento se mueve automáticamente. Estado genérico usado por el gestor de sonido */
                AutoMoveNorth,                  /* El elemento se mueve automáticamente al norte */
                AutoMoveSouth,                  /* El elemento se mueve automáticamente al sur */
                AutoMoveEast,                   /* El elemento se mueve automáticamente al este */
                AutoMoveWest,                   /* El elemento se mueve automáticamente al oeste */
                ForceDisplace,
                ForceDisplaceNorth,             /* El elemento se desplaza automáticamente al norte */
                ForceDisplaceSouth,             /* El elemento se desplaza automáticamente al sur */
                ForceDisplaceEast,              /* El elemento se desplaza automáticamente al este */
                ForceDisplaceWest,              /* El elemento se desplaza automáticamente al oeste */
                CancelDisplaceNorth,            /* El elemento deja de desplazarse automáticamente al norte */
                CancelDisplaceSouth,            /* El elemento deja de desplazarse automáticamente al sur */
                CancelDisplaceEast,             /* El elemento deja de desplazarse automáticamente al este */
                CancelDisplaceWest,             /* El elemento deja de desplazarse automáticamente al oeste */
                Freeze,                         /* El elemento está detenido por la acción de un rosquillazo o por la activación de un interruptor */
                WakeUp,                         /* El elemento se vuelve a activar por la desactivación del interruptor */
                StopTop,                        /* El elemento ha alcanzado su altura máxima. Aplicado a elementos de movimiento vertical */
                StopBottom,                     /* El elemento ha alcanzado su altura mínima. Aplicado a elementos de movimiento vertical */
                StartDestroy,                   /* El elemento inicia su destrucción */
                Destroy,                        /* El elemento se destruye */
                Doughnut,
                Rebound,                        /* El elemento rebota */
                SwitchIt,                       /* El elemento (un interruptor) cambia de estado */
                Collision,                      /* El elemento choca con algo */
                IsActive,
                Mistake                         /* Estado imposible, se utiliza en el gestor de sonido */
        } ;

        const int ScreenWidth = 640 ;

        const int ScreenHeight = 480 ;

        const int Top = -1 ;

        /**
         * Height in isometric units of layer
         * Item in the grid at height n is n * LayerHeight units
         */
        const int LayerHeight = 24 ;

        /**
         * Maximum number of layers in room
         * In isometric units maximum height of room is LayerHeight * MaxLayers
         */
        const int MaxLayers = 10 ;

        /**
         * Identifier of first free element assigned by engine
         * Supposed to be an odd number
         */
        const int FirstFreeId = 21 ;

        /**
         * Identifier of first grid element assigned by engine
         */
        const int FirstGridId = FirstFreeId + 1 ;

}

#endif
