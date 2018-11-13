// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ColorCyclingLabel_hpp_
#define ColorCyclingLabel_hpp_

#include "Label.hpp"
#include "Timer.hpp"


namespace gui
{

class ColorCyclingLabel : public Label
{

public:

        ColorCyclingLabel( const std::string& text, const std::string& family, int spacing = 0 ) ;

        virtual ~ColorCyclingLabel( ) ;

        virtual void draw ( const allegro::Pict & where ) ;

private:

        virtual void updateImageOfLabel ( const std::string& text, Font * font ) ;

        unsigned short cycle ;

        autouniqueptr < Timer > colorCyclingTimer ;

};

}

#endif
