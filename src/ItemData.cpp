
#include "ItemData.hpp"

namespace isomot
{

ItemData::ItemData( ) :
        label( "" ) ,
        widthX( 0 ) ,
        widthY( 0 ) ,
        height( 0 ) ,
        directionFrames( 0 ) ,
        mortal( false ) ,
        weight( 0 ) ,
        framesDelay( 0.0 ) ,
        speed( 0.0 ) ,
        frameWidth( 0 ) ,
        frameHeight( 0 ) ,
        shadowWidth( 0 ) ,
        shadowHeight( 0 ) ,
        extraFrames( 0 )
{
}

ItemData::~ItemData( )
{
}

}
