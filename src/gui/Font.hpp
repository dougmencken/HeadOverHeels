// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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
 * Font which uses letters from picture file. Letters may have non-white color or~and double height
 */

class Font
{

public:

        /**
         * @param name Name of this font to mention it
         * @param color Color of letters
         * @param doubleHeight Double height of letters
         */
        Font( const std::string& name, const Color & color, bool doubleHeight = false ) ;

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

        Color fontColor ;

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
         * Table of translation between letter in picture file and UTF-8 code of this letter,
         * which is read from file
         */
        static std::string * tableOfLetters ;

};

}

#endif
