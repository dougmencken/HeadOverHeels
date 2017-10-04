
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
        graphicSets[ "gfx.2003" ] = "Initial By Davit" ;

        menuOfGraphicSets = new Menu( );
        menuOfGraphicSets->setVerticalOffset( 50 );
}

void CreateMenuOfGraphicSets::doAction ()
{
        const size_t positionOfSecondColumn = 18;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );

        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( this->actionOnEscape );

                screen->placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                for ( std::map < std::string, std::string >::iterator i = graphicSets.begin (); i != graphicSets.end (); ++i )
                {
                        std::string nameOfSet = i->second;
                        std::string nameOfSetSpaced ( nameOfSet );

                        for ( size_t position = nameOfSetSpaced.length() ; position < positionOfSecondColumn ; ++position ) {
                                nameOfSetSpaced = nameOfSetSpaced + " ";
                        }

                        Label * theLabel = new Label( nameOfSetSpaced + i->first );
                        if ( i->first.compare( isomot::GameManager::getInstance()->getChosenGraphicSet() ) != 0 )
                        {
                                theLabel->changeFontAndColor( theLabel->getFontName (), "cyan" );
                        }
                        else
                        {
                                theLabel->changeFontAndColor( theLabel->getFontName (), "yellow" );
                        }

                        menuOfGraphicSets->addOption( theLabel );
                }

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

                                if ( theKey == KEY_ENTER )
                                {
                                        std::string chosenSet = menuOfGraphicSets->getActiveOption()->getText().substr( positionOfSecondColumn ) ;

                                        if ( chosenSet.compare( isomot::GameManager::getInstance()->getChosenGraphicSet() ) != 0 )
                                        { // new set is not the same as previous one
                                                isomot::GameManager::getInstance()->setChosenGraphicSet( chosenSet.c_str () ) ;

                                                gui::GuiManager::getInstance()->refreshScreens ();

                                                std::list < Label * > everySet = menuOfGraphicSets->getEveryOption ();
                                                for ( std::list < Label * >::iterator i = everySet.begin (); i != everySet.end (); ++i )
                                                {
                                                        ( * i )->changeFontAndColor( ( * i )->getFontName (), "cyan" );
                                                }
                                                menuOfGraphicSets->getActiveOption()->changeFontAndColor( menuOfGraphicSets->getActiveOption()->getFontName (), "yellow" );
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
