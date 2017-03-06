
#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
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
        where( picture )
{

}

void CreateAudioMenu::doIt ()
{
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        std::stringstream ss;

        const size_t positionOfValue = 20;

        // Efectos sonoros
        ss << SoundManager::getInstance()->getVolumeOfEffects();
        LanguageText* langStringEffects = languageManager->findLanguageString( "soundfx" );
        std::string stringEffectsSpaced ( langStringEffects->getText() );
        for ( size_t position = stringEffectsSpaced.length() ; position < positionOfValue ; ++position ) {
                stringEffectsSpaced = stringEffectsSpaced + " ";
        }
        Label* labelEffects = new Label( stringEffectsSpaced + ss.str() );

        ss.str( std::string() ); // clear ss

        // Música
        ss << SoundManager::getInstance()->getVolumeOfMusic();
        LanguageText* langStringMusic = languageManager->findLanguageString( "music" );
        std::string stringMusicSpaced ( langStringMusic->getText() );
        for ( size_t position = stringMusicSpaced.length() ; position < positionOfValue ; ++position ) {
                stringMusicSpaced = stringMusicSpaced + " ";
        }
        Label* labelMusic = new Label( stringMusicSpaced + ss.str() );

        Menu* menu = new Menu( langStringEffects->getX(), langStringEffects->getY() );
        menu->addActiveOption( labelEffects );
        menu->addOption( labelMusic );

        screen->addWidget( menu );
        screen->setNext( menu );

        GuiManager::getInstance()->changeScreen( screen );
        GuiManager::getInstance()->refresh();

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
                                int musicVolume = SoundManager::getInstance()->getVolumeOfMusic();
                                int effectsVolume = SoundManager::getInstance()->getVolumeOfEffects();
                                int value = ( menu->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;
                                int previousValue = value;

                                bool doneWithKey = false;

                                if ( theKey == KEY_LEFT )
                                { // decrease the volume
                                        value = ( value > 0 ? value - 1 : 0 );
                                        doneWithKey = true;
                                }
                                else if ( theKey == KEY_RIGHT )
                                { // increase the volume
                                        value = ( value < 99 ? value + 1 : 99 );
                                        doneWithKey = true;
                                }

                                if ( value != previousValue )
                                {
                                        ss.str( std::string() );
                                        ss << value;

                                        if ( menu->getActiveOption () == labelMusic )
                                        {
                                                std::string musicStringSpaced ( langStringMusic->getText() );
                                                for ( size_t position = musicStringSpaced.length() ; position < positionOfValue ; ++position ) {
                                                        musicStringSpaced = musicStringSpaced + " ";
                                                }
                                                labelMusic->setText( musicStringSpaced + ss.str() );
                                                SoundManager::getInstance()->setVolumeOfMusic( value );
                                        }
                                        else if ( menu->getActiveOption () == labelEffects )
                                        {
                                                std::string stringEffectsSpaced ( langStringEffects->getText() );
                                                for ( size_t position = stringEffectsSpaced.length() ; position < positionOfValue ; ++position ) {
                                                        stringEffectsSpaced = stringEffectsSpaced + " ";
                                                }
                                                labelEffects->setText( stringEffectsSpaced + ss.str() );
                                                SoundManager::getInstance()->setVolumeOfEffects( value );
                                        }

                                        menu->redraw ();
                                }

                                if ( ! doneWithKey )
                                {
                                        menu->handleKey ( theKey << 8 );
                                        menu->redraw ();
                                }

                                clear_keybuf();
                        }
                }

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 25 );
        }
}
