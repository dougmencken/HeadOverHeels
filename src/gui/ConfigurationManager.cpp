
#include "ConfigurationManager.hpp"
#include "SoundManager.hpp"
#include "InputManager.hpp"

using gui::ConfigurationManager;
using isomot::SoundManager;
using isomot::InputManager;


ConfigurationManager::ConfigurationManager( const std::string& fileName )
: fileName( fileName )
{

}

void ConfigurationManager::read()
{
        // Carga el archivo XML especificado y almacena los datos XML en los diferentes atributos
        try
        {
                std::auto_ptr< cxml::ConfigurationXML > configurationXML( cxml::configuration( fileName.c_str() ) );

                // Almacena el idioma seleccionado
                this->language = configurationXML->language();

                // Almacena la configuración del teclado
                this->setKey( "movenorth", configurationXML->keyboard().movenorth() );
                this->setKey( "movesouth", configurationXML->keyboard().movesouth() );
                this->setKey( "moveeast", configurationXML->keyboard().moveeast() );
                this->setKey( "movewest", configurationXML->keyboard().movewest() );
                this->setKey( "take", configurationXML->keyboard().take() );
                this->setKey( "jump", configurationXML->keyboard().jump() );
                this->setKey( "shoot", configurationXML->keyboard().shoot() );
                this->setKey( "take-jump", configurationXML->keyboard().takeandjump() );
                this->setKey( "swap", configurationXML->keyboard().swap() );
                this->setKey( "halt", configurationXML->keyboard().halt() );

                // Almacena los niveles de volumen
                this->setVolumeOfSoundEffects( configurationXML->volume().fx() );
                this->setVolumeOfMusic( configurationXML->volume().music() );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl;
        }
}

void ConfigurationManager::write()
{
        try
        {
                cxml::keyboard userKeys (
                        this->getKey( "movenorth" ),
                        this->getKey( "movesouth" ),
                        this->getKey( "moveeast" ),
                        this->getKey( "movewest" ),
                        this->getKey( "take" ),
                        this->getKey( "jump" ),
                        this->getKey( "shoot" ),
                        this->getKey( "take-jump" ),
                        this->getKey( "swap" ),
                        this->getKey( "halt" )
                );

                cxml::volume userVolume( this->getVolumeOfSoundEffects (), this->getVolumeOfMusic () );

                // Creación de la configuración
                cxml::ConfigurationXML configurationXML( this->language, userKeys, userVolume );

                // Información del esquema
                xml_schema::namespace_infomap map;
                map[ "" ].name = "";
                map[ "" ].schema = "configuration.xsd";

                // Creación del archivo
                std::ofstream outputFile( fileName.c_str () );
                cxml::configuration( outputFile, configurationXML, map );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl ;
        }
}

void ConfigurationManager::setKey ( const std::string& nameOfKey, int keyCode )
{
        InputManager::getInstance()->changeUserKey( nameOfKey, keyCode ) ;
}

int ConfigurationManager::getKey ( const std::string& nameOfKey )
{
        return InputManager::getInstance()->getUserKey( nameOfKey ) ;
}

void ConfigurationManager::setVolumeOfSoundEffects ( int volume ) const
{
        SoundManager::getInstance()->setVolumeOfEffects( volume ) ;
}

int ConfigurationManager::getVolumeOfSoundEffects () const
{
        return SoundManager::getInstance()->getVolumeOfEffects ();
}

void ConfigurationManager::setVolumeOfMusic ( int volume ) const
{
        SoundManager::getInstance()->setVolumeOfMusic( volume ) ;
}

int ConfigurationManager::getVolumeOfMusic () const
{
        return SoundManager::getInstance()->getVolumeOfMusic ();
}
