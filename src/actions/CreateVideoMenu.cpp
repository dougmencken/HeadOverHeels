
#include "CreateVideoMenu.hpp"
#include "CreateMenuOfGraphicSets.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateVideoMenu ;
using gui::CreateMenuOfGraphicSets ;


CreateVideoMenu::CreateVideoMenu( BITMAP* picture ) :
        Action(),
        where( picture ),
        listOfOptions ( 0 ),
        labelFullscreen ( 0 ),
        labelDrawBackground ( 0 ),
        labelGraphicSet ( 0 )
{

}

void CreateVideoMenu::doIt ()
{
        const size_t positionOfSetting = 20;

        LanguageManager* languageManager = gui::GuiManager::getInstance()->getLanguageManager();
        LanguageText* textFullscreen = languageManager->findLanguageString( "full-screen" );
        LanguageText* textDrawShadows = languageManager->findLanguageString( "draw-shadows" );
        LanguageText* textDrawBackground = languageManager->findLanguageString( "draw-background" );

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                screen->placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                std::string stringFullscreenSpaced ( textFullscreen->getText() );
                for ( size_t position = stringFullscreenSpaced.length() ; position < positionOfSetting ; ++position ) {
                        stringFullscreenSpaced = stringFullscreenSpaced + " ";
                }
                this->labelFullscreen = new Label( stringFullscreenSpaced + ( gui::GuiManager::getInstance()->isAtFullScreen () ? "yes" : "no" ) );

                std::string stringDrawShadowsSpaced ( textDrawShadows->getText() );
                for ( size_t position = stringDrawShadowsSpaced.length() ; position < positionOfSetting ; ++position ) {
                        stringDrawShadowsSpaced = stringDrawShadowsSpaced + " ";
                }
                this->labelDrawShadows = new Label( stringDrawShadowsSpaced + ( isomot::GameManager::getInstance()->getDrawShadows () ? "yes" : "no" ) );

                std::string stringDrawBackgroundSpaced ( textDrawBackground->getText() );
                for ( size_t position = stringDrawBackgroundSpaced.length() ; position < positionOfSetting ; ++position ) {
                        stringDrawBackgroundSpaced = stringDrawBackgroundSpaced + " ";
                }
                this->labelDrawBackground = new Label( stringDrawBackgroundSpaced + ( isomot::GameManager::getInstance()->hasBackgroundPicture () ? "yes" : "no" ) );

                LanguageText* textGraphicSet = languageManager->findLanguageString( "graphic-set" );
                this->labelGraphicSet = new Label( textGraphicSet->getText(), "regular", "yellow" );
                labelGraphicSet->setAction( new CreateMenuOfGraphicSets( this->where, this ) );

                this->listOfOptions = new Menu( textFullscreen->getX(), textFullscreen->getY() );
                listOfOptions->addOption( labelFullscreen );
                listOfOptions->addOption( labelDrawShadows );
                listOfOptions->addOption( labelDrawBackground );
                listOfOptions->addOption( labelGraphicSet );

                screen->addWidget( listOfOptions );
        }
        if ( screen->getKeyHandler() == 0 )
        {
                screen->setKeyHandler( listOfOptions );
        }

        gui::GuiManager::getInstance()->changeScreen( screen );

        clear_keybuf();

        while ( true )
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

                                // update labels of options here

                                std::string stringFullscreenSpaced ( textFullscreen->getText() );
                                for ( size_t position = stringFullscreenSpaced.length() ; position < positionOfSetting ; ++position ) {
                                        stringFullscreenSpaced = stringFullscreenSpaced + " ";
                                }
                                labelFullscreen->setText( stringFullscreenSpaced + ( gui::GuiManager::getInstance()->isAtFullScreen () ? "yes" : "no" ) );

                                std::string stringDrawShadowsSpaced ( textDrawShadows->getText() );
                                for ( size_t position = stringDrawShadowsSpaced.length() ; position < positionOfSetting ; ++position ) {
                                        stringDrawShadowsSpaced = stringDrawShadowsSpaced + " ";
                                }
                                labelDrawShadows->setText( stringDrawShadowsSpaced + ( isomot::GameManager::getInstance()->getDrawShadows () ? "yes" : "no" ) );

                                std::string stringDrawBackgroundSpaced ( textDrawBackground->getText() );
                                for ( size_t position = stringDrawBackgroundSpaced.length() ; position < positionOfSetting ; ++position ) {
                                        stringDrawBackgroundSpaced = stringDrawBackgroundSpaced + " ";
                                }
                                labelDrawBackground->setText( stringDrawBackgroundSpaced + ( isomot::GameManager::getInstance()->hasBackgroundPicture () ? "yes" : "no" ) );

                                listOfOptions->redraw ();
                        }

                        // No te comas la CPU
                        // Do not eat the CPU
                        sleep( 25 );
                }
        }
}
