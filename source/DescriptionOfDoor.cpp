
#include "DescriptionOfDoor.hpp"


DescriptionOfDoor::DescriptionOfDoor( const std::string & scene, const std::string & where )
        : DescriptionOfItem( scene + "-door-" + where ) // door item’s kind is %scenery%-door-%on%
        , scenery( scene )
        , doorOn( where )
{
        setNameOfFramesFile( getKind() + ".png" );
        setWidthOfFrame( DescriptionOfDoor::WIDTH_OF_DOOR_IMAGE );
        setHeightOfFrame( DescriptionOfDoor::HEIGHT_OF_DOOR_IMAGE );
        setHowManyOrientations( 1 );

        this->lintel = cloneAsLintel() ;
        this->leftJamb = cloneAsLeftJamb() ;
        this->rightJamb = cloneAsRightJamb() ;
}

DescriptionOfDoor::~DescriptionOfDoor ()
{
        delete this->lintel ;
        delete this->leftJamb ;
        delete this->rightJamb ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsLintel ()
{
        DescriptionOfItem * descriptionOfLintel = clone() ;
        descriptionOfLintel->setKind( getKind () + "~lintel" );
        descriptionOfLintel->markAsPartOfDoor ();

        const unsigned int lintelSmallerWidth = 9 ;
        const unsigned int lintelBroaderWidth = 32 ;
        if ( doorOn == "north" || doorOn == "south" ) {
                descriptionOfLintel->setWidthX( lintelSmallerWidth );
                descriptionOfLintel->setWidthY( lintelBroaderWidth );
        } else
           if ( doorOn == "west" || doorOn == "east" ) {
                descriptionOfLintel->setWidthX( lintelBroaderWidth );
                descriptionOfLintel->setWidthY( lintelSmallerWidth );
        }
        descriptionOfLintel->setHeight( 51 );

        return descriptionOfLintel ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsLeftJamb ()
{
        DescriptionOfItem * descriptionOfLeftJamb = clone() ;
        descriptionOfLeftJamb->setKind( getKind () + "~leftjamb" );
        descriptionOfLeftJamb->markAsPartOfDoor ();

        descriptionOfLeftJamb->setWidthX( 9 );
        descriptionOfLeftJamb->setWidthY( 9 );
        descriptionOfLeftJamb->setHeight( 48 );

        return descriptionOfLeftJamb ;
}

DescriptionOfItem * DescriptionOfDoor::cloneAsRightJamb ()
{
        DescriptionOfItem * descriptionOfRightJamb = clone() ;
        descriptionOfRightJamb->setKind( getKind () + "~rightjamb" );
        descriptionOfRightJamb->markAsPartOfDoor ();

        descriptionOfRightJamb->setWidthX( 9 );
        descriptionOfRightJamb->setWidthY( 9 );
        descriptionOfRightJamb->setHeight( 48 );

        return descriptionOfRightJamb ;
}
