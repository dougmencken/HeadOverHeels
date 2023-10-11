
#include "GamePreferences.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"

#include "screen.hpp"

#include <tinyxml2.h>

using game::GamePreferences ;

using game::GameManager ;


/* static */
bool GamePreferences::readPreferences( const std::string & fileName )
{
        std::cout << "readPreferences( " << fileName << " )" << std::endl ;

        // read list of sounds from XML file
        tinyxml2::XMLDocument preferences;
        tinyxml2::XMLError result = preferences.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read game’s preferences from \"" << fileName << "\"" << std::endl ;
                return false;
        }

        std::cout << "read game’s preferences" << std::endl ;

        tinyxml2::XMLElement* root = preferences.FirstChildElement( "preferences" );

        // chosen language

        tinyxml2::XMLElement* language = root->FirstChildElement( "language" ) ;
        std::string languageString = "en_US";
        if ( language != nilPointer )
        {
                languageString = language->FirstChild()->ToText()->Value();
        }
        gui::GuiManager::getInstance().setLanguage( languageString );

        // chosen keys

        tinyxml2::XMLElement* keyboard = root->FirstChildElement( "keyboard" ) ;
        if ( keyboard != nilPointer )
        {
                const std::vector < std::pair < /* action */ std::string, /* name */ std::string > > & chosenKeys = InputManager::getInstance().getUserKeys ();
                for ( std::vector< std::pair< std::string, std::string > >::const_iterator it = chosenKeys.begin () ; it != chosenKeys.end () ; ++ it )
                {
                        std::string xmlAction = ( it->first == "take&jump" ) ? "takeandjump" : it->first ;
                        tinyxml2::XMLElement* elementForKey = keyboard->FirstChildElement( xmlAction.c_str () ) ;

                        if ( elementForKey != nilPointer && elementForKey->FirstChild() != nilPointer )
                                InputManager::getInstance().changeUserKey( it->first, elementForKey->FirstChild()->ToText()->Value() );
                }
        }

        // preferences for audio

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

        // preferences for video

        tinyxml2::XMLElement* video = root->FirstChildElement( "video" ) ;
        if ( video != nilPointer )
        {
                tinyxml2::XMLElement* width = video->FirstChildElement( "width" ) ;
                if ( width != nilPointer )
                {
                        variables::setScreenWidth( std::atoi( width->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* height = video->FirstChildElement( "height" ) ;
                if ( height != nilPointer )
                {
                        variables::setScreenHeight( std::atoi( height->FirstChild()->ToText()->Value() ) ) ;
                }

                bool atFullScreen = false ;
                tinyxml2::XMLElement* fullscreen = video->FirstChildElement( "fullscreen" ) ;
                if ( fullscreen != nilPointer )
                {
                        atFullScreen = ( std::string( fullscreen->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;
                }

                allegroWindowSizeToScreenSize ();

                if ( gui::GuiManager::getInstance().isAtFullScreen () != atFullScreen ) {
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();
                }

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
        std::cout << "writePreferences( " << fileName << " )" << std::endl ;

        tinyxml2::XMLDocument preferences ;

        tinyxml2::XMLNode * root = preferences.NewElement( "preferences" );
        preferences.InsertFirstChild( root );

        // language

        tinyxml2::XMLElement * language = preferences.NewElement( "language" );
        language->SetText( gui::GuiManager::getInstance().getLanguage().c_str () );
        root->InsertEndChild( language );

        // keys
        {
                tinyxml2::XMLNode * keyboard = preferences.NewElement( "keyboard" );

                const std::vector < std::pair < /* action */ std::string, /* name */ std::string > > & chosenKeys = InputManager::getInstance().getUserKeys ();
                for ( std::vector< std::pair< std::string, std::string > >::const_iterator it = chosenKeys.begin () ; it != chosenKeys.end () ; ++ it )
                {
                        std::string xmlAction = ( it->first == "take&jump" ) ? "takeandjump" : it->first ;
                        tinyxml2::XMLElement* elementForKey = preferences.NewElement( xmlAction.c_str () ) ;
                        elementForKey->SetText( InputManager::getInstance().getUserKeyFor( it->first ).c_str () );
                        keyboard->InsertEndChild( elementForKey );
                }

                root->InsertEndChild( keyboard );
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
                width->SetText( variables::getScreenWidth () );
                video->InsertEndChild( width );

                tinyxml2::XMLElement * height = preferences.NewElement( "height" );
                height->SetText( variables::getScreenHeight () );
                video->InsertEndChild( height );

                tinyxml2::XMLElement * fullscreen = preferences.NewElement( "fullscreen" );
                fullscreen->SetText( gui::GuiManager::getInstance().isAtFullScreen () ? "true" : "false" );
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
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t write game’s preferences to \"" << fileName << "\"" << std::endl ;
                return false;
        }

        std::cout << "wrote game’s preferences" << std::endl ;
        return true;
}
