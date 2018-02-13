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
         * @param family Family of font to draw characters
         * @param color Color of text
         * @param spacing Space between characters
         */
        Label( const std::string& text, const std::string& family, const std::string& color, int spacing = 0 ) ;

        virtual ~Label( ) ;

        void update () ;

        void changeFontFamily ( const std::string& family ) ;

        void changeColor ( const std::string& color ) ;

        void changeFontFamilyAndColor ( const std::string& family, const std::string& color ) ;

        void draw ( BITMAP* where ) ;

        /**
         * Responde a la pulsación de una tecla
         */
        void handleKey ( int key ) ;

protected:

        static Font * getFontByFamilyAndColor ( const std::string& family, const std::string& color ) ;

        /**
         * Create image of label
         * @param text String of characters in utf-8
         * @param font Font to draw these characters
         * @return The image with pure magenta background and the text
         */
        BITMAP * createImageOfLabel ( const std::string& text, Font * font ) ;

private:

        std::string text ;

        std::string fontFamily ;

        std::string color ;

        int spacing ;

        /**
         * Image made as composition of characters
         */
        BITMAP* buffer ;

        Action* myAction ;

public:

        std::string getText () const {  return this->text ;  }

        void setText( const std::string& newText ) {  this->text = newText ; update () ;  }

        std::string getFontFamily () const {  return this->fontFamily ;  }

        std::string getColor () const {  return this->color ;  }

        Font * getFont () const {  return getFontByFamilyAndColor( fontFamily, color ) ;  }

        int getSpacing () const {  return this->spacing ;  }

        void setSpacing ( int newSpacing ) {  this->spacing = newSpacing ;  }

        unsigned int getWidth () const
        {
                // symbols of game fonts are monospaced
                return utf8StringLength( text ) * ( getFont()->getCharWidth() + spacing ) ;
        }

        unsigned int getHeight () const {  return getFont()->getCharHeight() ;  }

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
