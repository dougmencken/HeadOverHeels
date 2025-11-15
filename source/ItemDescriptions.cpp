
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

                tinyxml2::XMLElement* xWidthElement = item->FirstChildElement( "width-x" ) ;
                tinyxml2::XMLElement* yWidthElement = item->FirstChildElement( "width-y" ) ;
                tinyxml2::XMLElement* heightElement = item->FirstChildElement( "height" ) ;

                if ( xWidthElement != nilPointer )
                        newDescription->setWidthX( std::atoi( xWidthElement->FirstChild()->ToText()->Value() ) );
                if ( yWidthElement != nilPointer )
                        newDescription->setWidthY( std::atoi( yWidthElement->FirstChild()->ToText()->Value() ) );
                if ( heightElement != nilPointer )
                        newDescription->setHeight( std::atoi( heightElement->FirstChild()->ToText()->Value() ) );

                readDescriptionFurther( *item, *newDescription );

                // and at last
                descriptionsOfItems[ kindOfItem ] = newDescription ;
        }

        // now make the descriptions of doors

        std::string where[ 4 ] ;        // = { "north", "east", "south", "west" } ;
        where[ 0 ] = "north" ;
        where[ 1 ] = "east" ;
        where[ 2 ] = "south" ;
        where[ 3 ] = "west" ;

        std::string sceneries[ 8 ] ;    // = { "jail", "blacktooth", "market", "moon",
                                        //     "byblos", "egyptus", "penitentiary", "safari" } ;
        sceneries[ 0 ] = "jail" ;
        sceneries[ 1 ] = "blacktooth" ;
        sceneries[ 2 ] = "market" ;
        sceneries[ 3 ] = "moon" ;
        sceneries[ 4 ] = "byblos" ;
        sceneries[ 5 ] = "egyptus" ;
        sceneries[ 6 ] = "penitentiary" ;
        sceneries[ 7 ] = "safari" ;

        for ( unsigned int s = 0 ; s < 8 ; ++ s ) {
                for ( unsigned int w = 0 ; w < 4 ; ++ w )
                {
                        DescriptionOfDoor * doorDescription = new DescriptionOfDoor( sceneries[ s ], where[ w ] );
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

        const tinyxml2::XMLElement* picture = element.FirstChildElement( "graphics" ) ;
        if ( picture != nilPointer )
        {
                // the name of file with graphics for this item
                description.setNameOfFramesFile( picture->Attribute( "file" ) );

                // the width and height, in pixels, of a single frame
                const tinyxml2::XMLElement* frameWidth = picture->FirstChildElement( "frame-width" ) ;
                if ( frameWidth != nilPointer )
                        description.setWidthOfFrame( std::atoi( frameWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* frameHeight = picture->FirstChildElement( "frame-height" ) ;
                if ( frameHeight != nilPointer )
                        description.setHeightOfFrame( std::atoi( frameHeight->FirstChild()->ToText()->Value() ) );
        }
        else
        {
                const std::string & kindOfItem = description.getKind ();
                if ( kindOfItem == "invisible-wall-x" || kindOfItem == "invisible-wall-y" )
                {
                        description.setNameOfFramesFile( "" );
                        description.setWidthOfFrame( 64 );
                        description.setHeightOfFrame( 115 );
                }
        }

        // delay, in seconds, between frames in the animation sequence
        double delayBetweenFrames = 0.0 ;

        const tinyxml2::XMLElement* betweenFrames = element.FirstChildElement( "delay-between-frames" ) ;
        if ( betweenFrames != nilPointer )
                delayBetweenFrames = static_cast< double >( std::atoi( betweenFrames->FirstChild()->ToText()->Value() ) ) / 1000.0 ;

        description.setDelayBetweenFrames( delayBetweenFrames );

        // shadows for this item

        const tinyxml2::XMLElement* shadow = element.FirstChildElement( "shadows" ) ;
        if ( shadow != nilPointer ) // item may have no shadow
        {
                description.setNameOfShadowsFile( shadow->Attribute( "file" ) );

                const tinyxml2::XMLElement* shadowWidth = shadow->FirstChildElement( "width-of-shadow" ) ;
                if ( shadowWidth != nilPointer )
                        description.setWidthOfShadow( std::atoi( shadowWidth->FirstChild()->ToText()->Value() ) );

                const tinyxml2::XMLElement* shadowHeight = shadow->FirstChildElement( "height-of-shadow" ) ;
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
        const tinyxml2::XMLElement* extraFrames = element.FirstChildElement( "extra-frames" ) ;
        if ( extraFrames != nilPointer )
                description.setHowManyExtraFrames( std::atoi( extraFrames->FirstChild()->ToText()->Value() ) );
}

const DescriptionOfItem* ItemDescriptions::getDescriptionByKind ( const std::string & kind ) /* const */
{
	// auto-read the item descriptions file if it hasn’t been done before
        if ( ! this->alreadyRead )
                readDescriptions() ; // yep, it makes the function non-const

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
