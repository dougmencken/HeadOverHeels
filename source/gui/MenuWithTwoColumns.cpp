
#include "MenuWithTwoColumns.hpp"

#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "GamePreferences.hpp"
#include "Label.hpp"
#include "Font.hpp"


namespace gui
{

MenuWithTwoColumns::MenuWithTwoColumns( unsigned int space )
        : Menu( )
        , spaceBetweenColumns( space )
{

}

MenuWithTwoColumns::~MenuWithTwoColumns( )
{

}

void MenuWithTwoColumns::draw ()
{
        if ( getActiveOption() == nilPointer ) resetActiveOption ();

        const std::vector< Label* > & options = getEveryOption ();

        unsigned int lastInTheFirstColumn = lastRowInTheFirstColumn() ;

        const unsigned int prepictureWidth = Menu::getPictureBeforeOption().getWidth ();

        // calculate position of the second column
        unsigned int widthOfTheFirstColumn = 0 ;
        for ( unsigned int row = 0 ; row < options.size (); ++ row )
        {
                if ( row <= lastInTheFirstColumn )
                {
                        unsigned int theWidth = options[ row ]->getWidth() + prepictureWidth ;
                        if ( theWidth > widthOfTheFirstColumn ) widthOfTheFirstColumn = theWidth ;
                }
        }
        unsigned int secondColumnX = widthOfTheFirstColumn + this->spaceBetweenColumns ;

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( GamePreferences::getScreenWidth() - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( GamePreferences::getScreenHeight() - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( prepictureWidth );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( unsigned int row = 0 ; row < options.size (); ++ row )
        {
                Label* label = options[ row ] ;

                // pick the color of text
                if ( label == getActiveOption() )
                {
                        if ( label->getFont().getColor() != "orange" )
                                label->changeColor( "orange" );
                }
                else
                {
                        if ( label->getFont().getColor() != "white" )
                                label->changeColor( "white" );
                }

                // reset vertical position for the second column’s first option
                if ( row == 1 + lastInTheFirstColumn ) dy = 0 ;

                int columnX = ( row > lastInTheFirstColumn ) ? secondColumnX : 0 ;

                {       // dibuja la viñeta
                        // para cada etiqueta
                        const Picture & vignette = ( getActiveOption() == label )
                                                        ? Menu::getPictureBeforeChosenOptionSingle()
                                                        : Menu::getPictureBeforeOption() ;

                        allegro::drawSprite( vignette.getAllegroPict(), getX() + columnX, getY() + dy );
                }

                label->moveTo( getX() + dx + columnX, getY() + dy );
                label->draw ();

                // apply the vertical offset
                dy += label->getHeight() - 4 ;

                // adjust spacing between lines
                dy -= label->getHeight() >> 5 ;
        }

        // back to the initial location
        setX ( previousX );
        setY ( previousY );
}

unsigned int MenuWithTwoColumns::getWidthOfMenu () const
{
        unsigned int widthOfTheFirstColumn = 0 ;
        unsigned int widthOfTheSecondColumn = 0 ;

        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int row = 0 ; row < options.size (); ++ row )
        {
                unsigned int theWidth = options[ row ]->getWidth() + Menu::getPictureBeforeOption().getWidth() ;

                if ( row <= lastRowInTheFirstColumn () )
                {
                        if ( theWidth > widthOfTheFirstColumn )
                                widthOfTheFirstColumn = theWidth ;
                }
                else
                {
                        if ( theWidth > widthOfTheSecondColumn )
                                widthOfTheSecondColumn = theWidth ;
                }
        }

        return widthOfTheFirstColumn + this->spaceBetweenColumns + widthOfTheSecondColumn ;
}

unsigned int MenuWithTwoColumns::getHeightOfMenu () const
{
        unsigned int heightOfMenu = 0 ;

        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int row = 0 ; row < options.size (); ++ row )
        {
                if ( row <= lastRowInTheFirstColumn () )
                {
                        unsigned int heightOfOption = options[ row ]->getHeight() ;
                        heightOfMenu += heightOfOption - 4 ;
                        heightOfMenu -= heightOfOption >> 5 ;
                }
        }

        return heightOfMenu ;
}

void MenuWithTwoColumns::handleKey ( const std::string & key )
{
        Menu::handleKey( key );

        if ( getActiveOption() == nilPointer ) return ;

        const std::string & activeOptionText = getActiveOption()->getText() ;

        if ( ( key == "Right" || key == "p" )
                        && isInTheFirstColumn( activeOptionText ) )
        {
                const std::vector< Label* > & options = getEveryOption ();
                unsigned int lastRow = lastRowInTheFirstColumn() ;
                for ( unsigned int row = 0 ; row <= lastRow ; ++ row )
                {
                        if ( options[ row ]->getText() == activeOptionText ) {
                                unsigned int rowInTheSecondColumn = row + lastRow + 1 ;
                                if ( rowInTheSecondColumn < options.size () )
                                        setNthOptionAsActive( rowInTheSecondColumn );
                                break ;
                        }
                }
        }
        else if ( ( key == "Left" || key == "o" )
                        && isInTheSecondColumn( activeOptionText ) )
        {
                const std::vector< Label* > & options = getEveryOption ();
                unsigned int firstRow = lastRowInTheFirstColumn() + 1 ;
                for ( unsigned int row = firstRow ; row < options.size () ; ++ row )
                {
                        if ( options[ row ]->getText() == activeOptionText ) {
                                int rowInTheFirstColumn = row - firstRow ;
                                if ( rowInTheFirstColumn >= 0 )
                                        setNthOptionAsActive( rowInTheFirstColumn );
                                break ;
                        }
                }
        }
}

unsigned int MenuWithTwoColumns::lastRowInTheFirstColumn () const
{
        return getEveryOption().size () >> 1 ;
}

bool MenuWithTwoColumns::isInTheFirstColumn( const std::string & option ) const
{
        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int row = 0 ; row <= lastRowInTheFirstColumn() ; ++ row )
        {
                if ( options[ row ]->getText() == option ) return true ;
        }

        return false ;
}

bool MenuWithTwoColumns::isInTheSecondColumn( const std::string & option ) const
{
        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int row = 1 + lastRowInTheFirstColumn() ; row < options.size () ; ++ row )
        {
                if ( options[ row ]->getText() == option ) return true ;
        }

        return false ;
}

}
