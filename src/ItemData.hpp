// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ItemData_hpp_
#define ItemData_hpp_

#include <vector>
#include <string>

#include "Picture.hpp"


namespace iso
{

class ItemDataManager ;


/**
 * Container for definition of an item
 */

class ItemData
{

friend class ItemDataManager ;

public:

        ItemData( ) ;

        virtual ~ItemData( ) ;

        static ItemData * clone ( const ItemData & data ) ;

public:

        const std::string& getNameOfFile () const {  return this->nameOfFile;  }

        void setNameOfFile ( const std::string& newName ) {  this->nameOfFile = newName ;  }

        const std::string& getNameOfShadowFile () const {  return this->nameOfShadowFile;  }

        void setNameOfShadowFile ( const std::string& newNameOfShadow ) {  this->nameOfShadowFile = newNameOfShadow ;  }

        const std::string& getLabel () const {  return label ;  }

        unsigned int getWidthX () const {  return widthX ;  }

        unsigned int getWidthY () const {  return widthY ;  }

        unsigned int getHeight () const {  return height ;  }

        float getWeight() const {  return weight ;  }

        float getSpeed() const {  return speed ;  }

        float getDelayBetweenFrames () const {  return delayBetweenFrames ;  }

        unsigned int getWidthOfFrame () const {  return widthOfFrame ;  }

        unsigned int getHeightOfFrame () const {  return heightOfFrame ;  }

        unsigned int getWidthOfShadow () const {  return widthOfShadow ;  }

        unsigned int getHeightOfShadow () const {  return heightOfShadow ;  }

        bool isMortal() const {  return mortal ;  }

        unsigned short howManyOrientations () const {  return orientations ;  }

        unsigned int howManyExtraFrames () const {  return extraFrames ;  }

        unsigned int getFrameAt( size_t at ) const
        {
                return ( at < frames.size () ? frames[ at ] : 0 ) ;
        }

        unsigned int howManyFrames () const {  return frames.size() ;  }

        bool isPartOfDoor () const {  return partOfDoor ;  }

        const ItemDataManager * getItemDataManager () const {  return manager ;  }

private:

        std::string label ;

       /**
        * Spatial width of item along X axis
        */
        unsigned int widthX ;

       /**
        * Spatial width of item along Y axis
        */
        unsigned int widthY ;

       /**
        * Spatial height of item
        */
        unsigned int height ;

       /**
        * Weight of item in seconds, higher for bigger speed of falling, zero for don’t fall
        */
        float weight ;

       /**
        * Time in seconds needed for item to move
        */
        float speed ;

        unsigned short orientations ;

        float delayBetweenFrames ;

       /**
        * Width in pixels of each frame
        */
        unsigned int widthOfFrame ;

       /**
        * Height in pixels of each frame
        */
        unsigned int heightOfFrame ;

       /**
        * Width in pixels of each frame for item’s shadow
        */
        unsigned int widthOfShadow ;

       /**
        * Height in pixels of each frame for item’s shadow
        */
        unsigned int heightOfShadow ;

        bool mortal ;

       /**
        * Extra frames such as frames of jumping
        */
        unsigned int extraFrames ;

       /**
        * Sequence of animation
        */
        std::vector< unsigned int > frames ;

       /**
        * Name of the file that contains frames of this item
        */
        std::string nameOfFile ;

       /**
        * Name of the file that contains shadows for this item
        */
        std::string nameOfShadowFile ;

        bool partOfDoor ;

        ItemDataManager * manager ;

};

}

#endif
