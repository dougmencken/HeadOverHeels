
#include "ConfigurationManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"

using gui::ConfigurationManager;
using gui::GuiManager;
using isomot::GameManager;
using isomot::SoundManager;
using isomot::InputManager;


ConfigurationManager::ConfigurationManager( const std::string& fileName )
: fileName( fileName )
{

}

bool ConfigurationManager::read ()
{
        try
        {
                std::auto_ptr< cxml::ConfigurationXML > configurationXML( cxml::preferences( fileName.c_str() ) );

                // chosen language

                GuiManager::getInstance()->setLanguage( configurationXML->language() );

                // keys of keyboard

                InputManager::getInstance()->changeUserKey( "movenorth", configurationXML->keyboard().movenorth() );
                InputManager::getInstance()->changeUserKey( "movesouth", configurationXML->keyboard().movesouth() );
                InputManager::getInstance()->changeUserKey( "moveeast", configurationXML->keyboard().moveeast() );
                InputManager::getInstance()->changeUserKey( "movewest", configurationXML->keyboard().movewest() );
                InputManager::getInstance()->changeUserKey( "take", configurationXML->keyboard().take() );
                InputManager::getInstance()->changeUserKey( "jump", configurationXML->keyboard().jump() );
                InputManager::getInstance()->changeUserKey( "doughnut", configurationXML->keyboard().doughnut() );
                InputManager::getInstance()->changeUserKey( "take&jump", configurationXML->keyboard().takeandjump() );
                InputManager::getInstance()->changeUserKey( "swap", configurationXML->keyboard().swap() );
                InputManager::getInstance()->changeUserKey( "pause", configurationXML->keyboard().pause() );

                // preferences of audio

                SoundManager::getInstance()->setVolumeOfEffects( configurationXML->audio().fx() ) ;
                SoundManager::getInstance()->setVolumeOfMusic( configurationXML->audio().music() ) ;

                if ( GameManager::getInstance()->playMelodyOfScenery () != configurationXML->audio().roomtunes() )
                {
                        GameManager::getInstance()->togglePlayMelodyOfScenery ();
                }

                // preferences of video

                if ( GuiManager::getInstance()->isAtFullScreen () != configurationXML->video().fullscreen() )
                {
                        GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                if ( GameManager::getInstance()->getDrawShadows () != configurationXML->video().shadows() )
                {
                        GameManager::getInstance()->toggleDrawShadows ();
                }

                if ( GameManager::getInstance()->hasBackgroundPicture () != configurationXML->video().background() )
                {
                        GameManager::getInstance()->toggleBackgroundPicture ();
                }

                GameManager::getInstance()->setChosenGraphicSet( configurationXML->video().graphics().c_str () ) ;

                return true;
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl;
                return false;
        }
}

void ConfigurationManager::write()
{
        try
        {
                cxml::keyboard userKeys (
                        InputManager::getInstance()->getUserKey( "movenorth" ),
                        InputManager::getInstance()->getUserKey( "movesouth" ),
                        InputManager::getInstance()->getUserKey( "moveeast" ),
                        InputManager::getInstance()->getUserKey( "movewest" ),
                        InputManager::getInstance()->getUserKey( "take" ),
                        InputManager::getInstance()->getUserKey( "jump" ),
                        InputManager::getInstance()->getUserKey( "doughnut" ),
                        InputManager::getInstance()->getUserKey( "take&jump" ),
                        InputManager::getInstance()->getUserKey( "swap" ),
                        InputManager::getInstance()->getUserKey( "pause" )
                );

                cxml::audio audioPreferences (
                        SoundManager::getInstance()->getVolumeOfEffects (),
                        SoundManager::getInstance()->getVolumeOfMusic (),
                        GameManager::getInstance()->playMelodyOfScenery ()
                );

                cxml::video videoPreferences (
                        GuiManager::getInstance()->isAtFullScreen (),
                        GameManager::getInstance()->getDrawShadows (),
                        GameManager::getInstance()->hasBackgroundPicture (),
                        GameManager::getInstance()->getChosenGraphicSet()
                );

                cxml::ConfigurationXML configurationXML (
                        GuiManager::getInstance()->getLanguage(),
                        userKeys,
                        audioPreferences,
                        videoPreferences
                );

                xml_schema::namespace_infomap map;
                map[ "" ].name = "";
                map[ "" ].schema = "configuration.xsd";

                std::ofstream outputFile( fileName.c_str () );
                cxml::preferences( outputFile, configurationXML, map );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl ;
        }
}
