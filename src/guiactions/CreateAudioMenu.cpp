
#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

#include <string>
#include <sstream>

using gui::CreateAudioMenu;
using isomot::SoundManager;


CreateAudioMenu::CreateAudioMenu( BITMAP* picture ) :
        Action( picture ),
        listOfOptions ( nilPointer ),
        labelEffects ( nilPointer ),
        labelMusic ( nilPointer ),
        playRoomTunes ( nilPointer )
{

}

void CreateAudioMenu::doAction ()
{
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        LanguageText* langStringEffects = languageManager->findLanguageStringForAlias( "soundfx" );
        LanguageText* langStringMusic = languageManager->findLanguageStringForAlias( "music" );

        LanguageText* langStringRoomTunes = languageManager->findLanguageStringForAlias( "play-room-melodies" );

        std::string yeah = languageManager->findLanguageStringForAlias( "yep" )-> getText ();
        std::string nope = languageManager->findLanguageStringForAlias( "nope" )->getText ();

        std::stringstream ss;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( getWhereToDraw() ) );

                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                // efectos sonoros
                this->labelEffects = new Label( langStringEffects->getText() );

                // música
                this->labelMusic = new Label( langStringMusic->getText() );

                // play melody of scenery on entry to room or don’t
                this->playRoomTunes = new Label( langStringRoomTunes->getText() );

                // create menu

                this->listOfOptions = new MenuWithValues( );

                listOfOptions->addOption( labelEffects );
                ss.str( std::string() ); // clear ss
                ss << SoundManager::getInstance()->getVolumeOfEffects();
                listOfOptions->setValueOf( labelEffects, ss.str() );

                listOfOptions->addOption( labelMusic );
                ss.str( std::string() );
                ss << SoundManager::getInstance()->getVolumeOfMusic();
                listOfOptions->setValueOf( labelMusic, ss.str() );

                listOfOptions->addOption( playRoomTunes );
                listOfOptions->setValueOf( playRoomTunes, isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope );

                listOfOptions->setVerticalOffset( 35 );

                // add menu to screen

                screen->addWidget( listOfOptions );
        }
        else
        {
                // update labels
                listOfOptions->setValueOf( playRoomTunes, isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope );
        }

        if ( screen->getKeyHandler() == nilPointer )
        {
                screen->setKeyHandler( listOfOptions );
        }

        GuiManager::getInstance()->changeScreen( screen, true );

        clear_keybuf();

        while ( true )
        {
                if ( keypressed() )
                {
                        // get the key pressed by user
                        int theKey = readkey() >> 8;

                        if ( theKey == KEY_ESC )
                        {
                                clear_keybuf();
                                screen->handleKey ( theKey << 8 );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( listOfOptions->getActiveOption () == labelMusic || listOfOptions->getActiveOption () == labelEffects )
                                {
                                        int musicVolume = SoundManager::getInstance()->getVolumeOfMusic();
                                        int effectsVolume = SoundManager::getInstance()->getVolumeOfEffects();
                                        int value = ( listOfOptions->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;
                                        int previousValue = value;

                                        if ( theKey == KEY_LEFT )
                                        {
                                                value = ( value > 0 ? value - 1 : 0 ); // decrease volume
                                                doneWithKey = true;
                                        }
                                        else if ( theKey == KEY_RIGHT )
                                        {
                                                value = ( value < 99 ? value + 1 : 99 ); // increase volume
                                                doneWithKey = true;
                                        }

                                        if ( value != previousValue )
                                        {
                                                ss.str( std::string() ); // clear ss
                                                ss << value;

                                                if ( listOfOptions->getActiveOption () == labelMusic )
                                                {
                                                        listOfOptions->setValueOf( labelMusic, ss.str() );
                                                        SoundManager::getInstance()->setVolumeOfMusic( value );
                                                }
                                                else if ( listOfOptions->getActiveOption () == labelEffects )
                                                {
                                                        listOfOptions->setValueOf( labelEffects, ss.str() );
                                                        SoundManager::getInstance()->setVolumeOfEffects( value );
                                                }
                                        }
                                }
                                else if ( listOfOptions->getActiveOption () == playRoomTunes )
                                {
                                        if ( theKey == KEY_LEFT || theKey == KEY_RIGHT )
                                        {
                                                isomot::GameManager::getInstance()->togglePlayMelodyOfScenery ();
                                                listOfOptions->setValueOf( playRoomTunes, isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope );

                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                {
                                        screen->getKeyHandler()->handleKey ( theKey << 8 );
                                }

                                clear_keybuf();
                                listOfOptions->redraw ();
                        }
                }

                // no te comas la CPU
                // do not eat the CPU
                milliSleep( 25 );
        }
}
