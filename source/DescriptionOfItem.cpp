
#include "DescriptionOfItem.hpp"


DescriptionOfItem::~DescriptionOfItem( )
{
        sequenceOFrames.clear ();
}

/* private */ /* static */
DescriptionOfItem * DescriptionOfItem::clone ( const DescriptionOfItem & what )
{
        DescriptionOfItem * theClone = new DescriptionOfItem ( what.getLabel () );

        theClone->setWidthX( what.getWidthX () );
        theClone->setWidthY( what.getWidthY () );
        theClone->setHeight( what.getHeight () );

        theClone->setWeight( what.getWeight () );
        theClone->setSpeed( what.getSpeed () );

        theClone->setMortal( what.isMortal () );

        theClone->setNameOfPicturesFile( what.getNameOfPicturesFile () );

        theClone->setWidthOfFrame( what.getWidthOfFrame () );
        theClone->setHeightOfFrame( what.getHeightOfFrame () );

        theClone->setDelayBetweenFrames( what.getDelayBetweenFrames () );

        theClone->setNameOfShadowsFile( what.getNameOfShadowsFile () );

        theClone->setWidthOfShadow( what.getWidthOfShadow () );
        theClone->setHeightOfShadow( what.getHeightOfShadow () );

        // copy the sequence of animation
        size_t frames = what.sequenceOFrames.size () ;
        if ( frames > 1 ) {
                for ( unsigned int i = 0 ; i < frames ; i ++ )
                        theClone->sequenceOFrames.push_back( what.sequenceOFrames[ i ] );
        } else {
                // it’s just single 0 for a static item
                theClone->sequenceOFrames.push_back( 0 );
        }

        theClone->setHowManyOrientations( what.howManyOrientations () );
        theClone->setHowManyExtraFrames( what.howManyExtraFrames () );

        return theClone ;
}

/* static */
DescriptionOfItem * DescriptionOfItem::cloneAsLintelOfDoor ( const DescriptionOfItem & toClone )
{
        DescriptionOfItem * descriptionOfLintel = DescriptionOfItem::clone( toClone ) ;
        descriptionOfLintel->label += "~lintel" ;
        descriptionOfLintel->partOfDoor = true ;
        return descriptionOfLintel ;
}

/* static */
DescriptionOfItem * DescriptionOfItem::cloneAsLeftJambOfDoor ( const DescriptionOfItem & toClone )
{
        DescriptionOfItem * descriptionOfLeftJamb = DescriptionOfItem::clone( toClone ) ;
        descriptionOfLeftJamb->label += "~leftjamb" ;
        descriptionOfLeftJamb->partOfDoor = true ;
        return descriptionOfLeftJamb ;
}

/* static */
DescriptionOfItem * DescriptionOfItem::cloneAsRightJambOfDoor ( const DescriptionOfItem & toClone )
{
        DescriptionOfItem * descriptionOfRightJamb = DescriptionOfItem::clone( toClone ) ;
        descriptionOfRightJamb->label += "~rightjamb" ;
        descriptionOfRightJamb->partOfDoor = true ;
        return descriptionOfRightJamb ;
}
