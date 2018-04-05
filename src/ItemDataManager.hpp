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
#include <allegro.h>
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

        /**
         * Constructor
         * @param fileName Nombre del archivo XML que contiene los datos de los elementos
         */
        ItemDataManager( const std::string& fileName ) ;

        virtual ~ItemDataManager( ) ;

        /**
         * Compose data for items from XML file with definitions of these items
         */
        void loadItems () ;

        void freeItems () ;

        /**
         * Search for item in the list
         * @param label Label of item
         * @return data of item or nil when not found
         */
        ItemData* findDataByLabel ( const std::string& label ) ;

protected:

        /**
         * Extract frames for this item from picture file
         * @param itemData An item with the frames of picture with non-null frameWidth & frameHeight
         * @param gfxPrefix A prefix to where to search for the picture file
         * @return El elemento con el campo motion almacenando los fotogramas ó 0 si se produjo un error
         */
        ItemData * createPictureFrames ( ItemData * itemData, const char* gfxPrefix ) ;

        /**
         * Extract frames for shadow of this item from picture file
         * @param itemData An item with shadow, shadowWidth & shadowHeight fields non-null
         * @param gfxPrefix A prefix to where to search for the shadow file
         * @return El elemento con el campo motion almacenando los fotogramas ó 0 si se produjo un error
         */
        ItemData* createShadowFrames ( ItemData * itemData, const char* gfxPrefix ) ;

private:

        /**
         * Get lintel from door
         */
        BITMAP * cutOutLintel ( BITMAP * door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                unsigned int leftJambWidthX, unsigned int leftJambWidthY,
                                unsigned int rightJambWidthX, unsigned int rightJambWidthY,
                                const std::string& at ) ;

        /**
         * Get left jamb from door
         */
        BITMAP * cutOutLeftJamb ( BITMAP * door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                  /* unsigned int lintelWidthX, */ unsigned int lintelWidthY, unsigned int lintelHeight,
                                  const std::string& at ) ;

        /**
         * Get right jamb from door
         */
        BITMAP * cutOutRightJamb ( BITMAP * door, unsigned int widthX, unsigned int widthY, unsigned int height,
                                   unsigned int lintelWidthX, /* unsigned int lintelWidthY, */ unsigned int lintelHeight,
                                   const std::string& at ) ;

protected:

        std::string nameOfXMLFile ;

        std::list < ItemData * > itemData ;

};

}

#endif
