// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Font_hpp_
#define Font_hpp_

#include <map>
#include <string>
#include <vector>

#include "WrappersAllegro.hpp"

#include "Color.hpp"
#include "Picture.hpp"


namespace gui
{

/**
 * The font with letters from the picture file. The letters have colour and can be double height
 */

class Font
{

private:

        static bool readLettersFile () ;

public:

        /**
         * @param color the color of letters
         * @param doubleHeightStretching true for the double height stretching of letters
         */
        Font( const std::string & color, bool doubleHeightStretching = false ) ;

        Font( const Font & toCopy )
                : fontColor( toCopy.getColor() )
                , doubleHeight( toCopy.isDoubleHeight() ) {}

        virtual ~Font( ) {}

        Picture * getPictureOfLetter ( const std::string & letter ) const ;

        const std::string & getColor () const {  return this->fontColor ;  }
        void setColor ( const std::string & newColor ) {  this->fontColor = newColor ;  }

        bool isDoubleHeight () const {  return this->doubleHeight ;  }
        void setDoubleHeight ( bool new2xHeight ) {  this->doubleHeight = new2xHeight ;  }
        void toggleDoubleHeight () {  this->doubleHeight = ! this->doubleHeight ;  }

        unsigned int getWidthOfLetter () const
        {
                return ( Font::imageOfFont != nilPointer ) ? ( Font::imageOfFont->getWidth() / Font::Letters_Per_Line ) : 17 ;
        }

        unsigned int getHeightOfLetter () const
        {
                unsigned int letterHeight = ( Font::imageOfFont != nilPointer ) ? ( Font::imageOfFont->getHeight() / Font::Lines_In_Font ) : 29 ;
                return isDoubleHeight() ? ( letterHeight << 1 ) : letterHeight ;
        }

        unsigned int getWidthOfLetter( const std::string & letter ) const  {  return /* the font is monospaced */ getWidthOfLetter() ;  }
        unsigned int getHeightOfLetter( const std::string & letter ) const {  return /* the font is monospaced */ getHeightOfLetter() ;  }

        // the size of the font image is 272 x 609 pixels, or 16 x 21 letters 17 x 29 pixels each
        static const unsigned int Letters_Per_Line = 16 ;
        static const unsigned int Lines_In_Font = 21 ;

private:

        std::string fontColor ;

        bool doubleHeight ;

        // the letters drawn in the font, read from the font's picture file once for all font instances
        static Picture * imageOfFont ;

        static unsigned int howManyLetters ;

        // the mapping between the UTF-8 string of a letter and the sequential index of an image
        static std::map< std::string /* letter */, unsigned int /* index */ > * mapOfLetters ;

};

}

#endif
