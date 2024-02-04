// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TextField_hpp_
#define TextField_hpp_

#include <string>
#include <list>

#include <WrappersAllegro.hpp>

#include "Widget.hpp"


namespace gui
{

class Label ;


/**
 * Stores text in multiple lines aligned left, center or right
 */

class TextField : public Widget
{

public:

       /**
        * @param width of field
        * @param align horizontal alignment: left, center, or right
        */
        TextField( unsigned int width, const std::string& align ) ;

        virtual ~TextField( ) ;

        virtual void draw () ;

        void handleKey ( const std::string& /* key */ ) { /* do nothing */ }

       /**
        * Añade una nueva línea al campo de texto
        */
        void addLine ( const std::string& text, const std::string& font, const std::string& color ) ;

       /**
        * Change position of text field
        */
        void moveTo ( int x, int y ) ;

        unsigned int getHeightOfField() {  return heightOfField ;  }

        unsigned int getWidthOfField() {  return width ;  }

        std::string getAlignment () {  return alignment ;  }

        void setAlignment( const std::string& newAlignment ) ;

private:

        unsigned int width ;

        std::string alignment ;

        unsigned int heightOfField ;

       /**
        * El campo de texto se compone de etiquetas
        */
        std::list < Label * > lines ;

};

}

#endif
