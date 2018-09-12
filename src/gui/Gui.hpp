// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Gui_hpp_
#define Gui_hpp_

#include <string>


namespace gui
{

/*
 * moves the iterator to next unicode character in the string, returns number of bytes skipped
 * from code by Albert Zeyer and Dark Charlie licensed under LGPL https://sourceforge.net/p/openlierox/code/ci/master/tree/include/Unicode.h
 */
template < typename _Iterator1, typename _Iterator2 >
inline size_t incUtf8StringIterator( _Iterator1 & it, const _Iterator2 & last )
{
        if ( it == last ) return 0;
        it ++;
        size_t inc = 1;
        for ( ; last != it; ++inc ) {
                unsigned char c = *it;
                if ( ! ( c & 0x80 ) || ( ( c & 0xC0 ) == 0xC0 ) ) break;
                it++ ;
        }

        return inc;
}

/*
 * gives the real length of UTF-8 encoded std::string
 * from code by Albert Zeyer and Dark Charlie licensed under LGPL https://sourceforge.net/p/openlierox/code/ci/master/tree/include/Unicode.h
 */
inline size_t utf8StringLength( const std::string& str )
{
        size_t len = 0;
        std::string::const_iterator it = str.begin ();
        for ( ; it != str.end (); incUtf8StringIterator ( it, str.end () ) )
                len ++;

        return len ;
}

}

#endif
