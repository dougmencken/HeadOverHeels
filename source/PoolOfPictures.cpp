
#include "PoolOfPictures.hpp"
#include "GameManager.hpp"

#include "ospaths.hpp"

#ifdef DEBUG
# define DEBUG_POOL_OF_PICTURES  0
# include <iostream>
#endif


PoolOfPictures * PoolOfPictures::thePoolOfPictures = new PoolOfPictures( ) ;


/* static */
bool PoolOfPictures::isPictureThere ( const std::string & imageFile )
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string path = ospaths::pathToFile( ospaths::sharePath() + gfxPrefix, imageFile );
        autouniqueptr< allegro::Pict > picture( allegro::Pict::fromPNGFile( path ) );
        return picture->isNotNil() ;
}

bool PoolOfPictures::hasPicture ( const std::string & imageFile ) const
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        return ( pictures.find( key ) != pictures.end () );
}

NamedPicturePtr PoolOfPictures::getPicture( const std::string & imageFile ) const
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        std::map< std::string, NamedPicturePtr >::const_iterator pi = pictures.find( key ) ;
        return ( pi != pictures.end () ) ? ( *pi ).second : NamedPicturePtr () ;
}

NamedPicturePtr PoolOfPictures::getOrLoadAndGet( const std::string & imageFile )
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () )
        {
                multiptr< allegro::Pict > picture( allegro::Pict::fromPNGFile( ospaths::pathToFile( ospaths::sharePath() + gfxPrefix, imageFile ) ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = NamedPicturePtr( new NamedPicture( *picture ) );
                        pictures[ key ]->setName( imageFile );
                }
                else
                {
                #if defined( DEBUG_POOL_OF_PICTURES ) && DEBUG_POOL_OF_PICTURES
                        std::cout << CONSOLE_COLOR_RED << " picture " << CONSOLE_COLOR_BOLD_RED << key << CONSOLE_COLOR_RED << " is absent" << CONSOLE_COLOR_OFF << std::endl ;
                #endif
                        pictures[ key ] = NamedPicturePtr ();
                }
        }

        return pictures[ key ] ;
}

NamedPicturePtr PoolOfPictures::getOrLoadAndGetOrMakeAndGet( const std::string & imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () || pictures[ key ] == nilPointer )
        {
                multiptr< allegro::Pict > picture( allegro::Pict::fromPNGFile( ospaths::pathToFile( ospaths::sharePath() + gfxPrefix, imageFile ) ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = NamedPicturePtr( new NamedPicture( *picture ) );
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

NamedPicturePtr PoolOfPictures::makePicture( const std::string & imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        pictures[ key ] = NamedPicturePtr( new NamedPicture( imageWidth, imageHeight ) ) ;
        pictures[ key ]->fillWithTransparencyChequerboard ();
        pictures[ key ]->setName( "transparency chequerboard for absent " + key );

#if defined( DEBUG_POOL_OF_PICTURES ) && DEBUG_POOL_OF_PICTURES
        std::cout << CONSOLE_COLOR_GREEN << " made " << CONSOLE_COLOR_BOLD_GREEN << pictures[ key ]->getName() << CONSOLE_COLOR_OFF << std::endl ;
#endif

        return pictures[ key ] ;
}

void PoolOfPictures::putPicture( const std::string & imageFile, const NamedPicturePtr & picture )
{
        const std::string & gfxPrefix = GameManager::getInstance().getChosenGraphicsSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        pictures[ key ] = picture ;
        pictures[ key ]->setName( imageFile );
}
