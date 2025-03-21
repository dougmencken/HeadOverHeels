// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NoSuchThingException_hpp_
#define NoSuchThingException_hpp_

#include <exception>
#include <string>

/*
 * When throwing an exception is better than returning null
 */

class NoSuchThingException : public std::exception
{

public :
        explicit NoSuchThingException( const std::string & message ) : theMessage( message ) { }

        explicit NoSuchThingException( const char * message ) : theMessage( message ) { }

        virtual ~NoSuchThingException() noexcept {}

        virtual const char * what() const noexcept
        {
                return theMessage.c_str () ;
        }

private:
        std::string theMessage ;

} ;

#endif
