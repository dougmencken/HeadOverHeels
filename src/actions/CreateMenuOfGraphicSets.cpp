
#include "CreateMenuOfGraphicSets.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateMenuOfGraphicSets ;


CreateMenuOfGraphicSets::CreateMenuOfGraphicSets( BITMAP* picture, Action* previous ) :
        Action(),
        where( picture ),
        actionOnEscape( previous ),
        menuOfGraphicSets( 0 )
{
        graphicSets[ "gfx" ] = "Present" ;
        graphicSets[ "gfx.2009" ] = "By Davit" ;
        graphicSets[ "gfx.riderx" ] = "By Ricardo" ;
        graphicSets[ "gfx.2003" ] = "Initial By Davit " ;
}

void CreateMenuOfGraphicSets::doIt ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 || menuOfGraphicSets == 0 )
        {
                screen->setBackground( gui::GuiManager::getInstance()->findImage( "background" ) );
                screen->setEscapeAction( this->actionOnEscape );

                CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

                std::string nameOfSet = graphicSets[ "gfx" ];

                const size_t positionOfSecondColumn = 15;

                std::string stringGraphicSetSpaced ( "uno" );
                for ( size_t position = stringGraphicSetSpaced.length() ; position < positionOfSecondColumn ; ++position ) {
                        stringGraphicSetSpaced = stringGraphicSetSpaced + " ";
                }
                Label* labelGraphicSet = new Label( stringGraphicSetSpaced + nameOfSet );

                this->menuOfGraphicSets = new Menu( 100, 160 );
                menuOfGraphicSets->addOption( labelGraphicSet );

                screen->addWidget( menuOfGraphicSets );
                screen->setKeyHandler( menuOfGraphicSets );
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
                                        Label* activeSet = menuOfGraphicSets->getActiveOption() ;

                                        std::string previousSet( isomot::GameManager::getInstance()->getChosenGraphicSet() );

                                        // well it’s still something TO DO ...
                                        // now just paint it yellow or cyan
                                        if ( theKey == KEY_LEFT )
                                        {
                                                activeSet->changeFontAndColor( activeSet->getFontName (), "yellow" );
                                        }
                                        else if ( theKey == KEY_RIGHT )
                                        {
                                                activeSet->changeFontAndColor( activeSet->getFontName (), "cyan" );
                                        }

                                        if ( previousSet.compare( isomot::GameManager::getInstance()->getChosenGraphicSet() ) != 0 )
                                        { // new set is not the same as previous one
                                                gui::GuiManager::getInstance()->reloadImages ();
                                                sleep( 100 );
                                        }

                                        doneWithKey = true;
                                }

                                if ( ! doneWithKey )
                                {
                                        screen->getKeyHandler()->handleKey ( theKey << 8 );
                                }

                                clear_keybuf();
                                menuOfGraphicSets->redraw ();
                        }

                        // No te comas la CPU
                        // Do not eat the CPU
                        sleep( 25 );
                }
        }
}
