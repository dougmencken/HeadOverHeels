
#include "ItemDescriptions.hpp"
#include "GameManager.hpp"
#include "Color.hpp"

#include "ospaths.hpp"

#include <tinyxml2.h>


namespace iso
{

ItemDescriptions::~ItemDescriptions( )
{
        freeDescriptionOfItems() ;
}

void ItemDescriptions::readDescriptionOfItemsFrom( const std::string& nameOfXMLFile )
{
        freeDescriptionOfItems() ;

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
                DescriptionOfItem* newItem = new DescriptionOfItem ();
                newItem->descriptions = this ;

                // label of item
                newItem->label = item->Attribute( "label" ) ;

                bool isDoor = false;
                tinyxml2::XMLElement* door = item->FirstChildElement( "door" ) ;
                if ( door != nilPointer ) isDoor = true;

                // number of frames for orientations of item
                tinyxml2::XMLElement* orientations = item->FirstChildElement( "orientations" ) ;
                newItem->orientations = std::atoi( orientations->FirstChild()->ToText()->Value() ) ;

                // offensive or harmless
                tinyxml2::XMLElement* mortal = item->FirstChildElement( "mortal" ) ;
                if ( mortal != nilPointer )
                        newItem->mortal = ( std::string( mortal->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;
                else
                        newItem->mortal = false ;

                // how long, in milliseconds, it falls
                tinyxml2::XMLElement* weight = item->FirstChildElement( "weight" ) ;
                if ( weight != nilPointer )
                        newItem->weight = static_cast< float >( std::atoi( weight->FirstChild()->ToText()->Value() ) ) / 1000.0 ;
                else
                        newItem->weight = 0.0 ;

                // delay, in milliseconds, between frames in animation sequence
                tinyxml2::XMLElement* framesDelay = item->FirstChildElement( "framesDelay" ) ;
                if ( framesDelay != nilPointer )
                        newItem->delayBetweenFrames = static_cast< float >( std::atoi( framesDelay->FirstChild()->ToText()->Value() ) ) / 1000.0 ;
                else
                        newItem->delayBetweenFrames = 0.0 ;

                // how many milliseconds this item moves one single isometric unit
                tinyxml2::XMLElement* speed = item->FirstChildElement( "speed" ) ;
                if ( speed != nilPointer )
                        newItem->speed = static_cast< float >( std::atoi( speed->FirstChild()->ToText()->Value() ) ) / 1000.0 ;
                else
                        newItem->speed = 0.0 ;

                // sequence of animation
                tinyxml2::XMLElement* frames = item->FirstChildElement( "frames" ) ;
                if ( frames != nilPointer )
                {
                        unsigned int howManyFramesPerOrientation = std::atoi( frames->FirstChild()->ToText()->Value() ) ;

                        for ( unsigned int frame = 0 ; frame < howManyFramesPerOrientation ; frame ++ )
                        {
                                newItem->frames.push_back( frame );
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
                                        newItem->frames.push_back( std::atoi( frame->FirstChild()->ToText()->Value() ) );
                                }
                        }
                        else
                        {
                                newItem->frames.push_back( 0 ) ;
                        }
                }

                tinyxml2::XMLElement* extraFrames = item->FirstChildElement( "extraFrames" ) ;
                // character items have extra frames
                if ( extraFrames != nilPointer )
                {
                        newItem->extraFrames = std::atoi( extraFrames->FirstChild()->ToText()->Value() ) ;
                }

                tinyxml2::XMLElement* picture = item->FirstChildElement( "picture" ) ;
                if ( picture != nilPointer )
                {
                        // name of file with item's graphics
                        newItem->setNameOfFile( picture->Attribute( "file" ) );

                        // width and height, in pixels, of single frame
                        tinyxml2::XMLElement* frameWidth = picture->FirstChildElement( "frameWidth" ) ;
                        newItem->widthOfFrame = std::atoi( frameWidth->FirstChild()->ToText()->Value() ) ;
                        tinyxml2::XMLElement* frameHeight = picture->FirstChildElement( "frameHeight" ) ;
                        newItem->heightOfFrame = std::atoi( frameHeight->FirstChild()->ToText()->Value() ) ;
                }
                else if ( newItem->label == "invisible-wall-x" || newItem->label == "invisible-wall-y" )
                {
                        newItem->setNameOfFile( "" );
                        newItem->widthOfFrame = 64 ;
                        newItem->heightOfFrame = 115 ;
                }

                tinyxml2::XMLElement* shadow = item->FirstChildElement( "shadow" ) ;
                if ( shadow != nilPointer ) // item may have no shadow
                {
                        newItem->setNameOfShadowFile( shadow->Attribute( "file" ) );

                        tinyxml2::XMLElement* shadowWidth = shadow->FirstChildElement( "shadowWidth" ) ;
                        newItem->widthOfShadow = std::atoi( shadowWidth->FirstChild()->ToText()->Value() ) ;
                        tinyxml2::XMLElement* shadowHeight = shadow->FirstChildElement( "shadowHeight" ) ;
                        newItem->heightOfShadow = std::atoi( shadowHeight->FirstChild()->ToText()->Value() ) ;
                }

                // door has three parameters for its dimensions
                if ( isDoor )
                {
                        /* std::string doorAt = door->FirstChild()->ToText()->Value() ; */

                        // three parts of door are lintel, left jamb and right jamb
                        DescriptionOfItem* lintel = DescriptionOfItem::clone( *newItem ) ;
                        lintel->label += "~lintel";
                        lintel->partOfDoor = true;
                        DescriptionOfItem* leftJamb = DescriptionOfItem::clone( *newItem ) ;
                        leftJamb->label += "~leftjamb";
                        leftJamb->partOfDoor = true;
                        DescriptionOfItem* rightJamb = DescriptionOfItem::clone( *newItem ) ;
                        rightJamb->label += "~rightjamb";
                        rightJamb->partOfDoor = true;

                        for ( tinyxml2::XMLElement* widthX = item->FirstChildElement( "widthX" ) ;
                                        widthX != nilPointer ;
                                        widthX = widthX->NextSiblingElement( "widthX" ) )
                        {
                                int wx = std::atoi( widthX->FirstChild()->ToText()->Value() ) ;

                                if ( leftJamb->widthX == 0 )
                                        leftJamb->widthX = wx ;
                                else if ( rightJamb->widthX == 0 )
                                        rightJamb->widthX = wx ;
                                else if ( lintel->widthX == 0 )
                                        lintel->widthX = wx ;
                        }

                        for ( tinyxml2::XMLElement* widthY = item->FirstChildElement( "widthY" ) ;
                                        widthY != nilPointer ;
                                        widthY = widthY->NextSiblingElement( "widthY" ) )
                        {
                                int wy = std::atoi( widthY->FirstChild()->ToText()->Value() ) ;

                                if ( leftJamb->widthY == 0 )
                                        leftJamb->widthY = wy ;
                                else if ( rightJamb->widthY == 0 )
                                        rightJamb->widthY = wy ;
                                else if ( lintel->widthY == 0 )
                                        lintel->widthY = wy ;
                        }

                        for ( tinyxml2::XMLElement* height = item->FirstChildElement( "height" ) ;
                                        height != nilPointer ;
                                        height = height->NextSiblingElement( "height" ) )
                        {
                                int wz = std::atoi( height->FirstChild()->ToText()->Value() ) ;

                                if ( leftJamb->height == 0 )
                                        leftJamb->height = wz ;
                                else if ( rightJamb->height == 0 )
                                        rightJamb->height = wz ;
                                else if ( lintel->height == 0 )
                                        lintel->height = wz ;
                        }

                        descriptionOfItems[ leftJamb->label ] = leftJamb ;
                        descriptionOfItems[ rightJamb->label ] = rightJamb ;
                        descriptionOfItems[ lintel->label ] = lintel ;
                }
                else
                {
                        newItem->widthX = std::atoi( item->FirstChildElement( "widthX" )->FirstChild()->ToText()->Value() ) ;
                        newItem->widthY = std::atoi( item->FirstChildElement( "widthY" )->FirstChild()->ToText()->Value() ) ;
                        newItem->height = std::atoi( item->FirstChildElement( "height" )->FirstChild()->ToText()->Value() ) ;

                        descriptionOfItems[ newItem->label ] = newItem ;
                }
        }
}

void ItemDescriptions::freeDescriptionOfItems ()
{
        for ( std::map< std::string, const DescriptionOfItem * >::iterator i = descriptionOfItems.begin (); i != descriptionOfItems.end (); ++ i )
        {
                delete i->second ;
        }
        descriptionOfItems.clear();
}

const DescriptionOfItem* ItemDescriptions::getDescriptionByLabel( const std::string& label ) const
{
        if ( descriptionOfItems.find( label ) == descriptionOfItems.end () )
        {
                std::cerr << "description for item with label \"" << label << "\" is absent" << std::endl ;
                return nilPointer ;
        }

#ifdef __Cxx11__

        return descriptionOfItems.at( label );

#else

        std::map< std::string, const DescriptionOfItem * >::const_iterator it = descriptionOfItems.find( label );
        if ( it != descriptionOfItems.end () ) return it->second ;
        return nilPointer ;

#endif
}

}
