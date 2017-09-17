// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Exception_hpp_
#define Exception_hpp_

#include <exception>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

namespace isomot
{

class Exception : public std::exception
{

public:

        explicit Exception( const std::string& message ) ;

        virtual ~Exception( ) throw () ;

        /**
         * Inform in case of exception
         */
        virtual const char * what () const throw () ;

        virtual void raise () const ;

        virtual Exception * clone () const ;

private:

        /**
         * Information about exception
         */
        std::string message;

};

/**
 * Exception for handling null pointers
 */
struct ENullPointer : public Exception
{
        explicit ENullPointer () ;
};

/**
 * Function template for asserting
 */
template < class E, class C > inline void Assert ( C condition )
{
        if ( ! condition ) throw E () ;
}

}

#endif
