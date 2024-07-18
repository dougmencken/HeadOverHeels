
#include "CreateOptionsMenu.hpp"

#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "Label.hpp"
#include "Menu.hpp"
#include "Screen.hpp"

#include "CreateKeysMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateLanguageMenu.hpp"
#include "CreateMainMenu.hpp"


namespace gui
{

void CreateOptionsMenu::act ()
{
        Screen & optionsSlide = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( optionsSlide.isNewAndEmpty() )
        {
                optionsSlide.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

                Label* defineKeys = new Label( languageStrings->getTranslatedTextByAlias( "keys-menu" )->getText() );
                Label* adjustAudio = new Label( languageStrings->getTranslatedTextByAlias( "audio-menu" )->getText() );
                Label* adjustVideo = new Label( languageStrings->getTranslatedTextByAlias( "video-menu" )->getText() );
                Label* chooseLanguage = new Label( languageStrings->getTranslatedTextByAlias( "language-menu" )->getText() );

                defineKeys->setAction( new CreateKeysMenu() );
                adjustAudio->setAction( new CreateAudioMenu() );
                adjustVideo->setAction( new CreateVideoMenu() );
                chooseLanguage->setAction( new CreateLanguageMenu() );

                Menu * menu = new Menu( );
                menu->setVerticalOffset( 6 );

                menu->addOption( defineKeys );
                menu->addOption( adjustAudio );
                menu->addOption( adjustVideo );
                menu->addOption( chooseLanguage );

                optionsSlide.addWidget( menu );
                optionsSlide.setKeyHandler( menu );

                optionsSlide.setEscapeAction( new CreateMainMenu() );
        }

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

}
