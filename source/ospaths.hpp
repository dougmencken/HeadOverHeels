// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef os_paths_hpp_
#define os_paths_hpp_

#include <string>

namespace ospaths
{

        std::string nameFromPath ( std::string const& path ) ;

        std::string pathToFile ( const std::string & folder, const std::string & file ) ;

        void setPathToGame ( const char * pathToGame ) ;

        /**
         * Returns the absolute path to the user's home folder
         */
        std::string homePath () ;

        /**
         * Returns the absolute path to the game's data folder
         */
        std::string sharePath () ;

        bool folderExists( const std::string & path ) ;

        /**
         * Makes the new folder for the given path if a folder doesn’t exist there already
         */
        bool makeFolder( const std::string & path ) ;

        struct IsPathSeparator
        {
        # ifdef __WIN32
                bool operator() ( char c ) const {  return c == '\\' || c == '/' ;  }
        # else
                bool operator() ( char c ) const {  return c == '/' ;  }
        # endif
        } ;

}

#endif
