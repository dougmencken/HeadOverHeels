// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NamedMiniatures_hpp_
#define NamedMiniatures_hpp_

#include <map>
#include <string>

#include "Miniature.hpp"

#include "util.hpp"


/**
 * Encapsulates a set of miniatures mapped to their names
 */

class NamedMiniatures
{

public :

        // NamedMiniatures( ) {}

        virtual ~NamedMiniatures( )
        {
                std::map< std::string, Miniature * >::const_iterator it = this->miniatures.begin() ;
                std::map< std::string, Miniature * >::const_iterator end = this->miniatures.end ();
                while ( it != end ) {
                        if ( it->second != nilPointer ) delete it->second ;
                        ++ it ;
                }
        }

        Miniature * getMiniatureByName( const std::string & name ) const
        {
                std::map< std::string, Miniature * >::const_iterator entry = this->miniatures.find( name );
                if ( entry == this->miniatures.end () ) return nilPointer ;
                return entry->second ;
        }

        /**
         * When the new miniature is being set for the same name, the old one is forgotten and deleted
         */
        void setMiniatureForName( const std::string & name, Miniature * miniature )
        {
                Miniature * previous = this->miniatures[ name ] ;
                if ( miniature == previous ) /* nothing to do */ return ;

                this->miniatures[ name ] = miniature ;

                if ( previous != nilPointer ) delete previous ;
        }

private :

        std::map< std::string, Miniature * > miniatures ;

} ;

#endif
