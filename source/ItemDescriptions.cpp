
#include "ItemDescriptions.hpp"

#include "GameManager.hpp"
#include "Color.hpp"

#include "ospaths.hpp"


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
                        this->alreadyRead = false ;
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
                const std::string itemLabel = item->Attribute( "label" ) ; // the label of item
                DescriptionOfItem * newDescription = new DescriptionOfItem ( itemLabel );
                assert( newDescription != nilPointer );

                // spatial dimensions
                newDescription->setWidthX( std::atoi( item->FirstChildElement( "widthX" )->FirstChild()->ToText()->Value() ) );
                newDescription->setWidthY( std::atoi( item->FirstChildElement( "widthY" )->FirstChild()->ToText()->Value() ) );
                newDescription->setHeight( std::atoi( item->FirstChildElement( "height" )->FirstChild()->ToText()->Value() ) );

                readDescriptionFurther( *item, *newDescription );

                descriptionsOfItems[ itemLabel ] = newDescription ;
        }

        // and now the descriptions of doors
        // for a door there are three parts, and thus three times three dimensions

        for ( tinyxml2::XMLElement* item = root->FirstChildElement( "door" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "door" ) )
        {
                const std::string itemLabel = item->Attribute( "label" ) ; // the label of door
                DescriptionOfItem * wholeDoorDescription = new DescriptionOfItem ( itemLabel );
                assert( wholeDoorDescription != nilPointer );

                readDescriptionFurther( *item, *wholeDoorDescription );

                // the three parts of door are the lintel, the left jamb and the right jamb
                DescriptionOfItem* lintel = DescriptionOfItem::cloneAsLintelOfDoor( *wholeDoorDescription ) ;
                DescriptionOfItem* leftJamb = DescriptionOfItem::cloneAsLeftJambOfDoor( *wholeDoorDescription ) ;
                DescriptionOfItem* rightJamb = DescriptionOfItem::cloneAsRightJambOfDoor( *wholeDoorDescription ) ;

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

        this->alreadyRead = true ;
}

/* private */
void ItemDescriptions::readDescriptionFurther( const tinyxml2::XMLElement & item, DescriptionOfItem & description )
{
        // how long, in seconds, it falls
        double itemWeight = 0.0 ;

        const tinyxml2::XMLElement* weight = item.FirstChildElement( "weight" ) ;
        if ( weight != nilPointer )
                itemWeight = static_cast< double >( std::atoi( weight->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setWeight( itemWeight );

        // how many seconds this item moves one single isometric unit
        double itemSpeed = 0.0 ;

        const tinyxml2::XMLElement* speed = item.FirstChildElement( "speed" ) ;
        if ( speed != nilPointer )
                itemSpeed = static_cast< double >( std::atoi( speed->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setSpeed( itemSpeed );

        // mortal or harmless
        bool isMortal = false ;

        const tinyxml2::XMLElement* mortal = item.FirstChildElement( "mortal" ) ;
        if ( mortal != nilPointer )
                if ( std::string( mortal->FirstChild()->ToText()->Value() ) == "yes" )
                        isMortal = true ;

        description.setMortal( isMortal );

        // graphics for this item
        const tinyxml2::XMLElement* picture = item.FirstChildElement( "picture" ) ;
        if ( picture != nilPointer )
        {
                // the name of file with graphics for this item
                description.setNameOfPicturesFile( picture->Attribute( "file" ) );

                // the width and height, in pixels, of a single frame
                const tinyxml2::XMLElement* frameWidth = picture->FirstChildElement( "width" ) ;
                if ( frameWidth == nilPointer ) frameWidth = picture->FirstChildElement( "frameWidth" ) ;
                if ( frameWidth != nilPointer )
                        description.setWidthOfFrame( std::atoi( frameWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* frameHeight = picture->FirstChildElement( "height" ) ;
                if ( frameHeight == nilPointer ) frameHeight = picture->FirstChildElement( "frameHeight" ) ;
                if ( frameHeight != nilPointer )
                        description.setHeightOfFrame( std::atoi( frameHeight->FirstChild()->ToText()->Value() ) );
        }
        else
        {
                const std::string & itemLabel = description.getLabel ();
                if ( itemLabel == "invisible-wall-x" || itemLabel == "invisible-wall-y" )
                {
                        description.setNameOfPicturesFile( "" );
                        description.setWidthOfFrame( 64 );
                        description.setHeightOfFrame( 115 );
                }
        }

        // delay, in seconds, between frames in the animation sequence
        double itemDelayBetweenFrames = 0.0 ;

        const tinyxml2::XMLElement* betweenFrames = item.FirstChildElement( "betweenFrames" ) ;
        if ( betweenFrames != nilPointer )
                itemDelayBetweenFrames = static_cast< double >( std::atoi( betweenFrames->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setDelayBetweenFrames( itemDelayBetweenFrames );

        // shadows for this item
        const tinyxml2::XMLElement* shadow = item.FirstChildElement( "shadow" ) ;
        if ( shadow != nilPointer ) // item may have no shadow
        {
                description.setNameOfShadowsFile( shadow->Attribute( "file" ) );

                const tinyxml2::XMLElement* shadowWidth = shadow->FirstChildElement( "width" ) ;
                if ( shadowWidth == nilPointer ) shadowWidth = shadow->FirstChildElement( "shadowWidth" ) ;
                if ( shadowWidth != nilPointer )
                        description.setWidthOfShadow( std::atoi( shadowWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* shadowHeight = shadow->FirstChildElement( "height" ) ;
                if ( shadowHeight == nilPointer ) shadowHeight = shadow->FirstChildElement( "shadowHeight" ) ;
                if ( shadowHeight != nilPointer )
                        description.setHeightOfShadow( std::atoi( shadowHeight->FirstChild()->ToText()->Value() ) );
        }

        // the sequence of frames for an orientation is either simple 0,1,... or custom
        const tinyxml2::XMLElement* frames = item.FirstChildElement( "frames" ) ;
        if ( frames != nilPointer )
        {
                unsigned int howManyFramesPerOrientation = std::atoi( frames->FirstChild()->ToText()->Value() ) ;
                description.makeSequenceOFrames( howManyFramesPerOrientation ) ;
        }
        else
        {
                std::vector< unsigned int > sequence ;

                const tinyxml2::XMLElement* frame = item.FirstChildElement( "frame" ) ;
                if ( frame != nilPointer )
                {
                        // the custom sequence
                        for ( const tinyxml2::XMLElement* frame = item.FirstChildElement( "frame" ) ;
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

                description.setSequenceOFrames( sequence ) ;
        }

        // how many various orientations
        const tinyxml2::XMLElement* orientations = item.FirstChildElement( "orientations" ) ;
        description.setHowManyOrientations( std::atoi( orientations->FirstChild()->ToText()->Value() ) );

        // how many extra frames, such as for jumping or blinking character
        const tinyxml2::XMLElement* extraFrames = item.FirstChildElement( "extraFrames" ) ;
        if ( extraFrames != nilPointer )
                description.setHowManyExtraFrames( std::atoi( extraFrames->FirstChild()->ToText()->Value() ) );
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
