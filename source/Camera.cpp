
#include "Camera.hpp"

#include "Room.hpp"
#include "GamePreferences.hpp"


Camera::Camera( Room * room )
        : room( room )
        , offset( )
        , roomCenterOffset( )
{
        recenterRoom () ;
}

bool Camera::softShiftTo ( const CameraOffset & newOffset )
{
        CameraOffset offsetBefore = this->offset ;
        if ( newOffset == offsetBefore ) return false ;

        int xShift = newOffset.getX() - offsetBefore.getX() ;
        int yShift = newOffset.getY() - offsetBefore.getY() ;
        int stepX = xShift >> 2 ;
        int stepY = yShift >> 2 ;

        if ( stepX == 0 && xShift != 0 ) stepX = xShift >> 1 ;
        if ( stepY == 0 && yShift != 0 ) stepY = yShift >> 1 ;
        if ( stepX == 0 && xShift != 0 ) stepX = xShift ;
        if ( stepY == 0 && yShift != 0 ) stepY = yShift ;

        if ( stepX == 0 && stepY == 0 ) return false ;

        this->offset.setX( offsetBefore.getX() + stepX );
        this->offset.setY( offsetBefore.getY() + stepY );

        return true ;
}

bool Camera::softCenterRoom ()
{
        return softShiftTo( this->roomCenterOffset ) ;
}

void Camera::instantCenterRoom ()
{
        this->offset = this->roomCenterOffset ;
}

void Camera::recenterRoom ()
{
        if ( room != nilPointer ) {
                roomCenterOffset.setX( ( static_cast< int >( room->getWidthOfRoomImage() )
                                                - static_cast< int >( GamePreferences::getScreenWidth() ) ) >> 1 );
                roomCenterOffset.setY( ( static_cast< int >( room->getHeightOfRoomImage() )
                                                - static_cast< int >( GamePreferences::getScreenHeight() ) ) >> 1 );

                roomCenterOffset.addToY( Camera::spaceForAmbiance );
        }

        instantCenterRoom () ;
}

bool Camera::centerOnItem( const DescribedItem & item )
{
        CameraOffset newOffset ;

        // center on the room’s origin at first
        newOffset.setX( room->getX0 () - ( static_cast< int >( GamePreferences::getScreenWidth() ) >> 1 ) );
        newOffset.setY( room->getY0 () - ( static_cast< int >( GamePreferences::getScreenHeight() ) >> 1 ) );
        newOffset.addToY( Camera::spaceForAmbiance );

        // apply offset of the item's image
        newOffset.addToX( item.getImageOffsetX () );
        newOffset.addToY( item.getImageOffsetY () );

        // center the item itself
        int widthOfImage = ( item.getWidthX() + item.getWidthY() ) << 1 ;
        int heightOfImage = item.getWidthY() + item.getHeight() + item.getWidthX() ;
        newOffset.addToX( widthOfImage >> 1 );
        newOffset.addToY( - ( heightOfImage >> 1 ) );

        return softShiftTo( newOffset ) ;
}
