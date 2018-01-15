// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Label_hpp_
#define Label_hpp_

#include <string>
#include <allegro.h>

#include "Gui.hpp"
#include "Widget.hpp"
#include "Font.hpp"


namespace gui
{

class Screen ;
class Action ;

class Label : public Widget
{

public:

        /**
         * Constructor
         * @param text The text of this label
         */
        Label( const std::string& text ) ;

        /**
         * Constructor
         * @param text The text of this label
         * @param fontName Name of font to draw characters
         * @param color Color of text
         * @param spacing Space between characters
         */
        Label( const std::string& text, const std::string& fontName, const std::string& color, int spacing = 0 ) ;

        virtual ~Label( ) ;

        void update () ;

        /**
         * Change font and color of text
         */
        void changeFontAndColor ( const std::string& fontName, const std::string& color ) ;

        void draw ( BITMAP* where ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

protected:

        /**
         * Create image of label
         * @param text A string of characters in utf-8
         * @param font Name of font to draw these characters
         * @param color Color of text
         * @return The image with magenta background and the text
         */
        BITMAP * createImageOfLabel ( const std::string& text, const std::string& fontName, const std::string& color ) ;

private:

        std::string text ;

        std::string fontName ;

        std::string color ;

        Font* font ;

        int spacing ;

        /**
         * Image made as composition of characters
         */
        BITMAP* buffer ;

        Action* myAction ;

public:

        std::string getText () const {  return this->text ;  }

        void setText( const std::string& newText ) {  this->text = newText ; update () ;  }

        std::string getFontName () const {  return this->fontName ;  }

        std::string getColor () const {  return this->color ;  }

        Font* getFont () const {  return this->font ;  }

        int getSpacing () const {  return this->spacing ;  }

        void setSpacing ( int newSpacing ) {  this->spacing = newSpacing ;  }

        unsigned int getWidth () const
        {
                // symbols of game fonts are monospaced
                return utf8StringLength( text ) * ( font->getCharWidth() + spacing ) ;
        }

        unsigned int getHeight () const {  return font->getCharHeight() ;  }

        Action* getAction ( ) const {  return myAction ;  }

        void setAction ( Action* action ) {  myAction = action ;  }

};


struct EqualXYOfLabel : public std::binary_function< Label*, std::pair < int, int >, bool >
{
        bool operator() ( const Label* label, std::pair < int, int > thatXY ) const;
};


struct EqualTextOfLabel : public std::binary_function< Label*, std::string, bool >
{
        bool operator() ( const Label* label, const std::string& text ) const;
};

}

#endif
