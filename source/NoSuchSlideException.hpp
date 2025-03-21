// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NoSuchSlideException_hpp_
#define NoSuchSlideException_hpp_

#include "NoSuchThingException.hpp"


class NoSuchSlideException : public NoSuchThingException
{

public :
        explicit NoSuchSlideException( const std::string & message ) : NoSuchThingException( message ) { }
        explicit NoSuchSlideException( const char * message ) : NoSuchThingException( message ) { }

} ;

#endif
