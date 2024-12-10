
#include "CreateOptionsMenu.hpp"

#include "GuiManager.hpp"
#include "Label.hpp"
#include "SlideWithHeadAndHeels.hpp"

#include "CreateKeysMenu.hpp"
#include "CreateAudioMenu.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateLanguageMenu.hpp"
#include "PresentTheMainMenu.hpp"


void gui::CreateOptionsMenu::act ()
{
        SlideWithHeadAndHeels & optionsSlide = GuiManager::getInstance().findOrCreateSlideWithHeadAndHeelsForAction( getNameOfAction() );

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

                optionsSlide.setEscapeAction( new PresentTheMainMenu() );
        }

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}
