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
#include <vector>

#include <WrappersAllegro.hpp>

#include "Widget.hpp"
#include "Font.hpp"
#include "UnicodeUtilities.hpp"


namespace gui
{

class Action ;


/**
 * The line of text
 */

class Label : public Widget
{

public:

        Label( const std::string & theText ) ;

        /**
         * @param theText the text of this label
         * @param theFont the font to draw this label
         * @param multicolor true for coloring letters in the cycle
         */
        Label( const std::string & theText, Font * theFont, bool multicolor = false ) ;

        virtual ~Label( ) ;

        virtual void draw () ;

        void handleKey ( const std::string & key ) ;

        const std::string & getText () const {  return this->text ;  }
        void setText( const std::string & newText ) {  this->text = newText ; this->howManyLetters = utf8StringLength( newText );  }

        unsigned int getTextLength () const {  return this->howManyLetters ;  }

        const Font & getFont () const {  return * this->font ;  }
        Font & getFontToChange () {  return * this->font ;  }

        virtual bool areLettersCyclicallyColored () const {  return this->cyclicallyColoredLetters ;  }

        int getSpacing () const {  return this->spacing ;  }
        void setSpacing ( int newSpacing ) {  this->spacing = newSpacing ;  }

        unsigned int getWidth () const
        {
                return getTextLength() * ( getFont().getWidthOfLetter() + getSpacing() ) - getSpacing() /* for the last letter */ ;
        }

        unsigned int getHeight () const
        {
                return getFont().getHeightOfLetter() ;
        }

        Action* getAction ( ) const {  return myAction ;  }

        void setAction ( Action* action ) {  myAction = action ;  }

protected:

        virtual Picture* createImageOfLabel () const
        {
                return Label::createImageOfString( getText(), getTextLength(), getSpacing(), getFont(), getColorCycle() );
        }

        // the sequence of colors for a multicolor label
        virtual std::vector< std::string > getColorCycle () const ;

private:

        static Picture* createImageOfString ( const std::string & text, unsigned int textLength, int spacing,
                                                const Font & font, std::vector< std::string > colorCycle ) ;

        std::string text ;

        unsigned int howManyLetters ;

        Font * font ;

        // if true the letters are colored in a cycle
        bool cyclicallyColoredLetters ;

        // the space between letters, can be negative as well
        int spacing ;

        Action * myAction ;

} ;

}

#endif
