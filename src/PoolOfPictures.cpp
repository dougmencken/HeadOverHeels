
#include "PoolOfPictures.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"


namespace iso
{

PicturePtr PoolOfPictures::getOrLoadAndGet( const std::string& imageFile )
{
        std::string gfxPrefix = iso::GameManager::getInstance().getChosenGraphicSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () )
        {
                allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, imageFile ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = PicturePtr( new Picture( *picture ) );
                        pictures[ key ]->setName( imageFile );
                }
                else
                {
                        std::cerr << "picture \"" << imageFile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;
                        pictures[ key ] = PicturePtr ();
                }

                delete picture ;
        }

        return pictures[ key ] ;
}

PicturePtr PoolOfPictures::getOrLoadAndGetOrMakeAndGet( const std::string& imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        std::string gfxPrefix = iso::GameManager::getInstance().getChosenGraphicSet() ;
        std::string key = gfxPrefix + ":" + imageFile ;

        if ( pictures.find( key ) == pictures.end () )
        {
                allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, imageFile ) );

                if ( picture->isNotNil() )
                {
                        pictures[ key ] = PicturePtr( new Picture( *picture ) );
                        pictures[ key ]->setName( imageFile );
                }
                else
                {
                        std::cout << "picture \"" << imageFile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;

                        pictures[ key ] = PicturePtr( new Picture( imageWidth, imageHeight ) ) ;
                        pictures[ key ]->fillWithTransparencyChequerboard ();
                        pictures[ key ]->setName( "transparency chequerboard for absent " + imageFile );

                        std::cout << "made \"" << pictures[ key ]->getName() << "\"" << std::endl ;
                }

                delete picture ;
        }

        return pictures[ key ] ;
}

}
