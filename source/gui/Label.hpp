// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Label_hpp_
#define Label_hpp_

#include <string>

#include <WrappersAllegro.hpp>

#include "Widget.hpp"
#include "Font.hpp"


namespace gui
{

class Action ;


/**
 * The line of text
 */

class Label : public Widget
{

public:

        /**
         * @param text The text of this label
         */
        Label( const std::string & text ) ;

        /**
         * @param text the text of this label
         * @param font the font to draw this label
         * @param multicolor true for coloring letters in the cycle
         * @param spacing the space between letters
         */
        Label( const std::string & text, const Font & font, bool multicolor = false, int spacing = 0 ) ;

        virtual ~Label( ) ;

        void update () ;

        virtual void draw () ;

        void handleKey ( const std::string& key ) ;

        const std::string & getText () const {  return this->text ;  }

        void setText( const std::string & newText ) {  this->text = newText ; update () ;  }

        const Font & getFont () const {  return Font::fontByNameAndColor( this->fontName, this->color ) ;  }

        void changeFont( const std::string & nameOFont, const std::string & whichColor );

        void changeFont( const Font & font ) {  changeFont( font.getName (), font.getColor () );  }

        void changeColor ( const std::string & fontColor ) {  changeFont( this->fontName, fontColor );  }

        int getSpacing () const {  return this->spacing ;  }

        void setSpacing ( int newSpacing ) {  this->spacing = newSpacing ; update ();  }

        unsigned int getWidth () const
        {
                return ( imageOfLetters != nilPointer ) ? imageOfLetters->getWidth() : 0 ;
        }

        unsigned int getHeight () const
        {
                return ( imageOfLetters != nilPointer ) ? imageOfLetters->getHeight() : 0 ;
        }

        Action* getAction ( ) const {  return myAction ;  }

        void setAction ( Action* action ) {  myAction = action ;  }

protected:

        /**
         * @param text the string of characters in utf-8
         */
        virtual void createImageOfLabel ( const std::string & text ) ;

        Picture * imageOfLetters ;

private:

        std::string text ;

        std::string fontName ;

        std::string color ;

        bool multicolored ;

        int spacing ;

        Action * myAction ;

} ;

}

#endif
