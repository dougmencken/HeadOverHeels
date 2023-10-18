
#include "ItemDescriptions.hpp"

#include "GameManager.hpp"
#include "Color.hpp"

#include "ospaths.hpp"

#include <tinyxml2.h>


namespace iso
{

/* static */
const std::string ItemDescriptions::The_File_Full_Of_Item_Descriptions = "items.xml" ;

/* static */
autouniqueptr< ItemDescriptions > ItemDescriptions::theDescriptions( new ItemDescriptions( ) );

ItemDescriptions::~ItemDescriptions( )
{
        binDescriptions () ;
}

/* private */
void ItemDescriptions::binDescriptions ()
{
        for ( std::map< std::string, const DescriptionOfItem * >::iterator i = descriptionsOfItems.begin () ;
                        i != descriptionsOfItems.end () ; ++ i )
        {
                delete i->second ;
        }
        descriptionsOfItems.clear();

        this->alreadyRead = false ;
}

void ItemDescriptions::readDescriptionsFromFile( const std::string & nameOfXMLFile, bool reRead )
{
        if ( this->alreadyRead ) {
                if ( reRead ) {
                        std::cout << "**RE-READING** the descriptions of items from \"" << nameOfXMLFile << "\"" << std::endl ;
                        binDescriptions () ;
                } else
                        return ;
        }

        tinyxml2::XMLDocument itemsXml;
        tinyxml2::XMLError result = itemsXml.LoadFile( ( ospaths::sharePath() + nameOfXMLFile ).c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                return;
        }

        tinyxml2::XMLElement* root = itemsXml.FirstChildElement( "items" );

        for ( tinyxml2::XMLElement* item = root->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                std::string itemLabel = item->Attribute( "label" ) ; // the label of item
                DescriptionOfItem * newItem = new DescriptionOfItem ( itemLabel );

                // how many various orientations
                tinyxml2::XMLElement* orientations = item->FirstChildElement( "orientations" ) ;
                newItem->setHowManyOrientations( std::atoi( orientations->FirstChild()->ToText()->Value() ) );

                // mortal or harmless
                bool isMortal = false ;

                tinyxml2::XMLElement* mortal = item->FirstChildElement( "mortal" ) ;
                if ( mortal != nilPointer )
                        if ( std::string( mortal->FirstChild()->ToText()->Value() ) == "true" )
                                isMortal = true ;

                newItem->setMortal( isMortal );

                // how long, in milliseconds, it falls
                double itemWeight = 0.0 ;

                tinyxml2::XMLElement* weight = item->FirstChildElement( "weight" ) ;
                if ( weight != nilPointer )
                        itemWeight = static_cast< double >( std::atoi( weight->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

                newItem->setWeight( itemWeight );

                // how many milliseconds this item moves one single isometric unit
                double itemSpeed = 0.0 ;

                tinyxml2::XMLElement* speed = item->FirstChildElement( "speed" ) ;
                if ( speed != nilPointer )
                        itemSpeed = static_cast< double >( std::atoi( speed->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

                newItem->setSpeed( itemSpeed );

                // delay, in milliseconds, between frames in animation sequence
                double itemDelayBetweenFrames = 0.0 ;

                tinyxml2::XMLElement* framesDelay = item->FirstChildElement( "framesDelay" ) ;
                if ( framesDelay != nilPointer )
                        itemDelayBetweenFrames = static_cast< double >( std::atoi( framesDelay->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

                newItem->setDelayBetweenFrames( itemDelayBetweenFrames );

                // the sequence of frames for an orientation
                std::vector< unsigned int > sequence ;
                tinyxml2::XMLElement* frames = item->FirstChildElement( "frames" ) ;
                if ( frames != nilPointer )
                {
                        unsigned int howManyFramesPerOrientation = std::atoi( frames->FirstChild()->ToText()->Value() ) ;

                        for ( unsigned int frame = 0 ; frame < howManyFramesPerOrientation ; frame ++ )
                        {
                                sequence.push_back( frame );
                        }
                }
                else
                {
                        tinyxml2::XMLElement* frame = item->FirstChildElement( "frame" ) ;
                        if ( frame != nilPointer )
                        {
                                for ( tinyxml2::XMLElement* frame = item->FirstChildElement( "frame" ) ;
                                                frame != nilPointer ;
                                                frame = frame->NextSiblingElement( "frame" ) )
                                {
                                        sequence.push_back( std::atoi( frame->FirstChild()->ToText()->Value() ) );
                                }
                        }
                        else
                        {
                                if ( sequence.size () > 0 ) sequence.clear () ;
                                sequence.push_back( 0 ) ;
                        }
                }

                newItem->setSequenceOFrames( sequence ) ;

                tinyxml2::XMLElement* extraFrames = item->FirstChildElement( "extraFrames" ) ;
                // character items have extra frames
                if ( extraFrames != nilPointer )
                        newItem->setHowManyExtraFrames( std::atoi( extraFrames->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* picture = item->FirstChildElement( "picture" ) ;
                if ( picture != nilPointer )
                {
                        // the name of file with item's graphics
                        newItem->setNameOfFile( picture->Attribute( "file" ) );

                        // the width and height, in pixels, of a single frame
                        tinyxml2::XMLElement* frameWidth = picture->FirstChildElement( "frameWidth" ) ;
                        newItem->setWidthOfFrame( std::atoi( frameWidth->FirstChild()->ToText()->Value() ) );
                        tinyxml2::XMLElement* frameHeight = picture->FirstChildElement( "frameHeight" ) ;
                        newItem->setHeightOfFrame( std::atoi( frameHeight->FirstChild()->ToText()->Value() ) );
                }
                else if ( newItem->getLabel() == "invisible-wall-x" || newItem->getLabel() == "invisible-wall-y" )
                {
                        newItem->setNameOfFile( "" );
                        newItem->setWidthOfFrame( 64 );
                        newItem->setHeightOfFrame( 115 );
                }

                tinyxml2::XMLElement* shadow = item->FirstChildElement( "shadow" ) ;
                if ( shadow != nilPointer ) // item may have no shadow
                {
                        newItem->setNameOfShadowFile( shadow->Attribute( "file" ) );

                        tinyxml2::XMLElement* shadowWidth = shadow->FirstChildElement( "shadowWidth" ) ;
                        newItem->setWidthOfShadow( std::atoi( shadowWidth->FirstChild()->ToText()->Value() ) );
                        tinyxml2::XMLElement* shadowHeight = shadow->FirstChildElement( "shadowHeight" ) ;
                        newItem->setHeightOfShadow( std::atoi( shadowHeight->FirstChild()->ToText()->Value() ) );
                }

                tinyxml2::XMLElement* door = item->FirstChildElement( "door" ) ;
                bool notDoor = ( door == nilPointer ) ;

                if ( notDoor )
                {
                        newItem->setWidthX( std::atoi( item->FirstChildElement( "widthX" )->FirstChild()->ToText()->Value() ) );
                        newItem->setWidthY( std::atoi( item->FirstChildElement( "widthY" )->FirstChild()->ToText()->Value() ) );
                        newItem->setHeight( std::atoi( item->FirstChildElement( "height" )->FirstChild()->ToText()->Value() ) );

                        descriptionsOfItems[ newItem->getLabel () ] = newItem ;
                }
                else // for door there are three parts, and thus three times three dimensions
                {
                        /* std::string doorAt = door->FirstChild()->ToText()->Value() ; */

                        newItem->setWidthX( 0 );
                        newItem->setWidthY( 0 );
                        newItem->setHeight( 0 );

                        // the three parts of door are the lintel, the left jamb and the right jamb
                        DescriptionOfItem* lintel = DescriptionOfItem::cloneAsLintelOfDoor( *newItem ) ;
                        DescriptionOfItem* leftJamb = DescriptionOfItem::cloneAsLeftJambOfDoor( *newItem ) ;
                        DescriptionOfItem* rightJamb = DescriptionOfItem::cloneAsRightJambOfDoor( *newItem ) ;

                        for ( tinyxml2::XMLElement* widthX = item->FirstChildElement( "widthX" ) ;
                                        widthX != nilPointer ;
                                                widthX = widthX->NextSiblingElement( "widthX" ) )
                        {
                                int wx = std::atoi( widthX->FirstChild()->ToText()->Value() ) ;

                                     if  ( leftJamb->getWidthX() == 0 ) leftJamb->setWidthX( wx );
                                else if ( rightJamb->getWidthX() == 0 ) rightJamb->setWidthX( wx );
                                else if    ( lintel->getWidthX() == 0 ) lintel->setWidthX( wx );
                        }

                        for ( tinyxml2::XMLElement* widthY = item->FirstChildElement( "widthY" ) ;
                                        widthY != nilPointer ;
                                                widthY = widthY->NextSiblingElement( "widthY" ) )
                        {
                                int wy = std::atoi( widthY->FirstChild()->ToText()->Value() ) ;

                                     if  ( leftJamb->getWidthY() == 0 ) leftJamb->setWidthY( wy );
                                else if ( rightJamb->getWidthY() == 0 ) rightJamb->setWidthY( wy );
                                else if    ( lintel->getWidthY() == 0 ) lintel->setWidthY( wy );
                        }

                        for ( tinyxml2::XMLElement* height = item->FirstChildElement( "height" ) ;
                                        height != nilPointer ;
                                                height = height->NextSiblingElement( "height" ) )
                        {
                                int wz = std::atoi( height->FirstChild()->ToText()->Value() ) ;

                                     if  ( leftJamb->getHeight() == 0 ) leftJamb->setHeight( wz );
                                else if ( rightJamb->getHeight() == 0 ) rightJamb->setHeight( wz );
                                else if    ( lintel->getHeight() == 0 ) lintel->setHeight( wz );
                        }

                        descriptionsOfItems[  leftJamb->getLabel () ] = leftJamb ;
                        descriptionsOfItems[ rightJamb->getLabel () ] = rightJamb ;
                        descriptionsOfItems[    lintel->getLabel () ] = lintel ;
                }
        }

        this->alreadyRead = true ;
}

const DescriptionOfItem* ItemDescriptions::getDescriptionByLabel( const std::string & label ) /* const */
{
        if ( ! this->alreadyRead )
                readDescriptionsFromFile( The_File_Full_Of_Item_Descriptions ) ;

        if ( descriptionsOfItems.find( label ) == descriptionsOfItems.end () )
        {
                std::cerr << "the description of item with label \"" << label << "\" is absent" << std::endl ;
                return nilPointer ;
        }

#ifdef __Cxx11__

        return descriptionsOfItems.at( label );

#else

        std::map< std::string, const DescriptionOfItem * >::const_iterator it = descriptionsOfItems.find( label );
        if ( it != descriptionsOfItems.end () ) return it->second ;
        return nilPointer ;

#endif
}

}
