
#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

#include <string>
#include <sstream>

using gui::CreateAudioMenu;
using isomot::SoundManager;


CreateAudioMenu::CreateAudioMenu( BITMAP* picture ) :
        Action(),
        where( picture ),
        listOfOptions ( 0 ),
        labelEffects ( 0 ),
        labelMusic ( 0 ),
        playRoomTunes ( 0 )
{

}

void CreateAudioMenu::doAction ()
{
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        LanguageText* langStringEffects = languageManager->findLanguageString( "soundfx" );
        LanguageText* langStringMusic = languageManager->findLanguageString( "music" );

        LanguageText* langStringRoomTunes = languageManager->findLanguageString( "play-room-melodies" );

        std::string yeah = languageManager->findLanguageString( "yep" )-> getText ();
        std::string nope = languageManager->findLanguageString( "nope" )->getText ();

        std::stringstream ss;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                ss.str( std::string() ); // clear ss

                // efectos sonoros
                ss << SoundManager::getInstance()->getVolumeOfEffects();

                std::string stringEffectsSpaced ( langStringEffects->getText() );
                for ( size_t position = utf8StringLength( stringEffectsSpaced ) ; position < positionOfSetting ; ++position ) {
                        stringEffectsSpaced = stringEffectsSpaced + " ";
                }
                this->labelEffects = new Label( stringEffectsSpaced + ss.str() );

                ss.str( std::string() );

                // música
                ss << SoundManager::getInstance()->getVolumeOfMusic();

                std::string stringMusicSpaced ( langStringMusic->getText() );
                for ( size_t position = utf8StringLength( stringMusicSpaced ) ; position < positionOfSetting ; ++position ) {
                        stringMusicSpaced = stringMusicSpaced + " ";
                }
                this->labelMusic = new Label( stringMusicSpaced + ss.str() );

                // play melody of scenery on entry to room or don’t
                std::string playTunesSpaced ( langStringRoomTunes->getText() );
                for ( size_t position = utf8StringLength( playTunesSpaced ) ; position < positionOfSetting ; ++position ) {
                        playTunesSpaced = playTunesSpaced + " ";
                }
                this->playRoomTunes = new Label( playTunesSpaced + ( isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope ) );

                this->listOfOptions = new Menu( );
                listOfOptions->addOption( labelEffects );
                listOfOptions->addOption( labelMusic );
                listOfOptions->addOption( playRoomTunes );
                listOfOptions->setVerticalOffset( 40 );

                screen->addWidget( listOfOptions );
        }
        else
        {
                // update labels
                std::string playTunesSpaced ( langStringRoomTunes->getText() );
                for ( size_t position = utf8StringLength( playTunesSpaced ) ; position < positionOfSetting ; ++position ) {
                        playTunesSpaced = playTunesSpaced + " ";
                }
                playRoomTunes->setText( playTunesSpaced + ( isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope ) );
        }

        if ( screen->getKeyHandler() == 0 )
        {
                screen->setKeyHandler( listOfOptions );
        }

        GuiManager::getInstance()->changeScreen( screen );

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
                                                        std::string musicStringSpaced ( langStringMusic->getText() );
                                                        for ( size_t position = utf8StringLength( musicStringSpaced ) ; position < positionOfSetting ; ++position ) {
                                                                musicStringSpaced = musicStringSpaced + " ";
                                                        }
                                                        labelMusic->setText( musicStringSpaced + ss.str() );
                                                        SoundManager::getInstance()->setVolumeOfMusic( value );
                                                }
                                                else if ( listOfOptions->getActiveOption () == labelEffects )
                                                {
                                                        std::string stringEffectsSpaced ( langStringEffects->getText() );
                                                        for ( size_t position = utf8StringLength( stringEffectsSpaced ) ; position < positionOfSetting ; ++position ) {
                                                                stringEffectsSpaced = stringEffectsSpaced + " ";
                                                        }
                                                        labelEffects->setText( stringEffectsSpaced + ss.str() );
                                                        SoundManager::getInstance()->setVolumeOfEffects( value );
                                                }
                                        }
                                }
                                else if ( listOfOptions->getActiveOption () == playRoomTunes )
                                {
                                        if ( theKey == KEY_LEFT || theKey == KEY_RIGHT )
                                        {
                                                isomot::GameManager::getInstance()->togglePlayMelodyOfScenery ();
                                                doneWithKey = true;

                                                std::string playTunesSpaced ( langStringRoomTunes->getText() );
                                                for ( size_t position = utf8StringLength( playTunesSpaced ) ; position < positionOfSetting ; ++position ) {
                                                        playTunesSpaced = playTunesSpaced + " ";
                                                }
                                                playRoomTunes->setText( playTunesSpaced + ( isomot::GameManager::getInstance()->playMelodyOfScenery () ? yeah : nope ) );
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

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 25 );
        }
}
