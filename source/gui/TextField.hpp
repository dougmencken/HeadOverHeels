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
#include <vector>

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
         * @param the width of field
         * @param howToAlign the horizontal alignment: left, center, or right
         */
        TextField( unsigned int widthOField, const std::string & howToAlign )
                : Widget( )
                , width( widthOField )
                , height( 0 )
                , alignment( howToAlign )
                , interlignePercentage( 100 )
        {}

        virtual ~TextField( ) ;

        virtual void draw () ;

        void handleKey ( const std::string& /* key */ ) { /* do nothing */ }

        void appendText ( const std::string & text, bool height2x, const std::string & color ) ;

        virtual void moveTo ( int x, int y ) {  Widget::moveTo( x, y ) ; updatePositions () ;  }

        unsigned int getWidthOfField () const {  return this->width ;  }

        unsigned int getHeightOfField () const {  return this->height ;  }

        const std::string & getAlignment () const {  return this->alignment ;  }

        void setAlignment( const std::string & newAlignment ) {  this->alignment = newAlignment ; updatePositions() ;  }

        short getInterlignePercentage () const {  return this->interlignePercentage ;  }

        void setInterlignePercentage ( short newInterligne ) {  this->interlignePercentage = newInterligne ; updatePositions() ;  }

private:

        unsigned int width ;

        unsigned int height ;

        std::string alignment ;

        /**
         * the next line is going at
         *    previousLabel.getY() + previousLabel.getHeight() * interlignePercentage / 100
         */
        short interlignePercentage ;

        /**
         * The text field is made up of labels
         */
        std::vector < Label * > lines ;

        void updatePositions () ;

};

}

#endif
