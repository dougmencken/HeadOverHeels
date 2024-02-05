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

class Label : public Widget
{

public:

        /**
         * @param text The text of this label
         */
        Label( const std::string & text ) ;

        /**
         * @param text the text of this label
         * @param family the family of font
         * @param color the color of text
         * @param spacing the space between letters
         */
        Label( const std::string & text, const std::string & family, const std::string & color, int spacing = 0 ) ;

        virtual ~Label( ) ;

        void update () ;

        void changeFontFamily ( const std::string & family ) ;

        void changeColor ( const std::string & color ) ;

        void changeFontFamilyAndColor ( const std::string & family, const std::string & color ) ;

        virtual void draw () ;

        void handleKey ( const std::string& key ) ;

        const std::string & getText () const {  return this->text ;  }

        void setText( const std::string & newText ) {  this->text = newText ; update () ;  }

        const std::string & getFontFamily () const {  return this->fontFamily ;  }

        const std::string & getColor () const {  return this->color ;  }

        const Font & getFont () const {  return Label::fontByFamilyAndColor( this->fontFamily, this->color ) ;  }

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

        static Font & fontByFamilyAndColor ( const std::string & family, const std::string & color ) ;

protected:

        /**
         * @param text the string of characters in utf-8
         */
        virtual void createImageOfLabel ( const std::string & text ) ;

        Picture * imageOfLetters ;

private:

        std::string text ;

        std::string fontFamily ;

        std::string color ;

        int spacing ;

        Action * myAction ;

} ;

}

#endif
