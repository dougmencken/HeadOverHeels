// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PictureWidget_hpp_
#define PictureWidget_hpp_

#include <string>

#include "WrappersAllegro.hpp"

#include "Widget.hpp"
#include "Picture.hpp"


namespace gui
{

class PictureWidget : public Widget
{

public:

        PictureWidget( int x, int y, Picture * image, const std::string & name ) ;

        virtual ~PictureWidget( ) ;

        virtual void draw ( const allegro::Pict & where ) ;

        void handleKey ( const std::string& /* key */ ) {  /* do nothing */  }

        Picture * getPicture () const {  return picture ;  }

        unsigned int getWidth () const {  return this->picture->getWidth() ;  }

        unsigned int getHeight () const {  return this->picture->getHeight() ;  }

private:

        Picture * picture ;

        std::string nameOfPicture ;

};

}

#endif
