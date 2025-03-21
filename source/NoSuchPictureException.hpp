// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NoSuchPictureException_hpp_
#define NoSuchPictureException_hpp_

#include "NoSuchThingException.hpp"


class NoSuchPictureException : public NoSuchThingException
{

public :
        explicit NoSuchPictureException( const std::string & message ) : NoSuchThingException( message ) { }
        explicit NoSuchPictureException( const char * message ) : NoSuchThingException( message ) { }

} ;

#endif
