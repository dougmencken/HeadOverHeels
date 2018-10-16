// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemDataManager_hpp_
#define ItemDataManager_hpp_

#include <string>
#include <list>
#include <iostream>

#include <WrappersAllegro.hpp>

#include "ItemData.hpp"


namespace isomot
{

/**
 * Manages items of game. Extracts information from XML file, creating list of data that includes
 * graphics and shadows of each item
 */

class ItemDataManager
{

public:

        ItemDataManager( ) ;

        virtual ~ItemDataManager( ) ;

        /**
         * Load data about items from XML file with definitions of these items
         */
        void loadItems ( const std::string& nameOfXMLFile ) ;

        void freeItems () ;

        /**
         * Look for item by label
         * @return data of item or nil when not found
         */
        ItemData * findDataByLabel ( const std::string& label ) ;

protected:

        std::list < ItemData * > dataOfItems ;

        /**
         * Extract frames for this item from file
         * @param data An item
         * @param gfxPrefix A prefix to where to search for picture file
         */
        void createFrames ( ItemData * data, const char * gfxPrefix ) ;

        /**
         * Extract frames for shadow of this item from file
         * @param data An item with shadow
         * @param gfxPrefix A prefix to where to search for shadow file
         */
        void createShadowFrames ( ItemData * data, const char * gfxPrefix ) ;

private:

        /**
         * Get lintel from door
         */
        static Picture * cutOutLintel ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                        unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                        unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                        const std::string& at ) ;

        /**
         * Get left jamb from door
         */
        static Picture * cutOutLeftJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                                const std::string& at ) ;

        /**
         * Get right jamb from door
         */
        static Picture * cutOutRightJamb ( const allegro::Pict & door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                                unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                                const std::string& at ) ;

};

}

#endif
