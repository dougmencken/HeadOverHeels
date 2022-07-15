// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FlickeringColor_hpp_
#define FlickeringColor_hpp_

#include "Color.hpp"

#include "Timer.hpp"

#include <pthread.h>


/**
 * Color that changes periodically
 */

class FlickeringColor : public Color
{

public:

        /* default constructor makes blinking black and white changing twice per second */
        FlickeringColor( )
                : Color( Color::blackColor() )
                , first( Color::blackColor() )
                , second( Color::whiteColor() )
                , period( 0.5 )
                , isFirst( true ) {  initTimer() ;  }

        FlickeringColor( const Color& one, const Color& two, float when = 0.5 )
                : Color( one )
                , first( one )
                , second( two )
                , period( when )
                , isFirst( true ) {  initTimer() ;  }

        FlickeringColor( const FlickeringColor& copy )
                : Color( copy.first )
                , first( copy.first )
                , second( copy.second )
                , period( copy.period )
                , isFirst( true ) {  initTimer() ;  }

        virtual ~FlickeringColor( ) ;

        virtual unsigned char getRed () const {  return isFirst ? first.getRed() : second.getRed() ;  }

        virtual unsigned char getGreen () const {  return isFirst ? first.getGreen() : second.getGreen() ;  }

        virtual unsigned char getBlue () const {  return isFirst ? first.getBlue() : second.getBlue() ;  }

        virtual unsigned char getAlpha () const {  return isFirst ? first.getAlpha() : second.getAlpha() ;  }

        bool operator == ( const Color & c ) const {  return c.getRed() == getRed() && c.getGreen() == getGreen() && c.getBlue() == getBlue() && c.getAlpha() == getAlpha() ;  }

        bool operator != ( const Color & c ) const {  return c.getRed() != getRed() || c.getGreen() != getGreen() || c.getBlue() != getBlue() || c.getAlpha() != getAlpha() ;  }

        bool operator == ( const FlickeringColor & fc ) const {  return fc.first == first && fc.second == second && fc.period == period ;  }

        bool operator != ( const FlickeringColor & fc ) const {  return fc.first != first || fc.second != second || fc.period != period ;  }

        virtual AllegroColor toAllegroColor () const {  return isFirst ? first.toAllegroColor() : second.toAllegroColor() ;  }

        virtual std::string toString () const ;

        static const FlickeringColor & flickerWhiteAndTransparent () {  return *whiteAndTransparent ;  }

        static const FlickeringColor & flickerGray75AndTransparent () {  return *gray75AndTransparent ;  }

protected:

        Color first ;

        Color second ;

        float period ;

        static void * flicker ( void * thiz ) ;

private:

        Timer * changeColorTimer ;

        pthread_t thread ;

        bool isFirst ;

        void initTimer () ;

        static const FlickeringColor * whiteAndTransparent ;

        static const FlickeringColor * gray75AndTransparent ;

} ;


#endif
