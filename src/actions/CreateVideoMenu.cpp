
#include "CreateVideoMenu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

#include <string>

using gui::CreateVideoMenu;


CreateVideoMenu::CreateVideoMenu( BITMAP* picture ) :
        Action(),
        where( picture )
{

}

void CreateVideoMenu::doIt ()
{
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( gui::GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

        std::string nameOfSet = "default";
        const size_t positionOfSetting = 18;

        LanguageManager* languageManager = gui::GuiManager::getInstance()->getLanguageManager();

        LanguageText* textFullscreen = languageManager->findLanguageString( "full-screen" );
        std::string stringFullscreenSpaced ( textFullscreen->getText() );
        for ( size_t position = stringFullscreenSpaced.length() ; position < positionOfSetting ; ++position ) {
                stringFullscreenSpaced = stringFullscreenSpaced + " ";
        }
        Label* labelFullscreen = new Label( stringFullscreenSpaced + ( gui::GuiManager::getInstance()->isAtFullScreen () ? "yes" : "no" ) );

        LanguageText* textDrawBackground = languageManager->findLanguageString( "draw-background" );
        std::string stringDrawBackgroundSpaced ( textDrawBackground->getText() );
        for ( size_t position = stringDrawBackgroundSpaced.length() ; position < positionOfSetting ; ++position ) {
                stringDrawBackgroundSpaced = stringDrawBackgroundSpaced + " ";
        }
        Label* labelDrawBackground = new Label( stringDrawBackgroundSpaced + ( isomot::GameManager::getInstance()->hasBackgroundPicture () ? "yes" : "no" ) );

        LanguageText* textGraphicSet = languageManager->findLanguageString( "graphic-set" );
        std::string stringGraphicSetSpaced ( textGraphicSet->getText() );
        for ( size_t position = stringGraphicSetSpaced.length() ; position < positionOfSetting ; ++position ) {
                stringGraphicSetSpaced = stringGraphicSetSpaced + " ";
        }
        Label* labelGraphicSet = new Label( stringGraphicSetSpaced + nameOfSet );

        Menu* menu = new Menu( textFullscreen->getX(), textFullscreen->getY() );
        menu->addActiveOption( labelFullscreen );
        menu->addOption( labelDrawBackground );
        menu->addOption( labelGraphicSet );

        screen->addWidget( menu );
        screen->setNext( menu );

        gui::GuiManager::getInstance()->changeScreen( screen );
        gui::GuiManager::getInstance()->refresh();

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
                                        if ( menu->getActiveOption () == labelFullscreen )
                                        {
                                                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                                                doneWithKey = true;
                                        }
                                        else if ( menu->getActiveOption () == labelGraphicSet )
                                        {
                                                // well it’s still something TO DO ...
                                                // now just paint it yellow or cyan
                                                if ( theKey == KEY_LEFT )
                                                {
                                                        labelGraphicSet->changeFontAndColor( labelGraphicSet->getFontName (), "yellow" );
                                                }
                                                else if ( theKey == KEY_RIGHT )
                                                {
                                                        labelGraphicSet->changeFontAndColor( labelGraphicSet->getFontName (), "cyan" );
                                                }

                                                doneWithKey = true;
                                        }
                                        else if ( menu->getActiveOption () == labelDrawBackground )
                                        {
                                                isomot::GameManager::getInstance()->toggleBackgroundPicture ();
                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                {
                                        menu->handleKey ( theKey << 8 );
                                }

                                clear_keybuf();

                                // update labels for "full screen" & "draw background" options here

                                std::string stringFullscreenSpaced ( textFullscreen->getText() );
                                for ( size_t position = stringFullscreenSpaced.length() ; position < positionOfSetting ; ++position ) {
                                        stringFullscreenSpaced = stringFullscreenSpaced + " ";
                                }
                                labelFullscreen->setText( stringFullscreenSpaced + ( gui::GuiManager::getInstance()->isAtFullScreen () ? "yes" : "no" ) );

                                std::string stringDrawBackgroundSpaced ( textDrawBackground->getText() );
                                for ( size_t position = stringDrawBackgroundSpaced.length() ; position < positionOfSetting ; ++position ) {
                                        stringDrawBackgroundSpaced = stringDrawBackgroundSpaced + " ";
                                }
                                labelDrawBackground->setText( stringDrawBackgroundSpaced + ( isomot::GameManager::getInstance()->hasBackgroundPicture () ? "yes" : "no" ) );

                                menu->redraw ();
                        }

                        // No te comas la CPU
                        // Do not eat the CPU
                        sleep( 25 );
                }
        }
}
