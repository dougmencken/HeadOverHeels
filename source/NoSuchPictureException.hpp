// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NoSuchPictureException_hpp_
#define NoSuchPictureException_hpp_

#include <exception>
#include <string>

/*
 * When throwing an exception is better than returning null
 */

class NoSuchPictureException : public std::exception
{

public :
        explicit NoSuchPictureException( const std::string & message ) : theMessage( message ) { }

        explicit NoSuchPictureException( const char * message ) : theMessage( message ) { }

        virtual ~NoSuchPictureException() noexcept {}

        virtual const char * what() const noexcept
        {
                return theMessage.c_str () ;
        }

private:
        std::string theMessage ;

} ;

#endif
