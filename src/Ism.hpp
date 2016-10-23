// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
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
                ByTeleport2,       /* Segundo telepuerto, salida a otra sala con telepuerto */
                NoEntry,           /* No hay entrada a la sala */
                NoExit,            /* No hay salida de la sala */
                Restart,           /* Se sale de la sala, reiniciándola. Se usa cuando el jugador es destruido */
                Wait               /* Ninguna, espera en la sala */
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

        /**
         * Identificador de los jugadores
         */
        enum PlayerId
        {
                Head = 52,           /* El jugador Head */
                Heels = 53,          /* El jugador Heels */
                HeadAndHeels = 55,   /* El jugador compuesto de Head y Heels */
                NoPlayer = 127       /* No hay jugador, se utiliza en la transición vía telepuertos */
        } ;

        /**
         * Tipo de comportamiento que puede tener un elemento
         */
        enum BehaviorId
        {
                NoBehavior = 0,                     /* Sin comportamiento */
                BubblesBehavior = 1,                /* Representación del transporte de un jugador a través de un telepuerto */
                ConveyorBeltNortheast = 2,          /* Arrastra al elemento que tiene encima en dirección norte o este */
                ConveyorBeltSouthwest = 3,          /* Arrastra al elemento que tiene encima en dirección sur u oeste */
                DetectorBehavior = 4,               /* Detecta al jugador y avanza en la dirección adecuada para darle caza */
                DriveBehavior = 5,                  /* Movimiento en la dirección marcada por un elemento situado encima */
                ElevatorBehavior = 6,               /* Un ascensor */
                Hunter4Behavior = 7,                /* Persigue a un jugador hasta darle caza moviéndose en las direcciones norte, sur, este y oeste */
                HunterWaiting4Behavior = 8,         /* Persigue a un jugador hasta darle caza moviéndose en las direcciones norte, sur, este y oeste. La caza se activa cuando el jugador está a una cierta distancia */
                Hunter8Behavior = 9,                /* Persigue a un jugador hasta darle caza moviéndose en las ocho direcciones posibles */
                HunterWaiting8Behavior = 10,        /* Persigue a un jugador hasta darle caza moviéndose en las ocho direcciones posibles. La caza se activa cuando el jugador está a una cierta distancia */
                ImpelBehavior = 11,                 /* Movimiento en un único sentido provocado por un desplazamiento de otro elemento. Se detiene al chocar con algo */
                MobileBehavior = 12,                /* Se mueve al ser empujado */
                OneWayBehavior = 13,                /* Movimiento en un único sentido, cuando choca con algo gira 180º y prosigue en sentido contrario */
                Patrol4cBehavior = 14,              /* Movimiento aleatorio en las direcciones norte, sur, este y oeste */
                Patrol4dBehavior = 15,              /* Movimiento aleatorio en las direcciones noreste, noroeste, sudeste y sudoeste */
                Patrol8Behavior = 16,               /* Movimiento aleatorio en cualquiera de las ocho direcciones */
                RemoteControlBehavior = 17,         /* Control remoto de otro elemento */
                SinkBehavior = 18,                  /* Movimiento descendente producido cuando un elemento está encima */
                ShotBehavior = 19,                  /* Movimiento en un único sentido, cuando choca con algo lo paraliza, sólo si es un elemento móvil y mortal, luego desaparece */
                SpecialBehavior = 20,               /* Elementos volátiles que dan algún poder al jugador */
                SteerBehavior = 21,                 /* Elemento controlado a distancia por un control remoto */
                SwitchBehavior = 22,                /* Un conmutador que cambia el estado de elementos volátiles a no-volátiles y detiene a los elementos mortales */
                TeleportBehavior = 23,              /* Transporta a un jugador de una sala a otra */
                TrampolineBehavior = 24,            /* Provoca que el jugador que esté encima dé un gran salto */
                TurnLeftBehavior = 25,              /* Movimiento en un único sentido, cuando choca con algo gira 90º a la izquierda y prosigue la marcha */
                TurnRightBehavior = 26,             /* Movimiento en un único sentido, cuando choca con algo gira 90º a la derecha y prosigue la marcha */
                VolatileTimeBehavior = 27,          /* Elemento que se crean al destruir uno volátil */
                VolatileTouchBehavior = 28,         /* Se destruye cuando un jugador lo toca */
                VolatileWeightBehavior = 29,        /* Se destruye cuando un elemento está encima */
                VolatilePuppyBehavior = 30,         /* Se destruye cuando Head o el jugador compuesto está en la sala */
                VolatileHeavyBehavior = 31,         /* Como un volátil por peso pero tarda más en destruirse */
                FlyingOneWayBehavior = 32,          /* Movimiento en un único sentido, cuando choca con algo gira 180º y prosigue en sentido contrario. Flota en el aire */
                CannonBallBehavior = 33,            /* Se mueve hacia el norte, cuando colisiona desaparece como un volátil */
                HeadBehavior = 64,                  /* El jugador Head */
                HeelsBehavior = 65,                 /* El jugador Heels */
                HeadAndHeelsBehavior = 66           /* El jugador compuesto de Head y Heels */
        } ;

        // Estado de los elementos
        enum StateId
        {
                StateWait,                      /* El elemento está quieto */
                StatePush,                      /* El elemento ha sido empujado por otro */
                StateMove,                      /* El elemento se mueve. Estado genérico usado por el gestor de sonido */
                StateMoveNorth = 64,            /* El elemento se mueve al norte */
                StateMoveSouth,                 /* El elemento se mueve al sur */
                StateMoveEast,                  /* El elemento se mueve al este */
                StateMoveWest,                  /* El elemento se mueve al oeste */
                StateMoveNortheast,             /* El elemento se mueve al nordeste */
                StateMoveSoutheast,             /* El elemento se mueve al sudeste */
                StateMoveSouthwest,             /* El elemento se mueve al sudoeste */
                StateMoveNorthwest,             /* El elemento se mueve al noroeste */
                StateMoveUp,                    /* El elemento se mueve arriba */
                StateMoveDown,                  /* El elemento se mueve abajo */
                StateBlink,                     /* El elemento parpadea */
                StateJump,                      /* El elemento salta */
                StateRegularJump,               /* El elemento salta de modo normal */
                StateHighJump,                  /* El elemento salta de modo especial, a mayor altura y desplazamiento */
                StateFall,                      /* El elemento cae */
                StateGlide,                     /* El elemento planea */
                StateTakeItem,                  /* El elemento coge otro elemento */
                StateTakenItem,                 /* El elemento ha cogido otro elemento */
                StateDropItem,                  /* El elemento suelta a otro elemento previamente cogido */
                StateTakeAndJump,               /* El elemento coge otro elemento y luego salta */
                StateDropAndJump,               /* El elemento deja otro elemento y luego salta */
                StateDisplaceNorth,             /* El elemento es desplazado al norte por otro elemento */
                StateDisplaceSouth,             /* El elemento es desplazado al sur por otro elemento */
                StateDisplaceEast,              /* El elemento es desplazado al este por otro elemento */
                StateDisplaceWest,              /* El elemento es desplazado al oeste por otro elemento */
                StateDisplaceNortheast,         /* El elemento es desplazado al noreste por otro elemento */
                StateDisplaceSoutheast,         /* El elemento es desplazado al sudeste por otro elemento */
                StateDisplaceSouthwest,         /* El elemento es desplazado al sudoeste por otro elemento */
                StateDisplaceNorthwest,         /* El elemento es desplazado al noroeste por otro elemento */
                StateDisplaceUp,                /* El elemento es desplazado hacia arriba por otro elemento */
                StateDisplaceDown,              /* El elemento es desplazado hacia abajo por otro elemento */
                StateStartWayOutTeletransport,  /* El elemento inicia el teletransporte de ida */
                StateWayOutTeletransport,       /* El elemento se está teletransportando a otra sala */
                StateStartWayInTeletransport,   /* El elemento inicia el teletransporte de vuelta */
                StateWayInTeletransport,        /* El elemento se está teletransportando desde otra sala */
                StateAutoMove,                  /* El elemento se mueve automáticamente. Estado genérico usado por el gestor de sonido */
                StateAutoMoveNorth,             /* El elemento se mueve automáticamente al norte */
                StateAutoMoveSouth,             /* El elemento se mueve automáticamente al sur */
                StateAutoMoveEast,              /* El elemento se mueve automáticamente al este */
                StateAutoMoveWest,              /* El elemento se mueve automáticamente al oeste */
                StateForceDisplace,
                StateForceDisplaceNorth,        /* El elemento se desplaza automáticamente al norte */
                StateForceDisplaceSouth,        /* El elemento se desplaza automáticamente al sur */
                StateForceDisplaceEast,         /* El elemento se desplaza automáticamente al este */
                StateForceDisplaceWest,         /* El elemento se desplaza automáticamente al oeste */
                StateCancelDisplaceNorth,       /* El elemento deja de desplazarse automáticamente al norte */
                StateCancelDisplaceSouth,       /* El elemento deja de desplazarse automáticamente al sur */
                StateCancelDisplaceEast,        /* El elemento deja de desplazarse automáticamente al este */
                StateCancelDisplaceWest,        /* El elemento deja de desplazarse automáticamente al oeste */
                StateFreeze,                    /* El elemento está detenido por la acción de un rosquillazo o por la activación de un interruptor */
                StateWakeUp,                    /* El elemento se vuelve a activar por la desactivación del interruptor */
                StateStopTop,                   /* El elemento ha alcanzado su altura máxima. Aplicado a elementos de movimiento vertical */
                StateStopBottom,                /* El elemento ha alcanzado su altura mínima. Aplicado a elementos de movimiento vertical */
                StateStartDestroy,              /* El elemento inicia su destrucción */
                StateDestroy,                   /* El elemento se destruye */
                StateShot,                      /* El elemento dispara */
                StateRebound,                   /* El elemento rebota */
                StateSwitch,                    /* El elemento (un interruptor) cambia de estado */
                StateCollision,                 /* El elemento choca con algo */
                StateActive,                    /* El elemento se acaba de activar */
                Mistake                         /* Estado imposible, se utiliza en el gestor de sonido */
        } ;

        /**
         * Elementos con un significado especial en el juego
         */
        enum MagicItem
        {
                Donuts = 15,
                ExtraLife = 16,
                HighJump = 17,
                HighSpeed = 18,
                Shield = 19,
                Crown = 20,
                Horn = 26,
                Handbag = 30,
                ReincarnationFish = 33
        } ;

        /**
         * Teclas usadas para el control del juego
         */
        enum GameKey
        {
                KeyNorth,        /* Tecla para mover el jugador al norte, izquierda */
                KeySouth,        /* Tecla para mover el jugador al sur, derecha */
                KeyEast,         /* Tecla para mover el jugador al este, arriba */
                KeyWest,         /* Tecla para mover el jugador al oeste, abajo */
                KeyTake,         /* Tecla para coger y dejar objetos */
                KeyJump,         /* Tecla para saltar */
                KeyShoot,        /* Tecla para disparar */
                KeyTakeAndJump,  /* Tecla para coger/dejar un objeto y saltar */
                KeySwap,         /* Tecla para cambiar de jugador */
                KeyHalt,         /* Tecla para detener el juego */
                KeyNone          /* Tecla nula */
        } ;

        /**
         * Resolución horizontal de la pantalla del juego
         */
        const int ScreenWidth = 640 ;

        /**
         * Resolución vertical de la pantalla del juego
         */
        const int ScreenHeight = 480 ;

        /**
         * Valor de la coordenada Z. Indica que el elemento se situará encima de aquel a mayor altura
         */
        const int Top = -1 ;

        /**
         * Altura en unidades isométricas para una capa, es decir, un elemento situado en la rejilla a una
         * altura n estará a n * LayerHeight unidades isométricas del suelo
         */
        const int LayerHeight = 24 ;

        /**
         * Número máximo de capas de una sala. En unidades isométricas la altura máxima de una sala es el
         * resultado del producto LevelHeight * MaxLayers
         */
        const int MaxLayers = 10 ;

        /**
         * Identificador del primer elemento libre que asigna el motor. Debe ser un número impar
         */
        const int FirstFreeId = 21 ;

        /**
         * Identificador del primer elemento rejilla que asigna el motor
         */
        const int FirstGridId = FirstFreeId + 1 ;

        /**
         * Etiqueta del elemento empleado para representar la destrucción de los elementos volátiles
         */
        const short BubblesLabel = 38 ;

        /**
         * Etiqueta del elemento que representa al guarda imperial en espera
         * La cercanía de un jugador provocará su activación
         */
        const short ImperialGuardHeadLabel = 27 ;

        /**
         * Etiqueta del elemento que representa al guarda imperial
         */
        const short ImperialGuardLabel = 45 ;

        /**
         * Etiqueta del primer telepuerto
         */
        const short TeleportLabel = 35 ;

        /**
         * Etiqueta de la bola de cañón
         */
        const short CannonBallLabel = 23 ;

        /**
         * Etiqueta del guardián del trono
         */
        const short ThroneGuard = 66 ;

}

#endif
