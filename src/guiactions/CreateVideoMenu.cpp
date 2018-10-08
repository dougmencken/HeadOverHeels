
#include "CreateVideoMenu.hpp"
#include "CreateMenuOfGraphicSets.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateVideoMenu ;
using gui::CreateMenuOfGraphicSets ;


CreateVideoMenu::CreateVideoMenu( Picture * picture ) :
        Action( picture ),
        listOfOptions ( nilPointer ),
        labelScreenSize ( nilPointer ),
        labelFullscreen ( nilPointer ),
        labelDrawBackground ( nilPointer ),
        labelGraphicSet ( nilPointer )
{

}

void CreateVideoMenu::doAction ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() == 0 )
        {
                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                LanguageManager* languageManager = gui::GuiManager::getInstance()->getLanguageManager();

                LanguageText* textScreenSize = languageManager->findLanguageStringForAlias( "screen-size" );
                LanguageText* textFullscreen = languageManager->findLanguageStringForAlias( "full-screen" );
                LanguageText* textDrawShadows = languageManager->findLanguageStringForAlias( "draw-shadows" );
                LanguageText* textDrawBackground = languageManager->findLanguageStringForAlias( "draw-background" );

                std::string yeah = languageManager->findLanguageStringForAlias( "yep" )-> getText ();
                std::string nope = languageManager->findLanguageStringForAlias( "nope" )->getText ();

                this->labelScreenSize = new Label( textScreenSize->getText() );
                this->labelFullscreen = new Label( textFullscreen->getText() );
                this->labelDrawShadows = new Label( textDrawShadows->getText() );
                this->labelDrawBackground = new Label( textDrawBackground->getText() );

                LanguageText* textGraphicSet = languageManager->findLanguageStringForAlias( "graphic-set" );
                this->labelGraphicSet = new Label( textGraphicSet->getText(), "", "yellow" );
                labelGraphicSet->setAction( new CreateMenuOfGraphicSets( getWhereToDraw(), this ) );

                this->listOfOptions = new MenuWithValues( ' ', 1 );

                listOfOptions->addOption( labelScreenSize );
                listOfOptions->addOption( labelFullscreen );
                listOfOptions->addOption( labelDrawShadows );
                listOfOptions->addOption( labelDrawBackground );
                listOfOptions->addOption( labelGraphicSet );

                updateLabels ();

                listOfOptions->setVerticalOffset( 33 );

                screen->addWidget( listOfOptions );
        }
        else
        {
                updateLabels();
        }

        screen->setEscapeAction( new CreateMainMenu( getWhereToDraw() ) );

        if ( screen->getKeyHandler() == nilPointer )
        {
                screen->setKeyHandler( listOfOptions );
        }

        gui::GuiManager::getInstance()->changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( ! screen->getEscapeAction()->hasBegun() )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key pressed by user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                screen->handleKey( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == "Left" || theKey == "Right" )
                                {
                                        if ( listOfOptions->getActiveOption () == labelFullscreen )
                                        {
                                                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == labelDrawShadows )
                                        {
                                                isomot::GameManager::getInstance()->toggleDrawShadows ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == labelDrawBackground )
                                        {
                                                isomot::GameManager::getInstance()->toggleBackgroundPicture ();
                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                {
                                        screen->getKeyHandler()->handleKey ( theKey );
                                }

                                allegro::emptyKeyboardBuffer();

                                // update labels of options now
                                updateLabels();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        milliSleep( 25 );
                }
        }
}

void CreateVideoMenu::updateLabels ()
{
        LanguageManager* languageManager = gui::GuiManager::getInstance()->getLanguageManager();
        std::string yeah = languageManager->findLanguageStringForAlias( "yep" )-> getText ();
        std::string nope = languageManager->findLanguageStringForAlias( "nope" )->getText ();

        listOfOptions->setValueOf( labelDrawBackground, isomot::GameManager::getInstance()->hasBackgroundPicture () ? yeah : nope );
        listOfOptions->setValueOf( labelDrawShadows, isomot::GameManager::getInstance()->getDrawShadows () ? yeah : nope );
        listOfOptions->setValueOf( labelFullscreen, gui::GuiManager::getInstance()->isAtFullScreen () ? yeah : nope );
        // labelGraphicSet has no value but action

        std::string screenSize = languageManager->findLanguageStringForAlias( "screen-size" )->getText ();
        labelScreenSize->setText( screenSize + " " + isomot::numberToString( isomot::ScreenWidth() ) + " x " + isomot::numberToString( isomot::ScreenHeight() ) );

        listOfOptions->redraw ();
}
