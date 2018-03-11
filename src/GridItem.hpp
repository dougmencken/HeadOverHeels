// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GridItem_hpp_
#define GridItem_hpp_

#include <allegro.h>
#include "Ism.hpp"
#include "Item.hpp"
#include "Drawable.hpp"


namespace isomot
{

class ItemData ;

/**
 * Grid items are those which are placed in a particular position and have the same widths
 * as cells of grid have. Grid items are mostly static blocks on which player jumps. You can’t
 * change X and Y coordinates of grid item, you can only change its Z coordinate
 */

class GridItem : public Item, public Drawable
{

public:

        /**
         * @param itemData Data of item
         * @param cx Position on X of the cell in room where this item is
         * @param cy Position on Y of the cell in room where this item is
         * @param z Position on Z, or how far is floor
         * @param way Initial orientation of item
         */
        GridItem( ItemData* itemData, int cx, int cy, int z, const Way& way ) ;

        virtual ~GridItem( ) ;

        virtual std::string whichKindOfItem () const {  return "grid item" ;  }

        /**
         * Draw this grid item
         */
        void draw ( BITMAP * where ) ;

        virtual void changeImage ( BITMAP * newImage ) ;

        virtual void changeShadow ( BITMAP * newShadow ) ;

        /**
         * Change Z coordinate
         * @param value Value to add to current Z coordinate
         * @return true if Z is okay to change or false if there’s a collision
         */
        virtual bool addToZ ( int value ) ;

protected:

        virtual bool updatePosition ( int newValue, const Coordinate& whatToChange, const ChangeOrAdd& what ) ;

protected:

        /**
         * Position ( X, Y ) of the cell in room where this item is
         */
        std::pair < int, int > cell ;

        /**
         * Number of column of items to which this grid item belongs
         */
        int column ;

public:

        /**
         * Position on X of the cell in room where this item is
         */
        int getCellX () const {  return cell.first ;  }

        /**
         * Position on Y of the cell in room where this item is
         */
        int getCellY () const {  return cell.second ;  }

        void setColumn ( int column ) {  this->column = column ;  }

        int getColumn () const {  return column ;  }

};

}

#endif
