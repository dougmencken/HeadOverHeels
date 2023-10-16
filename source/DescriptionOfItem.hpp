// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef DescriptionOfItem_hpp_
#define DescriptionOfItem_hpp_

#include <vector>
#include <string>

#include "Picture.hpp"


namespace iso
{

class DescriptionOfItem
{

private:

        DescriptionOfItem ( const std::string & itemLabel
                                        , unsigned int itemWidthX , unsigned int itemWidthY , unsigned int itemHeight
                                        , double itemWeight
                                        , double itemSpeed
                                        , unsigned short itemOrientations
                                        , double itemDelayBetweenFrames
                                        , unsigned int itemWidthOfFrame , unsigned int itemHeightOfFrame
                                        , unsigned int itemWidthOfShadow , unsigned int itemHeightOfShadow
                                        , bool isItemMortal
                                        , unsigned int itemExtraFrames )
                : label( itemLabel )
                , widthX( itemWidthX )
                , widthY( itemWidthY )
                , height( itemHeight )
                , weight( itemWeight )
                , speed( itemSpeed )
                , orientations( itemOrientations )
                , delayBetweenFrames( itemDelayBetweenFrames )
                , widthOfFrame( itemWidthOfFrame )
                , heightOfFrame( itemHeightOfFrame )
                , widthOfShadow( itemWidthOfShadow )
                , heightOfShadow( itemHeightOfShadow )
                , mortal( isItemMortal )
                , extraFrames( itemExtraFrames )
                , partOfDoor( false )
        { }

        static DescriptionOfItem * clone ( const DescriptionOfItem & toClone ) ;

public:

        DescriptionOfItem( const std::string & itemLabel )
                : label( itemLabel )
                , widthX( 0 ) , widthY( 0 ) , height( 0 )
                , weight( 0.0 )
                , speed( 0.0 )
                , orientations( 0 )
                , delayBetweenFrames( 0.0 )
                , widthOfFrame( 0 ) , heightOfFrame( 0 )
                , widthOfShadow( 0 ) , heightOfShadow( 0 )
                , mortal( false )
                , extraFrames( 0 )
                , partOfDoor( false )
        { }

        virtual ~DescriptionOfItem () ;

        static DescriptionOfItem * cloneAsLintelOfDoor ( const DescriptionOfItem & toClone ) ;

        static DescriptionOfItem * cloneAsLeftJambOfDoor ( const DescriptionOfItem & toClone ) ;

        static DescriptionOfItem * cloneAsRightJambOfDoor ( const DescriptionOfItem & toClone ) ;

public:

        const std::string & getNameOfFile () const {  return this->nameOfFile;  }

        void setNameOfFile ( const std::string & newName ) {  this->nameOfFile = newName ;  }

        const std::string & getNameOfShadowFile () const {  return this->nameOfShadowFile;  }

        void setNameOfShadowFile ( const std::string & newNameOfShadow ) {  this->nameOfShadowFile = newNameOfShadow ;  }

        const std::string & getLabel () const {  return label ;  }

        unsigned int getWidthX () const {  return widthX ;  }

        unsigned int getWidthY () const {  return widthY ;  }

        unsigned int getHeight () const {  return height ;  }

        void setWidthX ( unsigned int wx ) {  this->widthX = wx ;  }

        void setWidthY ( unsigned int wy ) {  this->widthY = wy ;  }

        void setHeight ( unsigned int wz ) {  this->height = wz ;  }

        double getWeight () const {  return weight ;  }

        void setWeight( double newWeight ) {  this->weight = newWeight ;  }

        double getSpeed () const {  return speed ;  }

        void setSpeed( double newSpeed ) {  this->speed = newSpeed ;  }

        double getDelayBetweenFrames () const {  return delayBetweenFrames ;  }

        void setDelayBetweenFrames( double newDelay ) {  this->delayBetweenFrames = newDelay ;  }

        unsigned int getWidthOfFrame () const {  return widthOfFrame ;  }

        unsigned int getHeightOfFrame () const {  return heightOfFrame ;  }

        void setWidthOfFrame( unsigned int newWidthOfFrame ) {  this->widthOfFrame = newWidthOfFrame ;  }

        void setHeightOfFrame( unsigned int newHeightOfFrame ) {  this->heightOfFrame = newHeightOfFrame ;  }

        unsigned int getWidthOfShadow () const {  return widthOfShadow ;  }

        unsigned int getHeightOfShadow () const {  return heightOfShadow ;  }

        void setWidthOfShadow( unsigned int newWidthOfShadow ) {  this->widthOfShadow = newWidthOfShadow ;  }

        void setHeightOfShadow( unsigned int newHeightOfShadow ) {  this->heightOfShadow = newHeightOfShadow ;  }

        bool isMortal() const {  return mortal ;  }

        void setMortal( bool newMortal ) {  this->mortal = newMortal ;  }

        unsigned short howManyOrientations () const {  return orientations ;  }

        void setHowManyOrientations( unsigned short newOrientations ) {  this->orientations = newOrientations ;  }

        unsigned int howManyExtraFrames () const {  return extraFrames ;  }

        void setHowManyExtraFrames( unsigned int newExtraFrames ) {  this->extraFrames = newExtraFrames ;  }

        void setSequenceOFrames( const std::vector< unsigned int > & newSequence )
        {
                if ( this->sequenceOFrames.size() > 0 ) this->sequenceOFrames.clear () ;
                this->sequenceOFrames = newSequence ;
        }

        unsigned int getFrameAt( unsigned int at ) const
        {
                return ( at < sequenceOFrames.size () ? sequenceOFrames[ at ] : 0 ) ;
        }

        unsigned int howManyFramesPerOrientation () const {  return sequenceOFrames.size () ;  }

        bool isPartOfDoor () const {  return partOfDoor ;  }

private:

        std::string label ;

        /**
         * The three spatial dimensions of the item, along the x, along the y, and height along the z
         */
        unsigned int widthX ;
        unsigned int widthY ;
        unsigned int height ;

        /**
         * The weight of the item in seconds, higher for the bigger speed of falling, zero for no gravity (no falling)
         */
        double weight ;

        /**
         * Time in seconds the item takes to move one single isometric unit
         */
        double speed ;

        /**
         * 1 when there’s only one orientation,
         * 2 if there’re frames for south and west, or
         * 4 there’re frames for each orientation
         */
        unsigned short orientations ;

        double delayBetweenFrames ;

        /**
         * Width in pixels of each item’s frame
         */
        unsigned int widthOfFrame ;

        /**
         * Height in pixels of each item’s frame
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

        /**
         * When true, item takes one life from character on touch
         */
        bool mortal ;

        /**
         * Extra frames such as for jumping
         */
        unsigned int extraFrames ;

        /**
         * The sequence of item's frames for an orientation
         */
        std::vector< unsigned int > sequenceOFrames ;

        /**
         * The file with images for this item
         */
        std::string nameOfFile ;

        /**
         * The file with shadows for this item
         */
        std::string nameOfShadowFile ;

        bool partOfDoor ;

} ;

}

#endif
