
#include "CreateMenuOfGraphicsSets.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateMainMenu.hpp"

#include "sleep.hpp"

using gui::CreateMenuOfGraphicsSets ;


CreateMenuOfGraphicsSets::CreateMenuOfGraphicsSets( Action* previous ) :
        Action( ),
        actionOnEscape( previous ),
        menuOfGraphicsSets( new Menu( ) )
{
        setsOfGraphics[ "gfx" ] = "Present" ;
        setsOfGraphics[ "gfx.2009" ] = "By Davit" ;
        setsOfGraphics[ "gfx.riderx" ] = "By Ricardo" ;
        setsOfGraphics[ "gfx.2003" ] = "Initial By Davit" ;
        setsOfGraphics[ "gfx.simple" ] = "Black & White" ;

        menuOfGraphicsSets->setVerticalOffset( 40 );
}

CreateMenuOfGraphicsSets::~CreateMenuOfGraphicsSets( )
{
        delete menuOfGraphicsSets ;
}

void CreateMenuOfGraphicsSets::doAction ()
{
        const size_t positionOfSecondColumn = 18;

        Screen& screen = * GuiManager::getInstance().findOrCreateScreenForAction( this );

        if ( screen.countWidgets() == 0 )
        {
                screen.setEscapeAction( this->actionOnEscape );

                screen.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                for ( std::map < std::string, std::string >::iterator i = setsOfGraphics.begin (); i != setsOfGraphics.end (); ++ i )
                {
                        std::string nameOfSet = i->second;
                        std::string nameOfSetSpaced ( nameOfSet );

                        for ( size_t position = nameOfSetSpaced.length() ; position < positionOfSecondColumn ; ++position ) {
                                nameOfSetSpaced = nameOfSetSpaced + " ";
                        }

                        Label * theLabel = new Label( nameOfSetSpaced + i->first );
                        if ( i->first != game::GameManager::getInstance().getChosenGraphicsSet() )
                        {
                                theLabel->changeColor( "cyan" );
                        }
                        else
                        {
                                theLabel->changeColor( "yellow" );
                        }

                        menuOfGraphicsSets->addOption( theLabel );
                }

                screen.addWidget( menuOfGraphicsSets );
                screen.setKeyHandler( menuOfGraphicsSets );
        }

        std::list< Label* > labels = menuOfGraphicsSets->getEveryOption ();
        for ( std::list< Label * >::iterator il = labels.begin (); il != labels.end (); ++il )
        {
                if ( ( *il )->getColor() == "yellow" )
                {
                        menuOfGraphicsSets->setActiveOption( *il );
                }
        }

        gui::GuiManager::getInstance().changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( true )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key pressed by the user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                screen.handleKey( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == "Enter" || theKey == "Space" )
                                {
                                        std::string chosenSet = menuOfGraphicsSets->getActiveOption()->getText().substr( positionOfSecondColumn ) ;

                                        if ( chosenSet != game::GameManager::getInstance().getChosenGraphicsSet() )
                                        { // new set is not the same as previous one
                                                game::GameManager::getInstance().setChosenGraphicsSet( chosenSet.c_str () ) ;

                                                gui::GuiManager::getInstance().refreshScreens ();

                                                std::list< Label * > everySet = menuOfGraphicsSets->getEveryOption ();
                                                for ( std::list< Label* >::iterator is = everySet.begin (); is != everySet.end (); ++is )
                                                {
                                                        ( * is )->changeColor( "cyan" );
                                                }
                                                menuOfGraphicsSets->getActiveOption()->changeColor( "yellow" );
                                        }

                                        doneWithKey = true;
                                }

                                if ( ! doneWithKey )
                                {
                                        screen.getKeyHandler()->handleKey ( theKey );
                                }

                                allegro::emptyKeyboardBuffer();
                                menuOfGraphicsSets->redraw ();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        somn::milliSleep( 25 );
                }
        }
}
