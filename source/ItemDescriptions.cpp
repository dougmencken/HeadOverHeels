
#include "ItemDescriptions.hpp"

#include "DescriptionOfDoor.hpp"
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
                } else
                        return ;
        }

        tinyxml2::XMLDocument itemsXml ;
        tinyxml2::XMLError result = itemsXml.LoadFile( ospaths::pathToFile( ospaths::sharePath(), nameOfXMLFile ).c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t read file \"" << nameOfXMLFile << "\" with the descriptions of items" << std::endl ;
                return ;
        }

        tinyxml2::XMLElement* root = itemsXml.FirstChildElement( "items" );

        for ( tinyxml2::XMLElement* item = root->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                const std::string kindOfItem = item->Attribute( "kind" ) ; // the kind of item
                DescriptionOfItem * newDescription = new DescriptionOfItem ( kindOfItem );
                assert( newDescription != nilPointer );

                // spatial dimensions
                newDescription->setWidthX( std::atoi( item->FirstChildElement( "widthX" )->FirstChild()->ToText()->Value() ) );
                newDescription->setWidthY( std::atoi( item->FirstChildElement( "widthY" )->FirstChild()->ToText()->Value() ) );
                newDescription->setHeight( std::atoi( item->FirstChildElement( "height" )->FirstChild()->ToText()->Value() ) );

                readDescriptionFurther( *item, *newDescription );

                // and at last
                descriptionsOfItems[ kindOfItem ] = newDescription ;
        }

        // and now the descriptions of doors
        // for a door there are three parts, and thus three times three dimensions

        for ( tinyxml2::XMLElement* door = root->FirstChildElement( "door" ) ;
                        door != nilPointer ;
                        door = door->NextSiblingElement( "door" ) )
        {
                const std::string doorScenery = door->Attribute( "scenery" ) ;
                const std::string doorAt = door->Attribute( "at" ) ;

                DescriptionOfDoor * doorDescription = new DescriptionOfDoor ( doorScenery, doorAt );
                assert( doorDescription != nilPointer );

                // the three parts of door
                const DescriptionOfItem* lintel = doorDescription->getLintel ();
                const DescriptionOfItem* leftJamb = doorDescription->getLeftJamb ();
                const DescriptionOfItem* rightJamb = doorDescription->getRightJamb ();

                assert( lintel != nilPointer );
                assert( leftJamb != nilPointer );
                assert( rightJamb != nilPointer );

                // and at last
                descriptionsOfItems[  leftJamb->getKind () ] = leftJamb ;
                descriptionsOfItems[ rightJamb->getKind () ] = rightJamb ;
                descriptionsOfItems[    lintel->getKind () ] = lintel ;
        }

        this->alreadyRead = true ;
}

