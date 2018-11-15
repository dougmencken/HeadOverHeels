
#include "PoolOfPictures.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"


namespace iso
{

PicturePtr PoolOfPictures::getOrLoadAndGet( const std::string& imageFile )
{
        if ( pictures.find( imageFile ) == pictures.end () )
        {
                std::string gfxPrefix = iso::GameManager::getInstance().getChosenGraphicSet() ;
                allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, imageFile ) );

                if ( picture->isNotNil() )
                {
                        pictures[ imageFile ] = PicturePtr( new Picture( *picture ) );
                        pictures[ imageFile ]->setName( imageFile );
                }
                else
                {
                        std::cerr << "picture \"" << imageFile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;
                        pictures[ imageFile ] = PicturePtr ();
                }

                delete picture ;
        }

        return pictures[ imageFile ] ;
}

PicturePtr PoolOfPictures::getOrLoadAndGetOrMakeAndGet( const std::string& imageFile, unsigned int imageWidth, unsigned int imageHeight )
{
        if ( pictures.find( imageFile ) == pictures.end () )
        {
                std::string gfxPrefix = iso::GameManager::getInstance().getChosenGraphicSet() ;
                allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, imageFile ) );

                if ( picture->isNotNil() )
                {
                        pictures[ imageFile ] = PicturePtr( new Picture( *picture ) );
                        pictures[ imageFile ]->setName( imageFile );
                }
                else
                {
                        std::cout << "picture \"" << imageFile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;

                        pictures[ imageFile ] = PicturePtr( new Picture( imageWidth, imageHeight ) ) ;
                        pictures[ imageFile ]->fillWithTransparencyChequerboard ();
                        pictures[ imageFile ]->setName( "transparency chequerboard for absent " + imageFile );

                        std::cout << "made \"" << pictures[ imageFile ]->getName() << "\"" << std::endl ;
                }

                delete picture ;
        }

        return pictures[ imageFile ] ;
}

}
