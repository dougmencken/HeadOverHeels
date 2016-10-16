#include "CreateAudioMenu.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "AdjustSoundFx.hpp"
#include "AdjustMusic.hpp"

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
        screen->setAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ true );

        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        std::stringstream ss;

        // 1. Efectos sonoros
        ss << SoundManager::getInstance()->getVolumeOfEffects();
        langString = languageManager->findLanguageString( "soundfx" );
        Menu* menu = new Menu( langString->getX(), langString->getY() );
        label = new Label( langString->getText() + ss.str() );
        label->setAction( new AdjustSoundFx( menu, langString->getText() ) );
        menu->addActiveOption( label );

        // 2. Música
        ss.str( std::string() );
        ss << SoundManager::getInstance()->getVolumeOfMusic();
        langString = languageManager->findLanguageString( "music" );
        label = new Label( langString->getText() + ss.str() );
        label->setAction( new AdjustMusic( menu, langString->getText() ) );
        menu->addOption( label );
        screen->addWidget( menu );

        // Crea la cadena de responsabilidad
        screen->setSucessor( menu );

        // Cambia la pantalla mostrada en la interfaz
        GuiManager::getInstance()->changeScreen( screen );
}
