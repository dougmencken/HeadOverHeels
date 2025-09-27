// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef UnlikelyToHappenException_hpp_
#define UnlikelyToHappenException_hpp_

#include <exception>
#include <string>

/*
 * Something unexpected, although possible, happened
 */

class UnlikelyToHappenException : public std::exception
{

public:
        explicit UnlikelyToHappenException( const std::string & message ) : theMessage( message ) { }

        explicit UnlikelyToHappenException( const char * message ) : theMessage( message ) { }

        virtual ~UnlikelyToHappenException() noexcept {}

        virtual const char * what() const noexcept
        {
                return theMessage.c_str () ;
        }

private:
        std::string theMessage ;

} ;

#endif
