
#include "CreatePlanetsScreen.hpp"

#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
#include "Color.hpp"
#include "Screen.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "ContinueGame.hpp"

#include "screen.hpp"

using gui::CreatePlanetsScreen ;
using gui::ContinueGame ;
using gui::GuiManager ;


CreatePlanetsScreen::CreatePlanetsScreen( bool notNewGame )
        : Action( )
        , gameInProgress( notNewGame )
        , blacktoothFree( false )
        , egyptusFree( false )
        , penitentiaryFree( false )
        , byblosFree( false )
        , safariFree( false )
{

}

CreatePlanetsScreen::~CreatePlanetsScreen( )
{

}

void CreatePlanetsScreen::doAction ()
{
        SoundManager::getInstance().playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        LanguageManager* languageManager = GuiManager::getInstance().getLanguageManager();

        if ( GameManager::getInstance().isSimpleGraphicsSet () )
                Screen::toBlackBackground () ; // change the background from red to black

        Screen & planets = * GuiManager::getInstance().findOrCreateScreenForAction( this );

        if ( planets.countWidgets() > 0 )
        {
                planets.freeWidgets() ;
        }

        const unsigned int screenWidth = variables::getScreenWidth();
        const unsigned int screenHeight = variables::getScreenHeight();

        // “ El Imperio Blacktooth ”
        std::string colorOfLabel = "yellow" ;
        if ( GameManager::getInstance().isSimpleGraphicsSet () ) colorOfLabel = "red" ;
        Label* empire = new Label( languageManager->findLanguageStringForAlias( "blacktooth-empire" )->getText(), "big", colorOfLabel );
        empire->moveTo( ( screenWidth - empire->getWidth() ) >> 1, 2 );
        empire->setAction( new ContinueGame( gameInProgress ) );

        planets.addWidget( empire );
        planets.setKeyHandler( empire );

        PicturePtr chapeau( Screen::loadPicture( "crown.png" ) );
        PicturePtr chapeauTriste( new Picture( *chapeau ) );

        if ( ! GameManager::getInstance().isSimpleGraphicsSet () )
        {
                chapeauTriste->toGrayscale();
        }
        else
        {
                chapeauTriste->colorize( Color::byName( "gray" ) );
                chapeau->colorize( Color::byName( "yellow" ) );
        }

        const int halfOfChapeauWidth = chapeau->getWidth() >> 1;
        const int chapeauOffsetY = - ( chapeau->getHeight() + ( screenHeight >> 7 ) ) ;
        const int labelOffsetY = 80;
        const int topOffsetY = 60;

        // Blacktooth

        PicturePtr planetBlacktooth( Screen::loadPicture( "blacktooth.png" ) );
        if ( planetBlacktooth->getAllegroPict().isNotNil() )
        {
                const int blacktoothX = ( screenWidth - planetBlacktooth->getWidth() ) >> 1 ;
                const int blacktoothY = ( ( screenHeight - planetBlacktooth->getHeight() ) >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfBlacktooth = new PictureWidget( blacktoothX, blacktoothY, planetBlacktooth, "planet Blacktooth" );
                planets.addWidget( imageOfBlacktooth );

                PictureWidget* imageOfChapeau = new PictureWidget(
                        blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                        blacktoothY + chapeauOffsetY,
                        blacktoothFree ? chapeau : chapeauTriste,
                        "image of chapeau for blacktooth"
                );
                planets.addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "blacktooth" )->getText() );
                nameOfPlanet->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), blacktoothY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Egyptus

        PicturePtr planetEgyptus( Screen::loadPicture( "egyptus.png" ) );
        if ( planetEgyptus->getAllegroPict().isNotNil() )
        {
                const int egyptusX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetEgyptus->getWidth() >> 1 ) ;
                const int egyptusY = ( screenHeight >> 2 ) - ( planetEgyptus->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfEgyptus = new PictureWidget( egyptusX, egyptusY, planetEgyptus, "planet Egyptus" );
                planets.addWidget( imageOfEgyptus );

                PictureWidget* imageOfChapeau = new PictureWidget(
                        egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                        egyptusY + chapeauOffsetY,
                        egyptusFree ? chapeau : chapeauTriste,
                        "image of chapeau for egyptus"
                );
                planets.addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "egyptus" )->getText() );
                nameOfPlanet->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), egyptusY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Penitentiary

        PicturePtr planetPenitentiary( Screen::loadPicture( "penitentiary.png" ) );
        if ( planetPenitentiary->getAllegroPict().isNotNil() )
        {
                const int penitentiaryX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetPenitentiary->getWidth() >> 1 );
                const int penitentiaryY = ( screenHeight >> 2 ) - ( planetPenitentiary->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfPenitentiary = new PictureWidget( penitentiaryX, penitentiaryY, planetPenitentiary, "planet Penitentiary" );
                planets.addWidget( imageOfPenitentiary );

                PictureWidget* imageOfChapeau = new PictureWidget(
                        penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                        penitentiaryY + chapeauOffsetY,
                        penitentiaryFree ? chapeau : chapeauTriste,
                        "image of chapeau for penitentiary"
                );
                planets.addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "penitentiary" )->getText() );
                nameOfPlanet->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Byblos

        PicturePtr planetByblos( Screen::loadPicture( "byblos.png" ) );
        if ( planetByblos->getAllegroPict().isNotNil() )
        {
                const int byblosX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetByblos->getWidth() >> 1 ) ;
                const int byblosY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetByblos->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfByblos = new PictureWidget( byblosX, byblosY, planetByblos, "planet Bookworld" );
                planets.addWidget( imageOfByblos );

                PictureWidget* imageOfChapeau = new PictureWidget(
                        byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                        byblosY + chapeauOffsetY,
                        byblosFree ? chapeau : chapeauTriste,
                        "image of chapeau for byblos"
                );
                planets.addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "byblos" )->getText() );
                nameOfPlanet->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), byblosY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        // Safari

        PicturePtr planetSafari( Screen::loadPicture( "safari.png" ) );
        if ( planetSafari->getAllegroPict().isNotNil() )
        {
                const int safariX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetSafari->getWidth() >> 1 );
                const int safariY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetSafari->getHeight() >> 1 ) + ( topOffsetY >> 1 );

                PictureWidget* imageOfSafari = new PictureWidget( safariX, safariY, planetSafari, "planet Safari" );
                planets.addWidget( imageOfSafari );

                PictureWidget* imageOfChapeau = new PictureWidget(
                        safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                        safariY + chapeauOffsetY,
                        safariFree ? chapeau : chapeauTriste,
                        "image of chapeau for safari"
                );
                planets.addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "safari" )->getText() );
                nameOfPlanet->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), safariY + labelOffsetY );
                planets.addWidget( nameOfPlanet );
        }

        GuiManager::getInstance().changeScreen( planets, true );
}
