
#include "ShowSlideWithPlanets.hpp"

#include "GameManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
#include "GamePreferences.hpp"
#include "Color.hpp"
#include "Slide.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "ContinueGame.hpp"

#include "ospaths.hpp"


void gui::ShowSlideWithPlanets::act ()
{
        SoundManager::getInstance().playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        if ( GameManager::getInstance().isSimpleGraphicsSet () )
                Slide::toBlackBackground () ; // change the background from red to black

        Slide & planets = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( ! planets.isNewAndEmpty() ) planets.removeAllWidgets() ;

        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int screenHeight = GamePreferences::getScreenHeight();

        // “ El Imperio Blacktooth ”
        std::string colorOfLabel = "yellow" ;
        if ( GameManager::getInstance().isSimpleGraphicsSet () ) colorOfLabel = "red" ;
        Label* empire = new Label( languageStrings.getTranslatedTextByAlias( "blacktooth-empire" ).getText(),
                                        new Font( colorOfLabel, /* double height */ true ) );
        empire->moveTo( ( screenWidth - empire->getWidth() ) >> 1, 2 );
        empire->setAction( new ContinueGame() ); // from slides and menus to the game

        planets.addWidget( empire );
        planets.setKeyHandler( empire );

        const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;

        autouniqueptr< Picture > chapeau ( new Picture( pathToPictures, "crown.png" ) );
        autouniqueptr< Picture > chapeauTriste ( new Picture( *chapeau ) );

        if ( ! GameManager::getInstance().isSimpleGraphicsSet () )
                chapeauTriste->toGrayscale ();
        else {
                chapeauTriste->colorizeWhite( Color::byName( "gray" ) );
                chapeau->colorizeWhite( Color::byName( "yellow" ) );
        }

        const int halfOfChapeauWidth = chapeau->getWidth() >> 1;
        const int chapeauOffsetY = - ( chapeau->getHeight() + ( screenHeight >> 7 ) ) ;
        const int labelOffsetY = 80;
        const int topOffsetY = 60;

        // Blacktooth

        autouniqueptr< Picture > planetBlacktooth ( new Picture( pathToPictures, "blacktooth.png" ) );
        if ( planetBlacktooth != nilPointer && planetBlacktooth->getAllegroPict().isNotNil() )
        {
                const int blacktoothX = ( screenWidth - planetBlacktooth->getWidth() ) >> 1 ;
                const int blacktoothY = ( ( screenHeight - planetBlacktooth->getHeight() ) >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfBlacktooth = new PictureWidget( blacktoothX, blacktoothY, * planetBlacktooth, "planet Blacktooth" );
                planets.addWidget( imageOfBlacktooth );

                PictureWidget* blacktoothChapeau = new PictureWidget(
                        blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                        blacktoothY + chapeauOffsetY,
                        GameManager::getInstance().isFreePlanet( "blacktooth" ) ? *chapeau : *chapeauTriste,
                        "image of chapeau for blacktooth"
                );
                planets.addWidget( blacktoothChapeau );

                Label* nameOfPlanet = new Label( languageStrings.getTranslatedTextByAlias( "blacktooth" ).getText() );
                nameOfPlanet->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), blacktoothY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Egyptus

        autouniqueptr< Picture > planetEgyptus ( new Picture( pathToPictures, "egyptus.png" ) );
        if ( planetEgyptus != nilPointer && planetEgyptus->getAllegroPict().isNotNil() )
        {
                const int egyptusX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetEgyptus->getWidth() >> 1 ) ;
                const int egyptusY = ( screenHeight >> 2 ) - ( planetEgyptus->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfEgyptus = new PictureWidget( egyptusX, egyptusY, * planetEgyptus, "planet Egyptus" );
                planets.addWidget( imageOfEgyptus );

                PictureWidget* egyptusChapeau = new PictureWidget(
                        egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                        egyptusY + chapeauOffsetY,
                        GameManager::getInstance().isFreePlanet( "egyptus" ) ? *chapeau : *chapeauTriste,
                        "image of chapeau for egyptus"
                );
                planets.addWidget( egyptusChapeau );

                Label* nameOfPlanet = new Label( languageStrings.getTranslatedTextByAlias( "egyptus" ).getText() );
                nameOfPlanet->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), egyptusY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Penitentiary

        autouniqueptr< Picture > planetPenitentiary ( new Picture( pathToPictures, "penitentiary.png" ) );
        if ( planetPenitentiary != nilPointer && planetPenitentiary->getAllegroPict().isNotNil() )
        {
                const int penitentiaryX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetPenitentiary->getWidth() >> 1 );
                const int penitentiaryY = ( screenHeight >> 2 ) - ( planetPenitentiary->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfPenitentiary = new PictureWidget( penitentiaryX, penitentiaryY, * planetPenitentiary, "planet Penitentiary" );
                planets.addWidget( imageOfPenitentiary );

                PictureWidget* penitentiaryChapeau = new PictureWidget(
                        penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                        penitentiaryY + chapeauOffsetY,
                        GameManager::getInstance().isFreePlanet( "penitentiary" ) ? *chapeau : *chapeauTriste,
                        "image of chapeau for penitentiary"
                );
                planets.addWidget( penitentiaryChapeau );

                Label* nameOfPlanet = new Label( languageStrings.getTranslatedTextByAlias( "penitentiary" ).getText() );
                nameOfPlanet->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Byblos

        autouniqueptr< Picture > planetByblos ( new Picture( pathToPictures, "byblos.png" ) );
        if ( planetByblos != nilPointer && planetByblos->getAllegroPict().isNotNil() )
        {
                const int byblosX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetByblos->getWidth() >> 1 ) ;
                const int byblosY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetByblos->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfByblos = new PictureWidget( byblosX, byblosY, * planetByblos, "planet Bookworld" );
                planets.addWidget( imageOfByblos );

                PictureWidget* byblosChapeau = new PictureWidget(
                        byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                        byblosY + chapeauOffsetY,
                        GameManager::getInstance().isFreePlanet( "byblos" ) ? *chapeau : *chapeauTriste,
                        "image of chapeau for byblos"
                );
                planets.addWidget( byblosChapeau );

                Label* nameOfPlanet = new Label( languageStrings.getTranslatedTextByAlias( "byblos" ).getText() );
                nameOfPlanet->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), byblosY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Safari

        autouniqueptr< Picture > planetSafari ( new Picture( pathToPictures, "safari.png" ) );
        if ( planetSafari != nilPointer && planetSafari->getAllegroPict().isNotNil() )
        {
                const int safariX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetSafari->getWidth() >> 1 );
                const int safariY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetSafari->getHeight() >> 1 ) + ( topOffsetY >> 1 );

                PictureWidget* imageOfSafari = new PictureWidget( safariX, safariY, * planetSafari, "planet Safari" );
                planets.addWidget( imageOfSafari );

                PictureWidget* safariChapeau = new PictureWidget(
                        safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                        safariY + chapeauOffsetY,
                        GameManager::getInstance().isFreePlanet( "safari" ) ? *chapeau : *chapeauTriste,
                        "image of chapeau for safari"
                );
                planets.addWidget( safariChapeau );

                Label* nameOfPlanet = new Label( languageStrings.getTranslatedTextByAlias( "safari" ).getText() );
                nameOfPlanet->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), safariY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        planets.setTransitionFromThisSlideOff() ; // no transition because after this slide the game begins
        GuiManager::getInstance().changeToSlideFor( getNameOfAction(), true );
}
