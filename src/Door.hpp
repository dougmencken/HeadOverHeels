// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Door_hpp_
#define Door_hpp_

#include "Ism.hpp"
#include "Way.hpp"
#include "Mediated.hpp"


namespace isomot
{

class ItemDataManager ;
class FreeItem ;


/**
 * A door to the room. It is really three free items: two jambs and lintel
 */

class Door : public Mediated
{

public:

       /**
        * @param itemDataManager to find three parts of door
        * @param label label of door
        * @param cx cell on X
        * @param cy cell on Y
        * @param z position on Z or how far is item from ground
        * @param way orientation of door
        */
        Door( ItemDataManager* itemDataManager, const std::string& label, int cx, int cy, int z, const std::string& way ) ;

        virtual ~Door( ) ;

private:

       /**
        * Gestor de los datos invariables de los elementos del juego
        */
        ItemDataManager * itemDataManager ;

        std::string labelOfDoor ;

       /**
        * Cell on X axis of this door
        */
        int cellX ;

       /**
        * Cell on Y axis of this door
        */
        int cellY ;

       /**
        * Spatial position Z or how far is ground
        */
        int z ;

       /**
        * Initial coordinate of entrance
        */
        int rightLimit ;

       /**
        * Final coordinate of entrance
        */
        int leftLimit ;

        std::string positionOfDoor ;

        FreeItem * leftJamb ;

        FreeItem * rightJamb ;

        FreeItem * lintel ;

public:

        bool isUnderDoor ( int x, int y, int z ) ;

        std::string getWhereIsDoor () const {  return positionOfDoor ;  }

        FreeItem * getLeftJamb () ;

        FreeItem * getRightJamb () ;

        FreeItem * getLintel () ;

        int getCellX () const {  return cellX ;  }

        int getCellY () const {  return cellY ;  }

};

}

#endif
