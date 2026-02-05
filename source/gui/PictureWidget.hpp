// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PictureWidget_hpp_
#define PictureWidget_hpp_

#include "WrappersAllegro.hpp"

#include "Widget.hpp"
#include "NamedPicture.hpp"


namespace gui
{

class PictureWidget : public Widget
{

public:

        PictureWidget( int x, int y, const Picture & image, const std::string & nameOfPicture )
                : Widget( x, y )
                , picture( image )
        {
                this->picture.setName( nameOfPicture );
        }

        virtual ~PictureWidget( ) { }

        virtual void draw ()
        {
                allegro::drawSprite( picture.getAllegroPict(), getX(), getY() );
        }

        void handleKey ( const std::string& /* key */ ) {  /* do nothing */  }

        const NamedPicture & getPicture () const {  return this->picture ;  }

        unsigned int getWidth () const {  return this->picture.getWidth() ;  }

        unsigned int getHeight () const {  return this->picture.getHeight() ;  }

private:

        NamedPicture picture ;

};

}

#endif
