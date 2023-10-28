
#include "PoolOfPictures.hpp"
#include "GameManager.hpp"

#include "ospaths.hpp"

#ifdef DEBUG
# define DEBUG_POOL_OF_PICTURES  0
# include <iostream>
#endif


PicturePtr PoolOfPictures::getPicture( const std::string& imageFile ) const
{
        std::string gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        std::map< std::string, PicturePtr >::const_iterator pi = pictures.find( key ) ;
        return ( pi != pictures.end () ) ? ( *pi ).second : PicturePtr () ;
}

PicturePtr PoolOfPictures::getOrLoadAndGet( const std::string& imageFile )
{
        std::string gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () )
        {
                multiptr< allegro::Pict > picture( allegro::Pict::fromPNGFile( ospaths::pathToFile( ospaths::sharePath() + gfxPrefix, imageFile ) ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = PicturePtr( new Picture( *picture ) );
                        pictures[ key ]->setName( imageFile );
                }
                else
                {
                #if defined( DEBUG_POOL_OF_PICTURES ) && DEBUG_POOL_OF_PICTURES
                        std::cout << CONSOLE_COLOR_RED << " picture " << CONSOLE_COLOR_BOLD_RED << key << CONSOLE_COLOR_RED << " is absent" << CONSOLE_COLOR_OFF << std::endl ;
                #endif
                        pictures[ key ] = PicturePtr ();
                }
        }

        return pictures[ key ] ;
}

PicturePtr PoolOfPictures::getOrLoadAndGetOrMakeAndGet( const std::string& imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        std::string gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () || pictures[ key ] == nilPointer )
        {
                multiptr< allegro::Pict > picture( allegro::Pict::fromPNGFile( ospaths::pathToFile( ospaths::sharePath() + gfxPrefix, imageFile ) ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = PicturePtr( new Picture( *picture ) );
                        pictures[ key ]->setName( imageFile );
                }
                else
                {
                #if defined( DEBUG_POOL_OF_PICTURES ) && DEBUG_POOL_OF_PICTURES
                        std::cout << CONSOLE_COLOR_BLUE << " picture " << CONSOLE_COLOR_BOLD_BLUE << key << CONSOLE_COLOR_BLUE << " is absent" << CONSOLE_COLOR_OFF << std::endl ;
                #endif
                        makePicture( imageFile, imageWidth, imageHeight );
                }
        }

        return pictures[ key ] ;
}

PicturePtr PoolOfPictures::makePicture( const std::string& imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        std::string gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        pictures[ key ] = PicturePtr( new Picture( imageWidth, imageHeight ) ) ;
        pictures[ key ]->fillWithTransparencyChequerboard ();
        pictures[ key ]->setName( "transparency chequerboard for absent " + key );

#if defined( DEBUG_POOL_OF_PICTURES ) && DEBUG_POOL_OF_PICTURES
        std::cout << CONSOLE_COLOR_GREEN << " made " << CONSOLE_COLOR_BOLD_GREEN << pictures[ key ]->getName() << CONSOLE_COLOR_OFF << std::endl ;
#endif

        return pictures[ key ] ;
}

void PoolOfPictures::putPicture( const std::string& imageFile, const PicturePtr& picture )
{
        std::string gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        pictures[ key ] = picture ;
        pictures[ key ]->setName( imageFile );
}
