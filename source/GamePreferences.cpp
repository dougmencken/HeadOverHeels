
#include "GamePreferences.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"

#include "WrappersAllegro.hpp"

#include <tinyxml2.h>


/* static */ unsigned int GamePreferences::screenWidth = GamePreferences::Default_Screen_Width ;
/* static */ unsigned int GamePreferences::screenHeight = GamePreferences::Default_Screen_Height ;

/* static */ bool GamePreferences::keepWidth = false ;
/* static */ bool GamePreferences::keepHeight = false ;

/* static */
bool GamePreferences::allegroWindowSizeToScreenSize ()
{
        bool switched = allegro::switchToWindowedVideo( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) ;
        if ( ! switched ) {
                std::cout << "can’t change the size of game's window to "
                                << GamePreferences::getScreenWidth() << " x " << GamePreferences::getScreenHeight() << std::endl ;
        }

        allegro::Pict::theScreen().clearToColor( AllegroColor::makeColor( 0, 0, 0, 0xff ) );
        allegro::update ();

        return switched ;
}

/* static */
bool GamePreferences::readPreferences( const std::string & fileName )
{
        std::cout << "readPreferences( \"" << fileName << "\" )" << std::endl ;

        tinyxml2::XMLDocument preferences ;
        tinyxml2::XMLError result = preferences.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "no previously stored preferences in \"" << fileName << "\"" << std::endl ;
                return false ;
        }

        std::cout << "reading the game’s preferences" << std::endl ;

        tinyxml2::XMLElement* root = preferences.FirstChildElement( "preferences" );

        // the chosen language

        tinyxml2::XMLElement* language = root->FirstChildElement( "language" ) ;
        if ( language != nilPointer && language->FirstChild() != nilPointer )
                gui::GuiManager::getInstance().setLanguage( language->FirstChild()->ToText()->Value() );

        // the chosen keys

        tinyxml2::XMLElement* keys = root->FirstChildElement( "keys" ) ;
        if ( keys != nilPointer )
        {
                std::vector< std::string > userActions ;
                InputManager::getInstance().getAllActions( userActions );
                for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                {
                        const std::string & action = userActions[ i ];
                        std::string xmlAction = ( action == "take&jump" ) ? "takeandjump" : action ;
                        tinyxml2::XMLElement* elementForKey = keys->FirstChildElement( xmlAction.c_str () ) ;

                        if ( elementForKey != nilPointer && elementForKey->FirstChild() != nilPointer )
                                InputManager::getInstance().changeUserKey( action, elementForKey->FirstChild()->ToText()->Value() );
                }
        }

        // the preferences for audio

        tinyxml2::XMLElement* audio = root->FirstChildElement( "audio" ) ;
        if ( audio != nilPointer )
        {
                tinyxml2::XMLElement* fx = audio->FirstChildElement( "fx" ) ;
                if ( fx != nilPointer )
                {
                        SoundManager::getInstance().setVolumeOfEffects( std::atoi( fx->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* music = audio->FirstChildElement( "music" ) ;
                if ( music != nilPointer )
                {
                        SoundManager::getInstance().setVolumeOfMusic( std::atoi( music->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* roomtunes = audio->FirstChildElement( "roomtunes" ) ;
                if ( roomtunes != nilPointer )
                {
                        bool playRoomTunes = ( std::string( roomtunes->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().playMelodyOfScenery () != playRoomTunes )
                                GameManager::getInstance().togglePlayMelodyOfScenery ();
                }
        }

        // the preferences for video

        tinyxml2::XMLElement* video = root->FirstChildElement( "video" ) ;
        if ( video != nilPointer )
        {
                tinyxml2::XMLElement* width = video->FirstChildElement( "width" ) ;
                if ( width != nilPointer )
                {
                        if ( GamePreferences::isWidthKept () )
                                GamePreferences::keepThisWidth( false );
                        else
                                GamePreferences::setScreenWidth( std::atoi( width->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* height = video->FirstChildElement( "height" ) ;
                if ( height != nilPointer )
                {
                        if ( GamePreferences::isHeightKept () )
                                GamePreferences::keepThisHeight( false );
                        else
                                GamePreferences::setScreenHeight( std::atoi( height->FirstChild()->ToText()->Value() ) ) ;
                }

                bool inFullScreen = false ;
                tinyxml2::XMLElement* fullscreen = video->FirstChildElement( "fullscreen" ) ;
                if ( fullscreen != nilPointer )
                        inFullScreen = ( std::string( fullscreen->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                allegroWindowSizeToScreenSize ();

                if ( gui::GuiManager::getInstance().isInFullScreen () != inFullScreen )
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();

                tinyxml2::XMLElement* shadows = video->FirstChildElement( "drawshadows" ) ;
                if ( shadows != nilPointer )
                {
                        bool castShadows = ( std::string( shadows->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().getCastShadows () != castShadows )
                                GameManager::getInstance().toggleCastShadows ();
                }

                tinyxml2::XMLElement* sceneryDecor = video->FirstChildElement( "drawdecor" ) ;
                if ( sceneryDecor != nilPointer )
                {
                        bool drawDecor = ( std::string( sceneryDecor->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().drawSceneryDecor () != drawDecor )
                                GameManager::getInstance().toggleSceneryDecor ();
                }

                tinyxml2::XMLElement* roomMiniatures = video->FirstChildElement( "drawminiatures" ) ;
                if ( roomMiniatures != nilPointer )
                {
                        bool drawMiniatures = ( std::string( roomMiniatures->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().drawRoomMiniatures () != drawMiniatures )
                                GameManager::getInstance().toggleRoomMiniatures ();
                }

                tinyxml2::XMLElement* centerCameraOn = video->FirstChildElement( "centercamera" ) ;
                if ( centerCameraOn != nilPointer )
                {
                        bool centerCameraOnCharacter =
                                ( std::string( centerCameraOn->FirstChild()->ToText()->Value() ) == "character" ) ? true : false ;

                        if ( GameManager::getInstance().getIsomot().doesCameraFollowCharacter () != centerCameraOnCharacter )
                                GameManager::getInstance().getIsomot().toggleCameraFollowsCharacter ();
                }

                tinyxml2::XMLElement* graphics = video->FirstChildElement( "graphics" ) ;
                if ( graphics != nilPointer )
                {
                        GameManager::getInstance().setChosenGraphicsSet( graphics->FirstChild()->ToText()->Value() ) ;
                }
        }

        return true;
}

/* static */
bool GamePreferences::writePreferences( const std::string & fileName )
{
        std::cout << "writePreferences( \"" << fileName << "\" )" << std::endl ;

        tinyxml2::XMLDocument preferences ;

        tinyxml2::XMLNode * root = preferences.NewElement( "preferences" );
        preferences.InsertFirstChild( root );

        // language

        tinyxml2::XMLElement * language = preferences.NewElement( "language" );
        language->SetText( gui::GuiManager::getInstance().getLanguage().c_str () );
        root->InsertEndChild( language );

        // keys
        {
                tinyxml2::XMLNode * keys = preferences.NewElement( "keys" );

                std::vector< std::string > userActions ;
                InputManager::getInstance().getAllActions( userActions );
                for ( unsigned int i = 0 ; i < userActions.size () ; ++ i )
                {
                        const std::string & action = userActions[ i ];
                        std::string xmlAction = ( action == "take&jump" ) ? "takeandjump" : action ;
                        tinyxml2::XMLElement* elementForKey = preferences.NewElement( xmlAction.c_str () ) ;
                        elementForKey->SetText( InputManager::getInstance().getUserKeyFor( action ).c_str () );
                        keys->InsertEndChild( elementForKey );
                }

                root->InsertEndChild( keys );
        }

        // audio
        {
                tinyxml2::XMLNode * audio = preferences.NewElement( "audio" );

                tinyxml2::XMLElement * fx = preferences.NewElement( "fx" );
                fx->SetText( SoundManager::getInstance().getVolumeOfEffects () );
                audio->InsertEndChild( fx );

                tinyxml2::XMLElement * music = preferences.NewElement( "music" );
                music->SetText( SoundManager::getInstance().getVolumeOfMusic () );
                audio->InsertEndChild( music );

                tinyxml2::XMLElement * roomtunes = preferences.NewElement( "roomtunes" );
                roomtunes->SetText( GameManager::getInstance().playMelodyOfScenery () ? "true" : "false" );
                audio->InsertEndChild( roomtunes );

                root->InsertEndChild( audio );
        }

        // video
        {
                tinyxml2::XMLNode * video = preferences.NewElement( "video" );

                tinyxml2::XMLElement * width = preferences.NewElement( "width" );
                width->SetText( GamePreferences::getScreenWidth () );
                video->InsertEndChild( width );

                tinyxml2::XMLElement * height = preferences.NewElement( "height" );
                height->SetText( GamePreferences::getScreenHeight () );
                video->InsertEndChild( height );

                tinyxml2::XMLElement * fullscreen = preferences.NewElement( "fullscreen" );
                fullscreen->SetText( gui::GuiManager::getInstance().isInFullScreen () ? "true" : "false" );
                video->InsertEndChild( fullscreen );

                tinyxml2::XMLElement * shadows = preferences.NewElement( "drawshadows" );
                shadows->SetText( GameManager::getInstance().getCastShadows () ? "true" : "false" );
                video->InsertEndChild( shadows );

                tinyxml2::XMLElement * drawdecor = preferences.NewElement( "drawdecor" );
                drawdecor->SetText( GameManager::getInstance().drawSceneryDecor () ? "true" : "false" );
                video->InsertEndChild( drawdecor );

                tinyxml2::XMLElement * drawMiniatures = preferences.NewElement( "drawminiatures" );
                drawMiniatures->SetText( GameManager::getInstance().drawRoomMiniatures () ? "true" : "false" );
                video->InsertEndChild( drawMiniatures );

                tinyxml2::XMLElement * centerCameraOn = preferences.NewElement( "centercamera" );
                centerCameraOn->SetText( GameManager::getInstance().getIsomot().doesCameraFollowCharacter () ? "character" : "room" );
                video->InsertEndChild( centerCameraOn );

                tinyxml2::XMLElement * graphics = preferences.NewElement( "graphics" );
                graphics->SetText( GameManager::getInstance().getChosenGraphicsSet().c_str () );
                video->InsertEndChild( graphics );

                root->InsertEndChild( video );
        }

        tinyxml2::XMLError result = preferences.SaveFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t write the game’s preferences to \"" << fileName << "\"" << std::endl ;
                return false;
        }

        std::cout << "wrote the game’s preferences" << std::endl ;
        return true;
}
