
#include "CreateMenuOfGraphicSets.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateMenuOfGraphicSets ;


CreateMenuOfGraphicSets::CreateMenuOfGraphicSets( BITMAP* picture ) :
        Action(),
        where( picture )
{
        graphicSets[ "gfx" ] = "Present" ;
        graphicSets[ "gfx.2009" ] = "By Davit" ;
        graphicSets[ "gfx.riderx" ] = "By Ricardo" ;
        graphicSets[ "gfx.2003" ] = "Initial By Davit " ;
}

void CreateMenuOfGraphicSets::doIt ()
{
        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( gui::GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateVideoMenu( this->where ) );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ false, /* copyrights */ false );

        std::string nameOfSet = graphicSets[ "gfx" ];

        const size_t positionOfSecondColumn = 15;

        std::string stringGraphicSetSpaced ( "uno" );
        for ( size_t position = stringGraphicSetSpaced.length() ; position < positionOfSecondColumn ; ++position ) {
                stringGraphicSetSpaced = stringGraphicSetSpaced + " ";
        }
        Label* labelGraphicSet = new Label( stringGraphicSetSpaced + nameOfSet );

        Menu* menu = new Menu( 100, 160 );
        menu->addActiveOption( labelGraphicSet );

        screen->addWidget( menu );
        screen->setKeyHandler( menu );

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
                                        if ( menu->getActiveOption () == labelGraphicSet )
                                        {
                                                std::string previousSet( isomot::GameManager::getInstance()->getChosenGraphicSet() );

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

                                                if ( previousSet.compare( isomot::GameManager::getInstance()->getChosenGraphicSet() ) != 0 )
                                                { // new set is not the same as previous one
                                                        gui::GuiManager::getInstance()->reloadImages ();
                                                        sleep( 100 );
                                                }

                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                {
                                        screen->getKeyHandler()->handleKey ( theKey << 8 );
                                }

                                clear_keybuf();
                                menu->redraw ();
                        }

                        // No te comas la CPU
                        // Do not eat the CPU
                        sleep( 25 );
                }
        }
}
