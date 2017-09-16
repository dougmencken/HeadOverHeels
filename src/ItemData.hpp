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
 * Container for definition of an item read from file
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
        * Label names an item uniquely, there’s no two different items with the same label
        */
        std::string label ;

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
        * Width in pixels of each frame for item
        */
        int frameWidth ;

       /**
        * Height in pixels of each frame for item
        */
        int frameHeight ;

       /**
        * Width in pixels of each frame for item’s shadow
        */
        int shadowWidth ;

       /**
        * Height in pixels of each frame for item’s shadow
        */
        int shadowHeight ;

       /**
        * Extra frames are those that don’t relate to regular motion, such as frames of jump
        */
        int extraFrames ;

       /**
        * Frames that define sequence of animation
        */
        std::vector< int > frames ;

       /**
        * Pictures of item
        */
        std::vector< BITMAP * > motion ;

       /**
        * Pictures of item’s shadow
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
