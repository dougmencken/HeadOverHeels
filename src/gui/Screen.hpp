// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Screen_hpp_
#define Screen_hpp_

#include <list>
#include <allegro.h>
#include "Widget.hpp"


namespace gui
{

class Action ;
class Icon ;

/**
 * Container for elements of user interface
 */

class Screen : public Widget
{

public:

        /**
         * Constructor
         * @param picture Where to draw this screen
         * @param action Action for which this screen is created
         */
        Screen( BITMAP * picture, Action * action ) ;

        virtual ~Screen( ) ;

        /**
         * Dibuja todo el contenido de la pantalla
         * @param where Imagen donde será dibujada
         */
        void draw ( BITMAP* where ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

        /**
         * Añade un nuevo componente a la pantalla
         * @param widget Un componente de la interfaz de usuario
         */
        void addWidget ( Widget* widget ) ;

        size_t countWidgets () {  return widgets.size () ;  }

        void freeWidgets () ;

        void addIconOfHeadAt ( int x, int y ) ;

        void addIconOfHeelsAt ( int x, int y ) ;

        void placeHeadAndHeels ( bool iconsToo, bool copyrightsToo ) ;

private:

        /**
         * Color de fondo
         */
        int backgroundColor ;

        /**
         * Imagen de fondo de la pantalla
         */
        BITMAP * backgroundPicture ;

        /**
         * Imagen donde se volcarán la pantalla
         */
        BITMAP * where ;

        /**
         * Elementos de la interfaz de usuario contenidos en la pantalla
         */
        std::list < Widget * > widgets ;

        Action * actionOfScreen ;

        Action * escapeAction ;

        Widget * keyHandler ;

        Icon * iconOfHead ;

        Icon * iconOfHeels ;

public:

        /**
         * Establece la imagen de fondo de la pantalla
         * @param image Una imagen
         */
        void setBackground ( BITMAP * image ) {  backgroundPicture = image ;  }

        BITMAP * getBackground () const {  return backgroundPicture ;  }

        void refreshBackground () ;

        void refreshIcons () ;

        Action * getActionOfScreen () const {  return actionOfScreen ;  }

        void setEscapeAction ( Action * action ) {  escapeAction = action ;  }

        Action * getEscapeAction () const {  return escapeAction ;  }

        /**
         * Assign the successor of this component in the chain to handle of typing of a key
         */
        void setKeyHandler ( Widget* widget ) {  keyHandler = widget ;  }

        Widget * getKeyHandler () const {  return keyHandler ;  }

};

}

#endif
