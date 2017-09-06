
#include "Screen.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "Action.hpp"
#include "Label.hpp"
#include "Icon.hpp"


namespace gui
{

Screen::Screen( BITMAP* picture, Action* action ) :
        Widget( 0, 0 ),
        backgroundColor( makecol( 0, 0, 0 ) ),
        backgroundPicture( 0 ),
        where( picture ),
        actionOfScreen( action ),
        escapeAction( 0 ),
        keyHandler( 0 ),
        iconOfHead( 0 ),
        iconOfHeels( 0 )
{
        refreshBackground ();
        refreshIcons ();
}

Screen::~Screen( )
{
        std::for_each( this->widgets.begin (), this->widgets.end (), DeleteObject() );

        delete backgroundPicture ;
        delete iconOfHead ;
        delete iconOfHeels ;
}

void Screen::refreshBackground ()
{
        if ( backgroundPicture != 0 )
        {
                delete backgroundPicture;
                backgroundPicture = 0;
        }

        BITMAP * picture = load_png( ( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "background.png" ).c_str (), 0 );

        if ( picture != 0 )
                setBackground( picture );
}

void Screen::refreshIcons ()
{
        int xHead = 0, yHead = 0;
        if ( iconOfHead != 0 )
        {
                xHead = iconOfHead->getX ();
                yHead = iconOfHead->getY ();

                delete iconOfHead;
                iconOfHead = 0;
        }
        iconOfHead = new Icon( xHead, yHead, load_png( ( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "head.png" ).c_str (), 0 ) );

        int xHeels = 0, yHeels = 0;
        if ( iconOfHeels != 0 )
        {
                xHeels = iconOfHeels->getX ();
                yHeels = iconOfHeels->getY ();

                delete iconOfHeels;
                iconOfHeels = 0;
        }
        iconOfHeels = new Icon( xHeels, yHeels, load_png( ( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "heels.png" ).c_str (), 0 ) );
}

void Screen::draw( BITMAP* where )
{
        // Pinta del color de fondo la imagen destino
        clear_to_color( where, backgroundColor );

        // Si se definió una imagen de fondo entonces se vuelca
        if ( this->backgroundPicture != 0 )
        {
                blit( backgroundPicture, where, 0, 0, 0, 0, backgroundPicture->w, backgroundPicture->h );
        }

        // Dibuja cada componente en la pantalla
        std::for_each( widgets.begin (), widgets.end (), std::bind2nd( std::mem_fun( &Widget::draw ), where ) );

        // Vuelca la imagen destino a la memoria de pantalla
        blit( where, screen, 0, 0, 0, 0, where->w, where->h );
}

void Screen::handleKey( int rawKey )
{
        int theKey = rawKey >> 8;

        if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( theKey == KEY_F ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                return;
        }

        if ( escapeAction != 0 && theKey == KEY_ESC )
        {
                fprintf( stdout, "escape action %s ...\n", ( escapeAction != 0 ? escapeAction->getNameOfAction().c_str () : "nope" ) );
                this->escapeAction->doIt ();
                fprintf( stdout, "... done with action %s\n", ( escapeAction != 0 ? escapeAction->getNameOfAction().c_str () : "nope" ) );
        }
        else
        {
                if ( this->keyHandler != 0 )
                {
                        this->keyHandler->handleKey( rawKey );
                }
        }
}

void Screen::addWidget( Widget* widget )
{
        this->widgets.push_back( widget );
}

void Screen::freeWidgets ()
{
        widgets.clear() ;
}

void Screen::addIconOfHeadAt ( int x, int y )
{
        iconOfHead->moveTo( x, y );
        addWidget( iconOfHead );
}

void Screen::addIconOfHeelsAt ( int x, int y )
{
        iconOfHeels->moveTo( x, y );
        addWidget( iconOfHeels );
}

void Screen::placeHeadAndHeels( bool iconsToo, bool copyrightsToo )
{
        Label* label = 0;

        label = new Label( "Jon", "regular", "multicolor" );
        label->moveTo( 64, 22 );
        addWidget( label );

        label = new Label( "Ritman", "regular", "multicolor" );
        label->moveTo( 40, 52 );
        addWidget( label );

        label = new Label( "Bernie", "regular", "multicolor" );
        label->moveTo( 500, 22 );
        addWidget( label );

        label = new Label( "Drummond", "regular", "multicolor" );
        label->moveTo( 483, 52 );
        addWidget( label );

        label = new Label( "Head", "big", "yellow" );
        label->moveTo( 200, 24 );
        addWidget( label );

        label = new Label( "over", "regular", "multicolor" );
        label->moveTo( 280, 45 );
        addWidget( label );

        label = new Label( "Heels", "big", "yellow" );
        label->moveTo( 360, 24 );
        addWidget( label );

        if ( iconsToo )
        {
                addIconOfHeadAt( 206, 84 );
                addIconOfHeelsAt( 378, 84 );
        }

        if ( copyrightsToo )
        {
                const int whereX = 72;
                const int whereY = 440;
                const int stepY = 28;

                // (c) 1987 Ocean Software Ltd.
                std::string copyrightString ( "{ 1987 Ocean Software Ltd." );
                label = new Label( copyrightString, "regular", "cyan" );
                label->moveTo( whereX, whereY );
                addWidget( label );

                // (c) 2009 Jorge Rodríguez Santos
                copyrightString = "{ 2009 Jorge Rodríguez Santos" ;
                label = new Label( copyrightString, "regular", "orange" );
                label->moveTo( whereX, whereY - stepY );
                addWidget( label );

                // (c) 2017 Douglas Mencken
                copyrightString = "{ 2017 Douglas Mencken" ;
                label = new Label( copyrightString, "regular", "yellow" );
                label->moveTo( whereX, whereY - stepY - stepY );
                addWidget( label );
        }
}

}
