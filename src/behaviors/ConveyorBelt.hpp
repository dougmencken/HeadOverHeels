// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ConveyorBelt_hpp_
#define ConveyorBelt_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

class ConveyorBelt : public Behavior
{

public:

        ConveyorBelt( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~ConveyorBelt( ) ;

        virtual bool update () ;

private:

       /**
        * Timer for speed of dragging items above
        */
        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > animationTimer ;

};

}

#endif
