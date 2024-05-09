
#include "CreateCongratulationsScreen.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "GamePreferences.hpp"
#include "Screen.hpp"
#include "PictureWidget.hpp"
#include "TextField.hpp"
#include "CreateEndScreen.hpp"

#include "ospaths.hpp"


namespace gui {

CreateCongratulationsScreen::CreateCongratulationsScreen( unsigned short rooms, unsigned short planets )
        : Action( )
        , rooms( rooms )
        , planets( planets )
{
}

void CreateCongratulationsScreen::act ()
{
        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( screen.isNewAndEmpty() )
                screen.setEscapeAction( new CreateEndScreen( rooms, planets ) );
        else
                screen.freeWidgets() ;

        {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                autouniqueptr< Picture > imageDuChapeau( Picture::loadPicture( ospaths::pathToFile( pathToPictures, "crown.png" ) ) );

                screen.addWidget( new PictureWidget( 192, 50, PicturePtr( new Picture( *imageDuChapeau ) ), "image du chapeau de Head" ) );
                screen.addPictureOfHeadAt( 192, 100 );

                screen.addWidget( new PictureWidget( 400, 50, PicturePtr( new Picture( *imageDuChapeau ) ), "image du chapeau de Heels" ) );
                screen.addPictureOfHeelsAt( 400, 100 );
        }

        // texto final
        LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;
        LanguageText* finalText = languageStrings->getTranslatedTextByAlias( "final-text" );
        TextField* textField = new TextField( GamePreferences::getScreenWidth(), "center" );
        textField->moveTo( 0, 180 );

        for ( size_t i = 0; i < finalText->howManyLinesOfText(); i ++ ) {
                const LanguageLine & line = finalText->getNthLine( i );
                textField->appendText( line.getString(), line.isBigHeight(), line.getColor() );
        }

        screen.addWidget( textField );

        GuiManager::getInstance().changeScreen( screen, true );
}

}
