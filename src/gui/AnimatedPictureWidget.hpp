// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef AnimatedPictureWidget_hpp_
#define AnimatedPictureWidget_hpp_

#include "Ism.hpp"

#include <string>
#include <vector>

#include "Timer.hpp"
#include "Widget.hpp"


namespace gui
{

class AnimatedPictureWidget : public Widget
{

public:

        AnimatedPictureWidget( int x, int y, const std::vector < allegro::Pict * > & frames, float delay, const std::string& name ) ;

        virtual ~AnimatedPictureWidget( ) ;

        virtual void draw () ;

        void handleKey ( const std::string& /* key */ ) {  /* do nothing */  }

        unsigned int getWidth () const ;

        unsigned int getHeight () const ;

private:

        std::vector < allegro::Pict * > animation ;

        float delayBetweenFrames ;

        size_t theFrame ;

        autouniqueptr < Timer > animationTimer ;

        std::string nameOfAnimation ;

};

}

#endif
