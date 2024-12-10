// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SlideWithHeadAndHeels_hpp_
#define SlideWithHeadAndHeels_hpp_

#include "Slide.hpp"


namespace gui
{

class AnimatedPictureWidget ;


class SlideWithHeadAndHeels : public Slide
{

public :

        SlideWithHeadAndHeels( ) ;

        virtual ~SlideWithHeadAndHeels( ) ;

        void placeHeadAndHeels ( bool imagesToo, bool copyrightsToo ) ;

        void addPictureOfHeadAt ( int x, int y ) ;

        void addPictureOfHeelsAt ( int x, int y ) ;

        virtual void refresh () ;

private :

        AnimatedPictureWidget * pictureOfHead ;

        AnimatedPictureWidget * pictureOfHeels ;

} ;

}

#endif
