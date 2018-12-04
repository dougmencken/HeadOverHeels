
#include "DescriptionOfItem.hpp"
#include "Ism.hpp"
#include <iostream>


namespace iso
{

DescriptionOfItem::DescriptionOfItem( ) :
        label( "unlabeled" ) ,
        widthX( 0 ) ,
        widthY( 0 ) ,
        height( 0 ) ,
        weight( 0 ) ,
        speed( 0.0 ) ,
        orientations( 0 ) ,
        delayBetweenFrames( 0.0 ) ,
        widthOfFrame( 0 ) ,
        heightOfFrame( 0 ) ,
        widthOfShadow( 0 ) ,
        heightOfShadow( 0 ) ,
        mortal( false ) ,
        extraFrames( 0 ) ,
        partOfDoor( false ) ,
        descriptions( nilPointer )
{

}

DescriptionOfItem::~DescriptionOfItem( )
{
        frames.clear ();
}

/* static */ DescriptionOfItem* DescriptionOfItem::clone ( const DescriptionOfItem& data )
{
        DescriptionOfItem* copyOfData = new DescriptionOfItem( );

        copyOfData->label = data.getLabel () ;

        copyOfData->widthX = data.widthX;
        copyOfData->widthY = data.widthY;
        copyOfData->height = data.height;
        copyOfData->weight = data.weight;
        copyOfData->speed = data.speed;
        copyOfData->orientations = data.orientations;
        copyOfData->delayBetweenFrames = data.delayBetweenFrames;
        copyOfData->widthOfFrame = data.widthOfFrame;
        copyOfData->heightOfFrame = data.heightOfFrame;
        copyOfData->widthOfShadow = data.widthOfShadow;
        copyOfData->heightOfShadow = data.heightOfShadow;
        copyOfData->mortal = data.mortal;
        copyOfData->extraFrames = data.extraFrames;

        // copy sequence of animation, it’s just single 0 for static items
        if ( data.frames.size() > 1 )
        {
                for ( std::vector< unsigned int >::const_iterator fi = data.frames.begin (); fi != data.frames.end (); ++fi )
                {
                        copyOfData->frames.push_back( *fi );
                }
        }
        else
        {
                copyOfData->frames.push_back( 0 );
        }

        copyOfData->partOfDoor = data.partOfDoor ;
        copyOfData->descriptions = data.descriptions ;

        copyOfData->setNameOfFile( data.getNameOfFile () );
        copyOfData->setNameOfShadowFile( data.getNameOfShadowFile () );

        return copyOfData;
}

}
