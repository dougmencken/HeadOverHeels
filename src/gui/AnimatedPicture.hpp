// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef AnimatedPicture_hpp_
#define AnimatedPicture_hpp_

#include <allegro.h>
#include <algif.h>

#include <string>
#include <vector>

#include "Timer.hpp"
#include "Widget.hpp"


namespace gui
{

class AnimatedPicture : public Widget
{

public:

        AnimatedPicture( int x, int y, std::vector < BITMAP * > frames, double delay, std::string name ) ;

        virtual ~AnimatedPicture( ) ;

        void draw ( BITMAP * where ) ;

        void handleKey ( int rawKey ) {  /* do nothing */  }

        unsigned int getWidth () const ;

        unsigned int getHeight () const ;

private:

        std::vector < BITMAP * > animation ;

        double delayBetweenFrames ;

        size_t theFrame ;

        Timer * animationTimer ;

        std::string nameOfAnimation ;

};

}

#endif
