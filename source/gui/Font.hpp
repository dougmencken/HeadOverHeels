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

        /**
         * @param color the color of letters
         * @param doubleHeightStretching true for the double height stretching of letters
         */
        Font( const std::string & color, bool doubleHeightStretching = false ) ;

        Font( const Font & ) {} // no copying

        static bool readListOfLetters () ;

public:

        virtual ~Font( ) ;

        Picture * getPictureOfLetter ( const std::string & letter ) const ;

        const std::string & getColor () const {  return this->fontColor ;  }

        bool isDoubleHeight () const {  return this->doubleHeight ;  }

        unsigned int getWidthOfLetter( const std::string & letter ) const {  return getPictureOfLetter( letter )->getWidth () ;  }

        unsigned int getHeightOfLetter( const std::string & letter ) const {  return getPictureOfLetter( letter )->getHeight () ;  }

        static const Font & fontWithColor ( const std::string & color ) {  return fontByColorAndSize( color, false );  }
        static const Font & fontWith2xHeightAndColor ( const std::string & color ) {  return fontByColorAndSize( color, true );  }

        static const Font & fontByColorAndSize ( const std::string & color, bool height2x ) ;

private:

        std::string fontColor ;

        bool doubleHeight ;

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

        static std::vector < Font * > fonts ;

};

}

#endif
