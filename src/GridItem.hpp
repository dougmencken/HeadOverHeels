// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GridItem_hpp_
#define GridItem_hpp_

#include <string>

#include "WrappersAllegro.hpp"

#include "Picture.hpp"
#include "Item.hpp"
#include "Drawable.hpp"


namespace iso
{

class DescriptionOfItem ;

/**
 * Grid items are those which are placed in a particular position and have the same widths
 * as cells of grid have. Grid items are mostly static blocks on which player jumps. You can’t
 * change X and Y coordinates of grid item, you can only change its Z coordinate
 */

class GridItem : public Item, public Drawable
{

public:

        /**
         * @param description Description of item
         * @param cx Position on X of the cell in room where this item is
         * @param cy Position on Y of the cell in room where this item is
         * @param z Position on Z, or how far is floor
         * @param way Initial orientation of item
         */
        GridItem( const DescriptionOfItem * description, int cx, int cy, int z, const std::string& way ) ;

        virtual ~GridItem( ) ;

        virtual std::string whichKindOfItem () const {  return "grid item" ;  }

        bool isSegmentOfWallOnX () const
                {  return getOriginalLabel().find( "wall-x" ) != std::string::npos &&
                                getOriginalLabel().find( "invisible-wall" ) == std::string::npos ;  }

        bool isSegmentOfWallOnY () const
                {  return getOriginalLabel().find( "wall-y" ) != std::string::npos &&
                                getOriginalLabel().find( "invisible-wall" ) == std::string::npos ;  }

        virtual void calculateOffset () ;

        /**
         * Draw this grid item
         */
        virtual void draw () ;

        virtual bool addToPosition ( int x, int y, int z ) ;

        /**
         * Position on X of the cell in room where this item is
         */
        int getCellX () const {  return cell.first ;  }

        /**
         * Position on Y of the cell in room where this item is
         */
        int getCellY () const {  return cell.second ;  }

        unsigned int getColumnOfGrid () const ;

protected:

        /**
         * Position ( X, Y ) of the cell in room where this item is
         */
        std::pair < int, int > cell ;

        virtual void updateImage () ;

        virtual void updateShadow () ;

};

typedef safeptr < GridItem > GridItemPtr ;

}

#endif
