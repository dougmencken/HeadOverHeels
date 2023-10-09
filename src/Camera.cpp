
#include "Camera.hpp"
#include "Room.hpp"
#include "GameManager.hpp"

#include "screen.hpp"


namespace iso
{

Camera::Camera( Room * room )
        : room( room )
        , delta( std::pair< int, int >( 0, 0 ) )
{
        centerRoom ();
}

void Camera::centerRoom ()
{
        delta.first = ( static_cast< int >( room->getWidthOfRoomImage() ) - static_cast< int >( variables::getScreenWidth() ) ) >> 1 ;
        delta.second = ( static_cast< int >( room->getHeightOfRoomImage() ) - static_cast< int >( variables::getScreenHeight() ) ) >> 1 ;
        delta.second += GameManager::spaceForAmbiance ;

        centeredOnItem.clear();
}

bool Camera::centerOnItem( const Item & item )
{
        std::pair< int, int > offsetBefore = delta ;

        // center on room’s origin at first
        delta.first = room->getX0 () - ( static_cast< int >( variables::getScreenWidth() ) >> 1 ) ;
        delta.second = room->getY0 () - ( static_cast< int >( variables::getScreenHeight() ) >> 1 ) ;
        delta.second += GameManager::spaceForAmbiance ;

        // apply offset of item to room’s origin
        delta.first += item.getOffsetX () ;
        delta.second += item.getOffsetY () ;

        // center item itself
        int widthOfItem = ( item.getWidthX() + item.getWidthY() ) << 1 ;
        int heightOfItem = item.getWidthX() + item.getWidthY() + item.getHeight() ;
        delta.first += ( widthOfItem >> 1 );
        delta.second -= ( heightOfItem >> 1 );

        if ( centeredOnItem.empty() )
        {
                centeredOnItem = item.getUniqueName();
        }
        else if ( item.getUniqueName() != centeredOnItem )
        {
                int distanceX = delta.first - offsetBefore.first ;
                int distanceY = delta.second - offsetBefore.second ;
                int stepX = distanceX >> 2 ;
                int stepY = distanceY >> 2 ;

                if ( stepX == 0 && distanceX != 0 ) stepX = distanceX >> 1 ;
                if ( stepY == 0 && distanceY != 0 ) stepY = distanceY >> 1 ;
                if ( stepX == 0 && distanceX != 0 ) stepX = distanceX ;
                if ( stepY == 0 && distanceY != 0 ) stepY = distanceY ;

                if ( stepX != 0 || stepY != 0 )
                {
                        delta.first = offsetBefore.first + stepX ;
                        delta.second = offsetBefore.second + stepY ;
                        centeredOnItem += "+" ;
                }
                else
                {
                        centeredOnItem = item.getUniqueName();
                }
        }

        return delta == offsetBefore ;
}

}
