
#include "DescriptionOfDoor.hpp"


DescriptionOfDoor::DescriptionOfDoor( const std::string & scene, const std::string & where )
        : DescriptionOfItem( scene + "-door-" + where ) // door's item kind is %scenery%-door-%at%
        , scenery( scene )
        , doorAt( where )
{
        setNameOfFramesFile( getKind() + ".png" );
        setWidthOfFrame( DescriptionOfDoor::WIDTH_OF_DOOR_IMAGE );
        setHeightOfFrame( DescriptionOfDoor::HEIGHT_OF_DOOR_IMAGE );
        setHowManyOrientations( 1 );

        this->lintel = cloneAsLintelOfDoor() ;
        this->leftJamb = cloneAsLeftJambOfDoor() ;
        this->rightJamb = cloneAsRightJambOfDoor() ;
}

DescriptionOfDoor::~DescriptionOfDoor ()
{
        delete this->lintel ;
        delete this->leftJamb ;
        delete this->rightJamb ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsLintelOfDoor ()
{
        DescriptionOfItem * descriptionOfLintel = clone() ;
        descriptionOfLintel->setKind( getKind () + "~lintel" );
        descriptionOfLintel->markAsPartOfDoor ();

        const unsigned int lintelSmallerWidth = 9 ;
        const unsigned int lintelBroaderWidth = 32 ;
        if ( doorAt == "north" || doorAt == "south" ) {
                descriptionOfLintel->setWidthX( lintelSmallerWidth );
                descriptionOfLintel->setWidthY( lintelBroaderWidth );
        } else
           if ( doorAt == "west" || doorAt == "east" ) {
                descriptionOfLintel->setWidthX( lintelBroaderWidth );
                descriptionOfLintel->setWidthY( lintelSmallerWidth );
        }
        descriptionOfLintel->setHeight( 51 );

        return descriptionOfLintel ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsLeftJambOfDoor ()
{
        DescriptionOfItem * descriptionOfLeftJamb = clone() ;
        descriptionOfLeftJamb->setKind( getKind () + "~leftjamb" );
        descriptionOfLeftJamb->markAsPartOfDoor ();

        descriptionOfLeftJamb->setWidthX( 9 );
        descriptionOfLeftJamb->setWidthY( 9 );
        descriptionOfLeftJamb->setHeight( 48 );

        return descriptionOfLeftJamb ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsRightJambOfDoor ()
{
        DescriptionOfItem * descriptionOfRightJamb = clone() ;
        descriptionOfRightJamb->setKind( getKind () + "~rightjamb" );
        descriptionOfRightJamb->markAsPartOfDoor ();

        descriptionOfRightJamb->setWidthX( 9 );
        descriptionOfRightJamb->setWidthY( 9 );
        descriptionOfRightJamb->setHeight( 48 );

        return descriptionOfRightJamb ;
}
