
#include "CreatePlanetsScreen.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Screen.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "ContinueGame.hpp"

using gui::CreatePlanetsScreen ;
using gui::ContinueGame ;
using gui::GuiManager ;
using isomot::GameManager ;
using isomot::SoundManager ;


CreatePlanetsScreen::CreatePlanetsScreen( Picture * where, bool notNewGame )
        : Action( where )
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
        SoundManager::getInstance()->playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        Screen* planets = GuiManager::getInstance()->findOrCreateScreenForAction( this );

        if ( planets->countWidgets() > 0 )
        {
                planets->freeWidgets() ;
        }

        const unsigned int screenWidth = isomot::ScreenWidth();
        const unsigned int screenHeight = isomot::ScreenHeight();

        // etiqueta fija “ El Imperio Blacktooth ”
        std::string colorOfLabel = "yellow";
        if ( GameManager::getInstance()->isSimpleGraphicSet () ) colorOfLabel = "green";
        Label* empire = new Label( languageManager->findLanguageStringForAlias( "blacktooth-empire" )->getText(), "big", colorOfLabel );
        empire->moveTo( ( screenWidth - empire->getWidth() ) >> 1, 2 );
        empire->setAction( new ContinueGame( getWhereToDraw(), gameInProgress ) );

        planets->addWidget( empire );
        planets->setKeyHandler( empire );

        PictureWidget* imageOfChapeau = nilPointer;

        Picture* chapeau = Screen::loadPicture( "crown.png" ) ;
        Picture* chapeauTriste = nilPointer;
        if ( ! GameManager::getInstance()->isSimpleGraphicSet () )
        {
                chapeauTriste = chapeau->makeGrayscaleCopy();
        }
        else
        {
                chapeauTriste = chapeau->makeColorizedCopy( Color::gray50Color() );
                chapeau->colorize( Color::yellowColor() );
        }

        const int halfOfChapeauWidth = chapeau->getWidth() >> 1;
        const int chapeauOffsetY = - ( chapeau->getHeight() + ( screenHeight >> 7 ) ) ;
        const int labelOffsetY = 80;
        const int topOffsetY = 60;

        // Blacktooth

        Picture* planetBlacktooth = Screen::loadPicture( "blacktooth.png" ) ;
        if ( planetBlacktooth->getAllegroPict().isNotNil() )
        {
                const int blacktoothX = ( screenWidth - planetBlacktooth->getWidth() ) >> 1 ;
                const int blacktoothY = ( ( screenHeight - planetBlacktooth->getHeight() ) >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfBlacktooth = new PictureWidget( blacktoothX, blacktoothY, planetBlacktooth, "planet Blacktooth" );
                planets->addWidget( imageOfBlacktooth );

                imageOfChapeau = new PictureWidget(
                        blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                        blacktoothY + chapeauOffsetY,
                        new Picture( blacktoothFree ? *chapeau : *chapeauTriste ),
                        "image of chapeau for blacktooth"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "blacktooth" )->getText() );
                nameOfPlanet->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), blacktoothY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Egyptus

        Picture* planetEgyptus = Screen::loadPicture( "egyptus.png" ) ;
        if ( planetEgyptus->getAllegroPict().isNotNil() )
        {
                const int egyptusX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetEgyptus->getWidth() >> 1 ) ;
                const int egyptusY = ( screenHeight >> 2 ) - ( planetEgyptus->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfEgyptus = new PictureWidget( egyptusX, egyptusY, planetEgyptus, "planet Egyptus" );
                planets->addWidget( imageOfEgyptus );

                imageOfChapeau = new PictureWidget(
                        egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                        egyptusY + chapeauOffsetY,
                        new Picture( egyptusFree ? *chapeau : *chapeauTriste ),
                        "image of chapeau for egyptus"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "egyptus" )->getText() );
                nameOfPlanet->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), egyptusY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Penitentiary

        Picture* planetPenitentiary = Screen::loadPicture( "penitentiary.png" ) ;
        if ( planetPenitentiary->getAllegroPict().isNotNil() )
        {
                const int penitentiaryX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetPenitentiary->getWidth() >> 1 );
                const int penitentiaryY = ( screenHeight >> 2 ) - ( planetPenitentiary->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfPenitentiary = new PictureWidget( penitentiaryX, penitentiaryY, planetPenitentiary, "planet Penitentiary" );
                planets->addWidget( imageOfPenitentiary );

                imageOfChapeau = new PictureWidget(
                        penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                        penitentiaryY + chapeauOffsetY,
                        new Picture( penitentiaryFree ? *chapeau : *chapeauTriste ),
                        "image of chapeau for penitentiary"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "penitentiary" )->getText() );
                nameOfPlanet->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Byblos

        Picture* planetByblos = Screen::loadPicture( "byblos.png" ) ;
        if ( planetByblos->getAllegroPict().isNotNil() )
        {
                const int byblosX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetByblos->getWidth() >> 1 ) ;
                const int byblosY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetByblos->getHeight() >> 1 ) + ( topOffsetY >> 1 ) ;

                PictureWidget* imageOfByblos = new PictureWidget( byblosX, byblosY, planetByblos, "planet Bookworld" );
                planets->addWidget( imageOfByblos );

                imageOfChapeau = new PictureWidget(
                        byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                        byblosY + chapeauOffsetY,
                        new Picture( byblosFree ? *chapeau : *chapeauTriste ),
                        "image of chapeau for byblos"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "byblos" )->getText() );
                nameOfPlanet->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), byblosY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Safari

        Picture* planetSafari = Screen::loadPicture( "safari.png" ) ;
        if ( planetSafari->getAllegroPict().isNotNil() )
        {
                const int safariX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetSafari->getWidth() >> 1 );
                const int safariY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetSafari->getHeight() >> 1 ) + ( topOffsetY >> 1 );

                PictureWidget* imageOfSafari = new PictureWidget( safariX, safariY, planetSafari, "planet Safari" );
                planets->addWidget( imageOfSafari );

                imageOfChapeau = new PictureWidget(
                        safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                        safariY + chapeauOffsetY,
                        new Picture( safariFree ? *chapeau : *chapeauTriste ),
                        "image of chapeau for safari"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "safari" )->getText() );
                nameOfPlanet->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), safariY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        delete chapeau ;
        delete chapeauTriste ;

        GuiManager::getInstance()->changeScreen( planets, true );
}
