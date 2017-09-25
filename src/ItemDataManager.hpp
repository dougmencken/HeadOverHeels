// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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
#include <algorithm>
#include <iostream>
#include <functional>
#include <allegro.h>
#include "ItemData.hpp"
#include "csxml/ItemsXML.hpp"


namespace isomot
{

/**
 * Gestor de datos de los elementos del juego. Extrae la información a partir de la lectura de un
 * archivo XML, creando una lista de datos que incluye también los gráficos y las sombras de cada
 * elemento
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
         * @return Data of item or 0 when not found
         */
        ItemData* findItemByLabel ( const std::string& label ) ;

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
        * Dimensiones de las partes de una puerta
        */
        class DoorMeasures
        {

        public:

                DoorMeasures( ) ;

                /**
                 * Anchura espacial del dintel en el eje X
                 */
                int lintelWidthX ;

                /**
                 * Anchura espacial del dintel en el eje Y
                 */
                int lintelWidthY ;

                /**
                 * Altura espacial del dintel
                 */
                int lintelHeight ;

                /**
                 * Anchura espacial de la jamba izquierda en el eje X
                 */
                int leftJambWidthX ;

                /**
                 * Anchura espacial de la jamba izquierda en el eje Y
                 */
                int leftJambWidthY ;

                /**
                 * Altura espacial de la jamba izquierda
                 */
                int leftJambHeight ;

                /**
                 * Anchura espacial de la jamba derecha en el eje X
                 */
                int rightJambWidthX ;

                /**
                 * Anchura espacial de la jamba derecha en el eje Y
                 */
                int rightJambWidthY ;

                /**
                 * Altura espacial de la jamba derecha
                 */
                int rightJambHeight ;

        };

        /**
         * Get lintel from door
         * @param door Un gráfico con una puerta al completo
         * @param dm Medidas espaciales de las partes de la puerta
         * @param type Punto cardinal al que está orientado el vano de la puerta
         * @return Un gráfico con el dintel de la puerta ó 0 si se produjo algún error
         */
        BITMAP * cutOutLintel ( BITMAP * door, const DoorMeasures& dm, const ixml::door::value type ) ;

        /**
         * Get left jamb from door
         * @param door Un gráfico con una puerta al completo
         * @param dm Medidas espaciales de las partes de la puerta
         * @param type Punto cardinal al que está orientado el vano de la puerta
         * @return Un gráfico con la jamba izquierda de la puerta ó 0 si se produjo algún error
         */
        BITMAP * cutOutLeftJamb ( BITMAP * door, const DoorMeasures& dm, const ixml::door::value type ) ;

        /**
         * Get right jamb from door
         * @param door Un gráfico con una puerta al completo
         * @param dm Medidas espaciales de las partes de la puerta
         * @param type Punto cardinal al que está orientado el vano de la puerta
         * @return Un gráfico con la jamba derecha de la puerta ó 0 si se produjo algún error
         */
        BITMAP * cutOutRightJamb ( BITMAP * door, const DoorMeasures& dm, const ixml::door::value type ) ;

        /**
         * Finalize data of item
         */
        static void finalizeData ( ItemData& itemData ) ;

protected:

        /**
         * Nombre del archivo XML que contiene los datos de los elementos
         */
        std::string nameOfXMLFile ;

        /**
         * Datos de todos los elementos del juego
         */
        std::list < ItemData > itemData ;

};


struct EqualItemData : public std::binary_function< ItemData, std::string, bool >
{
        bool operator() ( const ItemData& itemData, const std::string& label ) const ;
};

}

#endif
