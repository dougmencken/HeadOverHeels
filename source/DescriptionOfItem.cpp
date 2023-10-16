
#include "DescriptionOfItem.hpp"


namespace iso
{

DescriptionOfItem::~DescriptionOfItem( )
{
        sequenceOFrames.clear ();
}

/* private */ /* static */
DescriptionOfItem * DescriptionOfItem::clone ( const DescriptionOfItem & toClone )
{
        DescriptionOfItem * copyOfDescription = new DescriptionOfItem (
                toClone.getLabel (),
                toClone.getWidthX (),
                toClone.getWidthY (),
                toClone.getHeight (),
                toClone.getWeight (),
                toClone.getSpeed (),
                toClone.howManyOrientations (),
                toClone.getDelayBetweenFrames (),
                toClone.getWidthOfFrame (),
                toClone.getHeightOfFrame (),
                toClone.getWidthOfShadow (),
                toClone.getHeightOfShadow (),
                toClone.isMortal (),
                toClone.howManyExtraFrames ()
        );

        // copy the sequence of animation
        if ( toClone.sequenceOFrames.size() > 1 )
        {
                for ( std::vector< unsigned int >::const_iterator fi = toClone.sequenceOFrames.begin () ;
                                fi != toClone.sequenceOFrames.end () ; ++ fi )
                {
                        copyOfDescription->sequenceOFrames.push_back( *fi );
                }
        }
        else // it’s just single 0 for a static item
        {
                copyOfDescription->sequenceOFrames.push_back( 0 );
        }

        copyOfDescription->setNameOfFile( toClone.getNameOfFile () );
        copyOfDescription->setNameOfShadowFile( toClone.getNameOfShadowFile () );

        return copyOfDescription ;
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

}
