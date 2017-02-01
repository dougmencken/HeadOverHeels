// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemData_hpp_
#define ItemData_hpp_

#include <vector>
#include <string>
#include <allegro.h>

namespace isomot
{

/**
 * Registro que almacena la definición de un elemento del juego leído de un archivo del disco
 */
class ItemData
{

public:

        ItemData( ) ;

        virtual ~ItemData( ) ;

public:

        std::string getNameOfFile () {  return this->nameOfFile;  }

        void setNameOfFile ( std::string newName ) {  this->nameOfFile = newName ;  }

        void clearNameOfFile () {  this->nameOfFile.clear() ;  }

        std::string getNameOfShadowFile () {  return this->nameOfShadowFile;  }

        void setNameOfShadowFile ( std::string newNameOfShadow ) {  this->nameOfShadowFile = newNameOfShadow ;  }

        void clearNameOfShadowFile () {  this->nameOfShadowFile.clear() ;  }

       /**
        * Etiqueta unívoca del elemento, es decir, no puede haber dos elementos de distinto tipo con la
        * misma etiqueta. Las etiquetas son constantes y no cadenas de caracteres
        */
        short label ;

       /**
        * Anchura espacial del elemento en el eje X
        */
        int widthX ;

       /**
        * Anchura espacial del elemento en el eje Y
        */
        int widthY ;

       /**
        * Altura espacial del elemento
        */
        int height ;

       /**
        * Indica cuántos fotogramas diferentes tiene el elemento para cada una de las direcciones
        * posibles: norte, sur, este y oeste. Los valores posibles son 1, 2 y 4
        */
        unsigned char directionFrames ;

       /**
        * Indica si el elemento es mortal, es decir, si al tocarlo el jugador perderá una vida
        */
        bool mortal ;

       /**
        * El peso del elemento en segundos. Puede ser 0 para indicar que aunque quede suspendido en el
        * aire no caerá o un valor distinto para indicar lo contrario. Cuanto mayor sea este valor mayor
        * será la velocidad de caída
        */
        double weight ;

       /**
        * Tiempo en segundos que será mostrado cada fotograma de la secuencia de animación
        */
        double framesDelay ;

       /**
        * Tiempo en segundos necesario para que el elemento se mueva
        */
        double speed ;

       /**
        * Anchura en píxeles de cada fotograma
        */
        int frameWidth ;

       /**
        * Altura en píxeles de cada fotograma
        */
        int frameHeight ;

       /**
        * Anchura en píxeles de cada sombra
        */
        int shadowWidth ;

       /**
        * Altura en píxeles de cada sombra
        */
        int shadowHeight ;

       /**
        * Fotogramas extra del elemento, es decir, aquellos que no tiene relación con el movimiento como,
        * por ejemplo, los fotogramas de salto
        */
        int extraFrames ;

       /**
        * Indica los fotogramas que forman la secuencia de animación
        */
        std::vector< int > frames ;

       /**
        * Conjunto de fotogramas del elemento
        */
        std::vector< BITMAP * > motion ;

       /**
        * Conjunto de sombras del elemento
        */
        std::vector< BITMAP * > shadows ;

private:

       /**
        * Name of the file that contains the frames of this item
        */
        std::string nameOfFile ;

       /**
        * Name of the file that contains the shadows for this item
        */
        std::string nameOfShadowFile ;

};

}

#endif
