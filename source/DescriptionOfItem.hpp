// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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


/**
 * The description of an item as read from items.xml
 */

class DescriptionOfItem
{

public:

        DescriptionOfItem( const std::string & kindOfItem )
                : kind( kindOfItem )
                , widthX( 0 ) , widthY( 0 ) , height( 0 )
                , weight( 0.0 )
                , speed( 0.0 )
                , mortal( false )
                , widthOfFrame( 0 ) , heightOfFrame( 0 )
                , delayBetweenFrames( 0.0 )
                , widthOfShadow( 0 ) , heightOfShadow( 0 )
                , orientations( 0 )
                , extraFrames( 0 )
                , partOfDoor( false )
        { }

        virtual ~DescriptionOfItem () ;

protected:

        DescriptionOfItem * clone () ;

public:

        const std::string & getKind () const {  return kind ;  }
        void setKind ( const std::string & newKind ) {  this->kind = newKind ;  }

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

        bool isMortal() const {  return mortal ;  }
        void setMortal( bool newMortal ) {  this->mortal = newMortal ;  }

        const std::string & getNameOfFramesFile () const {  return this->nameOfFramesFile ;  }
        void setNameOfFramesFile ( const std::string & newFramesFile ) {  this->nameOfFramesFile = newFramesFile ;  }

        unsigned int getWidthOfFrame () const {  return widthOfFrame ;  }
        void setWidthOfFrame( unsigned int newWidthOfFrame ) {  this->widthOfFrame = newWidthOfFrame ;  }

        unsigned int getHeightOfFrame () const {  return heightOfFrame ;  }
        void setHeightOfFrame( unsigned int newHeightOfFrame ) {  this->heightOfFrame = newHeightOfFrame ;  }

        double getDelayBetweenFrames () const {  return delayBetweenFrames ;  }
        void setDelayBetweenFrames( double newDelay ) {  this->delayBetweenFrames = newDelay ;  }

        const std::string & getNameOfShadowsFile () const {  return this->nameOfShadowsFile ;  }
        void setNameOfShadowsFile ( const std::string & newShadowsFile ) {  this->nameOfShadowsFile = newShadowsFile ;  }

        unsigned int getWidthOfShadow () const {  return widthOfShadow ;  }
        void setWidthOfShadow( unsigned int newWidthOfShadow ) {  this->widthOfShadow = newWidthOfShadow ;  }

        unsigned int getHeightOfShadow () const {  return heightOfShadow ;  }
        void setHeightOfShadow( unsigned int newHeightOfShadow ) {  this->heightOfShadow = newHeightOfShadow ;  }

        void setSequenceOFrames( const std::vector< unsigned int > & newSequence )
        {
                if ( this->sequenceOFrames.size() > 0 ) this->sequenceOFrames.clear () ;
                this->sequenceOFrames = newSequence ;
        }

        void makeSequenceOFrames( unsigned int howMany )
        {
                if ( this->sequenceOFrames.size() > 0 ) this->sequenceOFrames.clear () ;

                for ( unsigned int j = 0 ; j < howMany ; j ++ )
                        this->sequenceOFrames.push_back( j );
        }

        unsigned int getFrameAt( unsigned int at ) const
        {
                return ( at < sequenceOFrames.size () ) ? sequenceOFrames[ at ] : 0 ;
        }

        unsigned int howManyFramesPerOrientation () const {  return sequenceOFrames.size () ;  }

        unsigned short howManyOrientations () const {  return orientations ;  }

        void setHowManyOrientations( unsigned short newOrientations )
        {
                     if ( newOrientations == 1 )  orientations = 1 ;
                else if ( newOrientations == 2 )  orientations = 2 ;
                else if ( newOrientations == 4 )  orientations = 4 ;
                else                              orientations = 0 ;
        }

        unsigned short howManyExtraFrames () const {  return extraFrames ;  }
        void setHowManyExtraFrames( unsigned short newExtraFrames ) {  this->extraFrames = newExtraFrames ;  }

        bool isPartOfDoor () const {  return partOfDoor ;  }
        void markAsPartOfDoor () {  this->partOfDoor = true ;  }

private:

        std::string kind ;

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
         * The time in seconds the item takes to move one single isometric unit
         */
        double speed ;

        /**
         * When true, this item takes one life from the character on touch
         */
        bool mortal ;

        /**
         * The file with graphics for this item
         */
        std::string nameOfFramesFile ;

        /**
         * The width and height in pixels of a single frame of the item’s image
         */
        unsigned int widthOfFrame ;
        unsigned int heightOfFrame ;

	/**
         * The delay, in seconds, between frames in the animation sequence
         */
        double delayBetweenFrames ;

        /**
         * The file with shadows for this item
         */
        std::string nameOfShadowsFile ;

        /**
         * The width and height in pixels of a single frame of the item’s shadow
         */
        unsigned int widthOfShadow ;
        unsigned int heightOfShadow ;

        /**
         * The sequence of item's frames for one orientation
         */
        std::vector< unsigned int > sequenceOFrames ;

        /**
         * 1 if the item is the same on all sides, thus there’s only one orientation,
         * 2 if there’re different images for south and west, or
         * 4 when there’re different images for each orientation
         * 0 otherwise and for new descriptions
         */
        unsigned short orientations ;

        /**
         * Extra frames such as for jumping or blinking
         */
        unsigned short extraFrames ;

        // used when a door is disassembled into three parts
        bool partOfDoor ;

} ;

#endif
