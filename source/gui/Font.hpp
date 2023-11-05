// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Font_hpp_
#define Font_hpp_

#include <string>
#include <vector>

#include "WrappersAllegro.hpp"

#include "Color.hpp"
#include "Picture.hpp"


namespace gui
{

/**
 * The font with letters from the picture file. Letters may have a non-white color or~and the double height
 */

class Font
{

public:

        /**
         * @param name the name of this font
         * @param color the color of letters
         * @param doubleHeight true for the double height stretching of letters
         */
        Font( const std::string & name, const std::string & color, bool doubleHeight = false ) ;

        virtual ~Font( ) ;

        Picture * getPictureOfLetter ( const std::string & letter ) ;

        std::string getName () const  {  return fontName ;  }

        /**
         * Name of font is like color.family, use this to get just family
         */
        std::string getFamily () const {  return fontName.substr( fontName.find( "." ) + 1 ) ;  }

        std::string getColor () const {  return fontName.substr( 0, fontName.find( "." ) ) ;  }

        unsigned int getCharWidth () const  {  return charWidth ;  }

        unsigned int getCharHeight () const  {  return charHeight ;  }

private:

        Font( const Font& ) { }

        std::string fontName ;

        std::string fontColor ;

        /**
         * Width, in pixels, of each letter, game fonts are monospaced
         */
        unsigned int charWidth ;

        /**
         * Height, in pixels, of each letter
         */
        unsigned int charHeight ;

        /**
         * Images of letters
         */
        std::vector < Picture * > letters ;

        static Picture * imageOfFont ;

        static unsigned int howManyLetters ;

        /**
         * The letters drawn in the font
         * for mapping between the image of letter from the font's picture file and the UTF-8 code of this letter
         */
        static std::string * listOfLetters ;

};

}

#endif
