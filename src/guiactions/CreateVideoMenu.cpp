
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


CreateVideoMenu::CreateVideoMenu( allegro::Pict* picture ) :
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
                this->labelGraphicSet = new Label( textGraphicSet->getText(), "regular", "yellow" );
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

        clear_keybuf();

        while ( ! screen->getEscapeAction()->hasBegun() )
        {
                if ( keypressed() )
                {
                        // get the key pressed by user
                        int theKey = readkey() >> 8;

                        if ( theKey == KEY_ESC )
                        {
                                clear_keybuf();
                                screen->handleKey( theKey << 8 );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == KEY_LEFT || theKey == KEY_RIGHT )
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
                                        screen->getKeyHandler()->handleKey ( theKey << 8 );
                                }

                                clear_keybuf();

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
