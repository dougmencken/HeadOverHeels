
#include "Screen.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "Action.hpp"
#include "Label.hpp"
#include "Picture.hpp"


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
        pictureOfHead( 0 ),
        pictureOfHeels( 0 )
{
        refreshBackground ();
        refreshPicturesOfHeadAndHeels ();
}

Screen::~Screen( )
{
        freeWidgets() ;

        destroy_bitmap( backgroundPicture );
}

void Screen::setEscapeAction ( Action * action )
{
        delete escapeAction ;
        escapeAction = action ;
}

void Screen::refreshBackground ()
{
        if ( backgroundPicture != 0 )
        {
                destroy_bitmap( backgroundPicture );
                backgroundPicture = 0;
        }

        BITMAP * picture = load_png( ( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "background.png" ).c_str (), 0 );

        if ( picture != 0 )
                setBackground( picture );
}

void Screen::refreshPicturesOfHeadAndHeels ()
{
        int xHead = 0, yHead = 0;
        bool headOnScreen = false;

        if ( pictureOfHead != 0 )
        {
                xHead = pictureOfHead->getX ();
                yHead = pictureOfHead->getY ();

                if ( pictureOfHead->isOnScreen() )
                {
                        headOnScreen = true;
                        removeWidget( pictureOfHead );
                }
                else
                        delete pictureOfHead ;

                pictureOfHead = 0;
        }
        pictureOfHead = new Picture( xHead, yHead, loadPicture( "gui-head.png" ), "gui:head.png" );

        if ( headOnScreen )
                addWidget( pictureOfHead );

        int xHeels = 0, yHeels = 0;
        bool heelsOnScreen = false;

        if ( pictureOfHeels != 0 )
        {
                xHeels = pictureOfHeels->getX ();
                yHeels = pictureOfHeels->getY ();

                if ( pictureOfHeels->isOnScreen() )
                {
                        heelsOnScreen = true;
                        removeWidget( pictureOfHeels );
                }
                else
                        delete pictureOfHeels ;

                pictureOfHeels = 0;
        }
        pictureOfHeels = new Picture( xHeels, yHeels, loadPicture( "gui-heels.png" ), "gui:heels.png" );

        if ( heelsOnScreen )
                addWidget( pictureOfHeels );
}

void Screen::draw( BITMAP* where )
{
        // fill with color of background
        clear_to_color( where, backgroundColor );

        // draw background, if any
        if ( this->backgroundPicture != 0 )
        {
                blit( backgroundPicture, where, 0, 0, 0, 0, backgroundPicture->w, backgroundPicture->h );
        }

        // draw each component
        std::for_each( widgets.begin (), widgets.end (), std::bind2nd( std::mem_fun( &Widget::draw ), where ) );

        // copy resulting image to screen
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
        widget->setOnScreen( true );
}

bool Screen::removeWidget( Widget* widget )
{
        for ( std::list< Widget* >::const_iterator it = widgets.begin (); it != widgets.end (); ++ it )
        {
                if ( ( *it ) == widget )
                {
                        widgets.remove( widget );
                        delete widget;

                        return true;
                }
        }

        return false;
}

void Screen::freeWidgets ()
{
        if ( pictureOfHead != 0 && ! pictureOfHead->isOnScreen () )
                delete pictureOfHead;

        if ( pictureOfHeels != 0 && ! pictureOfHeels->isOnScreen () )
                delete pictureOfHeels;

        while ( ! this->widgets.empty () )
        {
                Widget* w = *( widgets.begin () );
                widgets.remove( w );
                delete w;
        }

        pictureOfHead = 0;
        pictureOfHeels = 0;
}

void Screen::addPictureOfHeadAt ( int x, int y )
{
        if ( pictureOfHead == 0 )
                pictureOfHead = new Picture( x, y, loadPicture( "gui-head.png" ), "gui:head.png" );
        else
                pictureOfHead->moveTo( x, y );

        addWidget( pictureOfHead );
}

void Screen::addPictureOfHeelsAt ( int x, int y )
{
        if ( pictureOfHeels == 0 )
                pictureOfHeels = new Picture( x, y, loadPicture( "gui-heels.png" ), "gui:heels.png" );
        else
                pictureOfHeels->moveTo( x, y );

        addWidget( pictureOfHeels );
}

void Screen::placeHeadAndHeels( bool picturesToo, bool copyrightsToo )
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

        if ( picturesToo )
        {
                addPictureOfHeadAt( 206, 84 );
                addPictureOfHeelsAt( 378, 84 );
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

/* static */
BITMAP * Screen::loadPicture ( const char * nameOfPicture )
{
        return load_png( ( GuiManager::getInstance()->getPathToPicturesOfGui() + nameOfPicture ).c_str (), 0 );
}

}
