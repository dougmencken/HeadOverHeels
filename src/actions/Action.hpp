// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details

#ifndef Action_hpp_
#define Action_hpp_

namespace gui
{

class Screen;

class Action
{

public:

        Action( ) { }

        virtual ~Action( ) { }

        virtual void doIt () = 0 ;

};

class DoNothing : public Action
{

public:
        virtual void doIt () {  }

};

}

#endif
