
#include "ShowCongratulations.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "GamePreferences.hpp"
#include "Slide.hpp"
#include "PictureWidget.hpp"
#include "TextField.hpp"
#include "CreateGameOverSlide.hpp"

#include "ospaths.hpp"


void gui::ShowCongratulations::act ()
{
        Slide & theFinSlide = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( theFinSlide.isNewAndEmpty() )
                theFinSlide.setEscapeAction( new CreateGameOverSlide( rooms, planets ) );
        else
                theFinSlide.freeWidgets() ;

        {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                autouniqueptr< Picture > imageDuChapeau( Picture::loadPicture( ospaths::pathToFile( pathToPictures, "crown.png" ) ) );

                theFinSlide.addWidget( new PictureWidget( 192, 50, PicturePtr( new Picture( *imageDuChapeau ) ), "image du chapeau de Head" ) );
                theFinSlide.addPictureOfHeadAt( 192, 100 );

                theFinSlide.addWidget( new PictureWidget( 400, 50, PicturePtr( new Picture( *imageDuChapeau ) ), "image du chapeau de Heels" ) );
                theFinSlide.addPictureOfHeelsAt( 400, 100 );
        }

        // texto final
        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;
        TextField* finalTextField = new TextField( GamePreferences::getScreenWidth(), "center" );
        finalTextField->moveTo( 0, 180 );
        finalTextField->fillWithLanguageText( languageStrings.getTranslatedTextByAlias( "final-text" ) );

        theFinSlide.addWidget( finalTextField );

        theFinSlide.setTransitionToThisSlideOff() ;
        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}
