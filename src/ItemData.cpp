
#include "ItemData.hpp"
#include "Ism.hpp"
#include <iostream>


namespace isomot
{

ItemData::ItemData( ) :
        label( "unlabeled" ) ,
        widthX( 0 ) ,
        widthY( 0 ) ,
        height( 0 ) ,
        weight( 0 ) ,
        speed( 0.0 ) ,
        framesPerOrientation( 0 ) ,
        delayBetweenFrames( 0.0 ) ,
        widthOfFrame( 0 ) ,
        heightOfFrame( 0 ) ,
        widthOfShadow( 0 ) ,
        heightOfShadow( 0 ) ,
        mortal( false ) ,
        extraFrames( 0 )
{

}

ItemData::~ItemData( )
{
        clearNameOfFile ();
        clearNameOfShadowFile ();
        frames.clear ();

        for ( std::vector< Picture * >::iterator it = motion.begin (); it != motion.end (); ++ it )
        {
                delete *it ;
        }

        for ( std::vector< Picture * >::iterator it = shadows.begin (); it != shadows.end (); ++ it )
        {
                delete *it ;
        }
}

/* static */ ItemData* ItemData::clone ( const ItemData& data )
{
        ItemData* copyOfData = new ItemData( );

        copyOfData->label = data.getLabel () ;

        copyOfData->widthX = data.widthX;
        copyOfData->widthY = data.widthY;
        copyOfData->height = data.height;
        copyOfData->weight = data.weight;
        copyOfData->speed = data.speed;
        copyOfData->framesPerOrientation = data.framesPerOrientation;
        copyOfData->delayBetweenFrames = data.delayBetweenFrames;
        copyOfData->widthOfFrame = data.widthOfFrame;
        copyOfData->heightOfFrame = data.heightOfFrame;
        copyOfData->widthOfShadow = data.widthOfShadow;
        copyOfData->heightOfShadow = data.heightOfShadow;
        copyOfData->mortal = data.mortal;
        copyOfData->extraFrames = data.extraFrames;

        // copy sequence of animation, it is just one element 0 for static items
        if ( data.frames.size() > 1 )
        {
                for ( std::vector< int >::const_iterator fi = data.frames.begin (); fi != data.frames.end (); ++fi )
                {
                        copyOfData->frames.push_back( *fi );
                }
        }

        copyOfData->setNameOfFile( data.getNameOfFile () );
        copyOfData->setNameOfShadowFile( data.getNameOfShadowFile () );

        return copyOfData;
}

}