/* private */
void ItemDescriptions::readDescriptionFurther( const tinyxml2::XMLElement & element, DescriptionOfItem & description )
{
        // how long, in seconds, it falls
        double itemWeight = 0.0 ;

        const tinyxml2::XMLElement* weight = element.FirstChildElement( "weight" ) ;
        if ( weight != nilPointer )
                itemWeight = static_cast< double >( std::atoi( weight->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setWeight( itemWeight );

        // how many seconds this item moves one single isometric unit
        double itemSpeed = 0.0 ;

        const tinyxml2::XMLElement* speed = element.FirstChildElement( "speed" ) ;
        if ( speed != nilPointer )
                itemSpeed = static_cast< double >( std::atoi( speed->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setSpeed( itemSpeed );

        // mortal or harmless
        bool isMortal = false ;

        const tinyxml2::XMLElement* mortal = element.FirstChildElement( "mortal" ) ;
        if ( mortal != nilPointer )
                if ( std::string( mortal->FirstChild()->ToText()->Value() ) == "yes" )
                        isMortal = true ;

        description.setMortal( isMortal );

        // graphics for this item
        const tinyxml2::XMLElement* picture = element.FirstChildElement( "picture" ) ;
        if ( picture != nilPointer )
        {
                // the name of file with graphics for this item
                description.setNameOfPicturesFile( picture->Attribute( "file" ) );

                // the width and height, in pixels, of a single frame
                const tinyxml2::XMLElement* frameWidth = picture->FirstChildElement( "width" ) ;
                if ( frameWidth != nilPointer )
                        description.setWidthOfFrame( std::atoi( frameWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* frameHeight = picture->FirstChildElement( "height" ) ;
                if ( frameHeight != nilPointer )
                        description.setHeightOfFrame( std::atoi( frameHeight->FirstChild()->ToText()->Value() ) );
        }
        else
        {
                const std::string & kindOfItem = description.getKind ();
                if ( kindOfItem == "invisible-wall-x" || kindOfItem == "invisible-wall-y" )
                {
                        description.setNameOfPicturesFile( "" );
                        description.setWidthOfFrame( 64 );
                        description.setHeightOfFrame( 115 );
                }
        }

        // delay, in seconds, between frames in the animation sequence
        double itemDelayBetweenFrames = 0.0 ;

        const tinyxml2::XMLElement* betweenFrames = element.FirstChildElement( "betweenFrames" ) ;
        if ( betweenFrames != nilPointer )
                itemDelayBetweenFrames = static_cast< double >( std::atoi( betweenFrames->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setDelayBetweenFrames( itemDelayBetweenFrames );

        // shadows for this item
        const tinyxml2::XMLElement* shadow = element.FirstChildElement( "shadow" ) ;
        if ( shadow != nilPointer ) // item may have no shadow
        {
                description.setNameOfShadowsFile( shadow->Attribute( "file" ) );

                const tinyxml2::XMLElement* shadowWidth = shadow->FirstChildElement( "width" ) ;
                if ( shadowWidth != nilPointer )
                        description.setWidthOfShadow( std::atoi( shadowWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* shadowHeight = shadow->FirstChildElement( "height" ) ;
                if ( shadowHeight != nilPointer )
                        description.setHeightOfShadow( std::atoi( shadowHeight->FirstChild()->ToText()->Value() ) );
        }

        // the sequence of frames for an orientation may be either simple 0,1,... or custom
        const tinyxml2::XMLElement* frames = element.FirstChildElement( "frames" ) ;
        if ( frames != nilPointer )
        {
                unsigned int howManyFramesPerOrientation = std::atoi( frames->FirstChild()->ToText()->Value() ) ;
                description.makeSequenceOFrames( howManyFramesPerOrientation ) ;
        }
        else
        {
                // the custom sequence
                std::vector< unsigned int > sequence ;

                for ( const tinyxml2::XMLElement* frame = element.FirstChildElement( "frame" ) ;
                                frame != nilPointer ;
                                frame = frame->NextSiblingElement( "frame" ) )
                {
                        sequence.push_back( std::atoi( frame->FirstChild()->ToText()->Value() ) );
                }

                if ( sequence.size() > 0 )
                        description.setSequenceOFrames( sequence ) ;
        }

        // ... if neither
        if ( description.howManyFramesPerOrientation () == 0 )
                description.makeSequenceOFrames( 1 ) ; // then it's static

        // how many various orientations
        const tinyxml2::XMLElement* orientations = element.FirstChildElement( "orientations" ) ;
        if ( orientations != nilPointer )
                description.setHowManyOrientations( std::atoi( orientations->FirstChild()->ToText()->Value() ) );

        // how many extra frames, such as for jumping or blinking character
        const tinyxml2::XMLElement* extraFrames = element.FirstChildElement( "extraFrames" ) ;
        if ( extraFrames != nilPointer )
                description.setHowManyExtraFrames( std::atoi( extraFrames->FirstChild()->ToText()->Value() ) );
}

const DescriptionOfItem* ItemDescriptions::getDescriptionByKind ( const std::string & kind ) /* const */
{
	// auto-read the item descriptions file if it hasn’t been done before
        if ( ! this->alreadyRead )
                readDescriptionsFromFile( The_File_Full_Of_Item_Descriptions ) ; // yep, it makes the function non-const

        if ( descriptionsOfItems.find( kind ) == descriptionsOfItems.end () )
        {
                std::cout << "the description of the item kind \"" << kind << "\" is absent" << std::endl ;
                return nilPointer ;
        }

#ifdef __Cxx11__

        return descriptionsOfItems.at( kind );

#else

        std::map< std::string, const DescriptionOfItem * >::const_iterator it = descriptionsOfItems.find( kind );
        if ( it != descriptionsOfItems.end () ) return it->second ;
        return nilPointer ;

#endif
}
