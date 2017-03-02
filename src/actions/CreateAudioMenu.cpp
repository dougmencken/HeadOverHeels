#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateAudioMenu;
using isomot::SoundManager;


CreateAudioMenu::CreateAudioMenu( BITMAP* picture )
: Action(),
  where( picture )
{

}

void CreateAudioMenu::doIt ()
{
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ true );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        std::stringstream ss;

        // Efectos sonoros
        ss << SoundManager::getInstance()->getVolumeOfEffects();
        LanguageText* langStringFx = languageManager->findLanguageString( "soundfx" );
        std::string stringFxDotted ( langStringFx->getText() );
        for ( size_t position = stringFxDotted.length() ; position < 20 ; ++position ) {
                stringFxDotted = stringFxDotted + ".";
        }
        Label* labelEffects = new Label( stringFxDotted + ss.str() );

        ss.str( std::string() ); // clear ss

        // Música
        ss << SoundManager::getInstance()->getVolumeOfMusic();
        LanguageText* langStringMusic = languageManager->findLanguageString( "music" );
        std::string stringMusicDotted ( langStringMusic->getText() );
        for ( size_t position = stringMusicDotted.length() ; position < 20 ; ++position ) {
                stringMusicDotted = stringMusicDotted + ".";
        }
        Label* labelMusic = new Label( stringMusicDotted + ss.str() );

        Menu* menu = new Menu( langStringFx->getX(), langStringFx->getY() );
        menu->addActiveOption( labelEffects );
        menu->addOption( labelMusic );

        screen->addWidget( menu );
        screen->setNext( menu );

        // Cambia la pantalla mostrada en la interfaz
        GuiManager::getInstance()->changeScreen( screen );
        GuiManager::getInstance()->refresh();

        clear_keybuf();
        int theKey = KEY_MAX;
        while ( theKey != KEY_ESC )
        {
                int musicVolume = SoundManager::getInstance()->getVolumeOfMusic();
                int effectsVolume = SoundManager::getInstance()->getVolumeOfEffects();
                int value = ( menu->getActiveOption () == labelMusic ) ? musicVolume : effectsVolume ;

                if ( keypressed () )
                {
                        int previousValue = value;

                        // Tecla pulsada por el usuario
                        theKey = readkey() >> 8;

                        // Si se pulsa el cursor izquierdo se baja el volumen
                        if ( theKey == KEY_LEFT )
                        {
                                value = ( value > 0 ? value - 1 : 0 );
                                theKey = KEY_MAX;
                        }
                        // Si se pulsa el cursor derecho se sube el volumen
                        else if ( theKey == KEY_RIGHT )
                        {
                                value = ( value < 99 ? value + 1 : 99 );
                                theKey = KEY_MAX;
                        }
                        else if ( theKey == KEY_UP || theKey == KEY_DOWN )
                        {
                                menu->handleKey ( theKey << 8 );
                                GuiManager::getInstance()->refresh();
                                theKey = KEY_MAX;
                        }

                        if ( value != previousValue )
                        {
                                ss.str( std::string() );
                                ss << value;

                                if ( menu->getActiveOption () == labelMusic )
                                {
                                        std::string musicStringDotted ( langStringMusic->getText() );
                                        for ( size_t position = musicStringDotted.length() ; position < 20 ; ++position ) {
                                                musicStringDotted = musicStringDotted + ".";
                                        }
                                        labelMusic->setText( musicStringDotted + ss.str() );
                                        SoundManager::getInstance()->setVolumeOfMusic( value );
                                }
                                else if ( menu->getActiveOption () == labelEffects )
                                {
                                        std::string stringEffectsDotted ( langStringFx->getText() );
                                        for ( size_t position = stringEffectsDotted.length() ; position < 20 ; ++position ) {
                                                stringEffectsDotted = stringEffectsDotted + ".";
                                        }
                                        labelEffects->setText( stringEffectsDotted + ss.str() );
                                        SoundManager::getInstance()->setVolumeOfEffects( value );
                                }

                                GuiManager::getInstance()->refresh();
                        }

                        if ( theKey != KEY_MAX )
                        {
                                simulate_keypress( theKey << 8 );
                        }
                }

                // No te comas la CPU
                // Do not eat the CPU
                sleep( 25 );
        }
}
