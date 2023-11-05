
#include "DescriptionOfItem.hpp"


DescriptionOfItem::~DescriptionOfItem( )
{
        sequenceOFrames.clear ();
}

/* protected */
DescriptionOfItem * DescriptionOfItem::clone ()
{
        DescriptionOfItem * theClone = new DescriptionOfItem ( this->getKind () );

        theClone->setWidthX( this->getWidthX () );
        theClone->setWidthY( this->getWidthY () );
        theClone->setHeight( this->getHeight () );

        theClone->setWeight( this->getWeight () );
        theClone->setSpeed( this->getSpeed () );

        theClone->setMortal( this->isMortal () );

        theClone->setNameOfPicturesFile( this->getNameOfPicturesFile () );

        theClone->setWidthOfFrame( this->getWidthOfFrame () );
        theClone->setHeightOfFrame( this->getHeightOfFrame () );

        theClone->setDelayBetweenFrames( this->getDelayBetweenFrames () );

        theClone->setNameOfShadowsFile( this->getNameOfShadowsFile () );

        theClone->setWidthOfShadow( this->getWidthOfShadow () );
        theClone->setHeightOfShadow( this->getHeightOfShadow () );

        // copy the sequence of animation
        size_t frames = this->sequenceOFrames.size () ;
        if ( frames > 1 ) {
                for ( unsigned int i = 0 ; i < frames ; i ++ )
                        theClone->sequenceOFrames.push_back( this->sequenceOFrames[ i ] );
        } else {
                // it’s just single 0 for a static item
                theClone->sequenceOFrames.push_back( 0 );
        }

        theClone->setHowManyOrientations( this->howManyOrientations () );
        theClone->setHowManyExtraFrames( this->howManyExtraFrames () );

        return theClone ;
}
