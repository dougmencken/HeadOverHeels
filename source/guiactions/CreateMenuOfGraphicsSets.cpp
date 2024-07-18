
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

void CreateMenuOfGraphicsSets::act ()
{
        const size_t positionOfSecondColumn = 18;

        Screen & slideOfGraphicsSets = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( slideOfGraphicsSets.isNewAndEmpty() )
        {
                slideOfGraphicsSets.setEscapeAction( this->actionOnEscape );

                slideOfGraphicsSets.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                for ( std::map < std::string, std::string >::const_iterator i = setsOfGraphics.begin (); i != setsOfGraphics.end (); ++ i )
                {
                        std::string nameOfSet = i->second;
                        std::string nameOfSetSpaced ( nameOfSet );

                        for ( size_t position = nameOfSetSpaced.length() ; position < positionOfSecondColumn ; ++position ) {
                                nameOfSetSpaced = nameOfSetSpaced + " ";
                        }

                        menuOfGraphicsSets->addOption( new Label (
                                nameOfSetSpaced + i->first,
                                new Font( i->first == GameManager::getInstance().getChosenGraphicsSet() ? "yellow" : "cyan" )
                        ) );
                }

                slideOfGraphicsSets.addWidget( menuOfGraphicsSets );
                slideOfGraphicsSets.setKeyHandler( menuOfGraphicsSets );
        }

        const std::vector< Label* > & sets = menuOfGraphicsSets->getEveryOption ();
        for ( unsigned int i = 0 ; i < sets.size (); ++ i )
        {
                if ( sets[ i ]->getFont().getColor() == "yellow" )
                        menuOfGraphicsSets->setNthOptionAsActive( i );
        }

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), true );

        allegro::emptyKeyboardBuffer();

        while ( true )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key typed by the user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                slideOfGraphicsSets.handleKey( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == "Enter" || theKey == "Space" )
                                {
                                        std::string chosenSet = menuOfGraphicsSets->getActiveOption()->getText().substr( positionOfSecondColumn ) ;

                                        if ( chosenSet != GameManager::getInstance().getChosenGraphicsSet() )
                                        { // the new set is not the same as the previous one
                                                GameManager::getInstance().setChosenGraphicsSet( chosenSet.c_str () ) ;

                                                gui::GuiManager::getInstance().refreshSlides ();

                                                const std::vector< Label * > & everySet = menuOfGraphicsSets->getEveryOption ();
                                                for ( unsigned int i = 0 ; i < everySet.size (); ++ i )
                                                        everySet[ i ]->getFontToChange().setColor( "cyan" );

                                                menuOfGraphicsSets->getActiveOption()->getFontToChange().setColor( "yellow" );
                                        }

                                        doneWithKey = true;
                                }

                                if ( ! doneWithKey )
                                        slideOfGraphicsSets.getKeyHandler()->handleKey( theKey );

                                allegro::emptyKeyboardBuffer();
                                menuOfGraphicsSets->redraw ();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        somn::milliSleep( 25 );
                }
        }
}
