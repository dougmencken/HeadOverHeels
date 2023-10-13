// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MayNotBePossible_hpp_
#define MayNotBePossible_hpp_

#include <exception>
#include <string>

/*
 * Something to throw, somewhat really impossible
 */

class MayNotBePossible : public std::exception
{

public:
        explicit MayNotBePossible( const std::string & message ) : theMessage( message ) { }

        explicit MayNotBePossible( const char * message ) : theMessage( message ) { }

        virtual ~MayNotBePossible() noexcept {}

        virtual const char * what() const noexcept
        {
                return theMessage.c_str () ;
        }

private:
        std::string theMessage ;

} ;

#endif
