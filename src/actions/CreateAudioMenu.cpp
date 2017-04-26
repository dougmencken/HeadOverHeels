
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
        where( picture ),
        listOfOptions ( 0 ),
        labelEffects ( 0 ),
        labelMusic ( 0 )
{

}

void CreateAudioMenu::doIt ()
{
        const size_t positionOfValue = 20;

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        LanguageText* langStringEffects = languageManager->findLanguageString( "soundfx" );
        LanguageText* langStringMusic = languageManager->findLanguageString( "music" );

        std::stringstream ss;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

                ss.str( std::string() ); // clear ss

                // Efectos sonoros
                ss << SoundManager::getInstance()->getVolumeOfEffects();

                std::string stringEffectsSpaced ( langStringEffects->getText() );
                for ( size_t position = stringEffectsSpaced.length() ; position < positionOfValue ; ++position ) {
                        stringEffectsSpaced = stringEffectsSpaced + " ";
                }
                this->labelEffects = new Label( stringEffectsSpaced + ss.str() );

                ss.str( std::string() );

                // Música
                ss << SoundManager::getInstance()->getVolumeOfMusic();

                std::string stringMusicSpaced ( langStringMusic->getText() );
                for ( size_t position = stringMusicSpaced.length() ; position < positionOfValue ; ++position ) {
                        stringMusicSpaced = stringMusicSpaced + " ";
                }
                this->labelMusic = new Label( stringMusicSpaced + ss.str() );

                this->listOfOptions = new Menu( langStringEffects->getX(), langStringEffects->getY() );
                listOfOptions->addOption( labelEffects );
                listOfOptions->addOption( labelMusic );

                screen->addWidget( listOfOptions );
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
                                int musicVolume = SoundManager::getInstance()->getVolumeOfMusic();
                                int effectsVolume = SoundManager::getInstance()->getVolumeOfEffects();
                                int value = ( listOfOptions->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;
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
                                        ss.str( std::string() ); // clear ss
                                        ss << value;

                                        if ( listOfOptions->getActiveOption () == labelMusic )
                                        {
                                                std::string musicStringSpaced ( langStringMusic->getText() );
                                                for ( size_t position = musicStringSpaced.length() ; position < positionOfValue ; ++position ) {
                                                        musicStringSpaced = musicStringSpaced + " ";
                                                }
                                                labelMusic->setText( musicStringSpaced + ss.str() );
                                                SoundManager::getInstance()->setVolumeOfMusic( value );
                                        }
                                        else if ( listOfOptions->getActiveOption () == labelEffects )
                                        {
                                                std::string stringEffectsSpaced ( langStringEffects->getText() );
                                                for ( size_t position = stringEffectsSpaced.length() ; position < positionOfValue ; ++position ) {
                                                        stringEffectsSpaced = stringEffectsSpaced + " ";
                                                }
                                                labelEffects->setText( stringEffectsSpaced + ss.str() );
                                                SoundManager::getInstance()->setVolumeOfEffects( value );
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
