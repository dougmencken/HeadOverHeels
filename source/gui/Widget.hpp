// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Widget_hpp_
#define Widget_hpp_

#include "WrappersAllegro.hpp"

#include "Drawable.hpp"
#include "KeyHandler.hpp"

#include "util.hpp"

#include <string>
#include <utility>


namespace gui
{

class Screen ;


/**
 * The foundation for creating elements of the user interface
 */

class Widget : public Drawable, public KeyHandler
{

public:

        Widget( ) : whereX( 0 ), whereY( 0 ), onWhatSlide( nilPointer ) { }

        Widget( int x, int y ) : whereX( x ), whereY( y ), onWhatSlide( nilPointer ) { }

        virtual ~Widget( ) { }

        int getX () const {  return this->whereX ;  }
        int getY () const {  return this->whereY ;  }

        virtual void moveTo ( int x, int y ) {  setX( x ); setY( y );  }

        Screen * getContainingSlide () const {  return this->onWhatSlide ;  }
        void setContainingSlide( Screen * theSlide ) {  this->onWhatSlide = theSlide ;  }

        bool isOnSomeSlide() const {  return this->onWhatSlide != nilPointer ;  }

protected:

        void setX ( int x ) {  this->whereX = x ;  }
        void setY ( int y ) {  this->whereY = y ;  }

private:

        // where is this widget on the containing slide
        int whereX ;
        int whereY ;

        Screen* onWhatSlide ;

};

}

#endif
