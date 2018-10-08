
#include "CreateMenuOfGraphicSets.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateVideoMenu.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateMenuOfGraphicSets ;


CreateMenuOfGraphicSets::CreateMenuOfGraphicSets( Picture* picture, Action* previous ) :
        Action( picture ),
        actionOnEscape( previous ),
        menuOfGraphicSets( nilPointer )
{
        graphicSets[ "gfx" ] = "Present" ;
        graphicSets[ "gfx.2009" ] = "By Davit" ;
        graphicSets[ "gfx.riderx" ] = "By Ricardo" ;
        graphicSets[ "gfx.2003" ] = "Initial By Davit" ;
        graphicSets[ "gfx.simple" ] = "Black & White" ;

        menuOfGraphicSets = new Menu( );
        menuOfGraphicSets->setVerticalOffset( 40 );
}

CreateMenuOfGraphicSets::~CreateMenuOfGraphicSets( )
{

}

void CreateMenuOfGraphicSets::doAction ()
{
        const size_t positionOfSecondColumn = 18;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );

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
                        if ( i->first != isomot::GameManager::getInstance()->getChosenGraphicSet() )
                        {
                                theLabel->changeColor( "cyan" );
                        }
                        else
                        {
                                theLabel->changeColor( "yellow" );
                        }

                        menuOfGraphicSets->addOption( theLabel );
                }

                screen->addWidget( menuOfGraphicSets );
                screen->setKeyHandler( menuOfGraphicSets );
        }

        std::list< Label* > labels = menuOfGraphicSets->getEveryOption ();
        for ( std::list< Label * >::iterator il = labels.begin (); il != labels.end (); ++il )
        {
                if ( ( *il )->getColor() == "yellow" )
                {
                        menuOfGraphicSets->setActiveOption( *il );
                }
        }

        gui::GuiManager::getInstance()->changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( true )
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

                                if ( theKey == "Enter" )
                                {
                                        std::string chosenSet = menuOfGraphicSets->getActiveOption()->getText().substr( positionOfSecondColumn ) ;

                                        if ( chosenSet != isomot::GameManager::getInstance()->getChosenGraphicSet() )
                                        { // new set is not the same as previous one
                                                isomot::GameManager::getInstance()->setChosenGraphicSet( chosenSet.c_str () ) ;

                                                gui::GuiManager::getInstance()->refreshScreens ();

                                                std::list< Label * > everySet = menuOfGraphicSets->getEveryOption ();
                                                for ( std::list< Label* >::iterator is = everySet.begin (); is != everySet.end (); ++is )
                                                {
                                                        ( * is )->changeColor( "cyan" );
                                                }
                                                menuOfGraphicSets->getActiveOption()->changeColor( "yellow" );
                                        }

                                        doneWithKey = true;
                                }

                                if ( ! doneWithKey )
                                {
                                        screen->getKeyHandler()->handleKey ( theKey );
                                }

                                allegro::emptyKeyboardBuffer();
                                menuOfGraphicSets->redraw ();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        milliSleep( 25 );
                }
        }
}
