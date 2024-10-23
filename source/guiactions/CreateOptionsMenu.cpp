
#include "CreateOptionsMenu.hpp"

#include "GuiManager.hpp"
#include "Label.hpp"
#include "Slide.hpp"

#include "CreateKeysMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateLanguageMenu.hpp"
#include "CreateMainMenu.hpp"


void gui::CreateOptionsMenu::act ()
{
        Slide & optionsSlide = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( optionsSlide.isNewAndEmpty() )
        {
                optionsSlide.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                optionsMenu.deleteAllOptions() ;

                Label* defineKeys = optionsMenu.addOptionByLanguageTextAlias( "keys-menu" ) ;
                Label* adjustAudio = optionsMenu.addOptionByLanguageTextAlias( "audio-menu" ) ;
                Label* adjustVideo = optionsMenu.addOptionByLanguageTextAlias( "video-menu" ) ;
                Label* chooseLanguage = optionsMenu.addOptionByLanguageTextAlias( "language-menu" );

                defineKeys->setAction( new CreateKeysMenu() );
                adjustAudio->setAction( new CreateAudioMenu() );
                adjustVideo->setAction( new CreateVideoMenu() );
                chooseLanguage->setAction( new CreateLanguageMenu() );

                optionsSlide.addWidget( & optionsMenu );
                optionsSlide.setKeyHandler( & optionsMenu );

                optionsSlide.setEscapeAction( new CreateMainMenu() );
        }

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}
