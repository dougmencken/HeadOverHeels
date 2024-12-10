
#include "CreateMenuOfGraphicsSets.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "SlideWithHeadAndHeels.hpp"
#include "Label.hpp"
#include "CreateVideoMenu.hpp"

#include "sleep.hpp"

using gui::CreateMenuOfGraphicsSets ;


CreateMenuOfGraphicsSets::CreateMenuOfGraphicsSets( Action* previous ) :
        ActionWithHandlingKeys( ),
        actionOnEscape( previous ),
        menuOfGraphicsSets( )
{
        setsOfGraphics[ "gfx" ] = "Present" ;
        setsOfGraphics[ "gfx.2009" ] = "By Davit" ;
        setsOfGraphics[ "gfx.riderx" ] = "By Ricardo" ;
        setsOfGraphics[ "gfx.2003" ] = "Initial By Davit" ;
        setsOfGraphics[ "gfx.simple" ] = "Black & White" ;

        menuOfGraphicsSets.setVerticalOffset( 40 );
}

void CreateMenuOfGraphicsSets::act ()
{
        SlideWithHeadAndHeels & slideOfGraphicsSets = GuiManager::getInstance().findOrCreateSlideWithHeadAndHeelsForAction( getNameOfAction() );

        if ( slideOfGraphicsSets.isNewAndEmpty() )
        {
                slideOfGraphicsSets.setEscapeAction( this->actionOnEscape );

                slideOfGraphicsSets.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                for ( std::map < std::string, std::string >::const_iterator i = setsOfGraphics.begin (); i != setsOfGraphics.end (); ++ i )
                {
                        std::string nameOfSet = i->second;
                        std::string nameOfSetSpaced ( firstLetterOfSetNames, ' ' );
                        nameOfSetSpaced.replace( 0, nameOfSet.length(), nameOfSet );

                        Label * option = menuOfGraphicsSets.addOptionWithText( nameOfSetSpaced + i->first );
                        option->getFontToChange().setColor( "cyan" );

                        if ( i->first == GameManager::getInstance().getChosenGraphicsSet() ) {
                                option->getFontToChange().setColor( "yellow" );
                                menuOfGraphicsSets.setActiveOptionByText( option->getText() );
                        }
                }

                slideOfGraphicsSets.addWidget( & menuOfGraphicsSets );
        }

        slideOfGraphicsSets.setKeyHandler( this );

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

void CreateMenuOfGraphicsSets::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( theKey == "Enter" || theKey == "Space" )
        {
                std::string chosenSet = menuOfGraphicsSets.getActiveOption()->getText().substr( firstLetterOfSetNames ) ;

                if ( chosenSet != GameManager::getInstance().getChosenGraphicsSet() )
                { // the new set is not the same as the previous one
                        GameManager::getInstance().setChosenGraphicsSet( chosenSet.c_str () ) ;

                        gui::GuiManager::getInstance().refreshSlides ();

                        const std::vector< Label * > & everySet = menuOfGraphicsSets.getEveryOption ();
                        for ( unsigned int i = 0 ; i < everySet.size (); ++ i )
                                everySet[ i ]->getFontToChange().setColor( "cyan" );

                        menuOfGraphicsSets.getActiveOption()->getFontToChange().setColor( "yellow" );
                }

                doneWithKey = true ;
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                this->menuOfGraphicsSets.redraw ();
        } else
                this->menuOfGraphicsSets.handleKey( theKey ) ;
}
