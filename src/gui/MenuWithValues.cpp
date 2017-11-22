
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "Ism.hpp"


namespace gui
{

MenuWithValues::MenuWithValues( char fill, unsigned short minSpaces )
        : Menu( )
        , symbolToFill( fill )
        , minSpacesBeforeValue( minSpaces )
{

}

MenuWithValues::~MenuWithValues( )
{
        listOfValues.clear();
}

void MenuWithValues::draw( BITMAP* where )
{
        if ( where == 0 ) return ;

        if ( where != this->whereToDraw )
        {
                this->whereToDraw = where;
        }

        if ( activeOption == 0 )
        {
                resetActiveOption ();
        }

        unsigned int maxLetters = 0;
        unsigned int maxPixels = 0;

        // get longest option
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* option = *i;
                if ( option->getWidth() > maxPixels )
                {
                        maxPixels = option->getWidth();
                        maxLetters = utf8StringLength( option->getText() );
                }
        }

        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* option = *i;

                if ( option == this->activeOption )
                {
                        if ( option->getFontName() != "big" )
                                option->changeFontAndColor( "big", option->getColor() );
                }
                else
                {
                        if ( option->getFontName() != "regular" )
                                option->changeFontAndColor( "regular", option->getColor() );
                }
        }

        std::list< Label * > optionsWithValues;
        Label* activeOptionWithValue = 0;

        for ( std::list< Label* >::const_iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* option = *i;

                std::string textOfOption = option->getText() ;
                unsigned int lengthOfText = utf8StringLength( textOfOption );
                for ( unsigned int off = lengthOfText; off < maxLetters + minSpacesBeforeValue; off++ )
                {
                        textOfOption = textOfOption + symbolToFill;
                }

                Label* optionWithValue = new Label( textOfOption + getValueOf( option ), option->getFontName(), option->getColor(), option->getSpacing() );
                optionsWithValues.push_back( optionWithValue );

                if ( option == this->activeOption )
                        activeOptionWithValue = optionWithValue;
        }

        // condense options of menu so they fit on screen
        // hey but how about alignment, need to condense every option to the same as for most condensed one
        /* for ( std::list< Label* >::iterator o = optionsWithValues.begin (); o != optionsWithValues.end (); ++o )
        {
                Label* option = *o;
                while ( option->getWidth() > ( isomot::ScreenWidth - 100 ) )
                        option->setSpacing( option->getSpacing() - 1 );
        } */

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( isomot::ScreenWidth - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( isomot::ScreenHeight - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( this->optionImage != 0 ? this->optionImage->w : 0 );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( std::list< Label* >::iterator i = optionsWithValues.begin (); i != optionsWithValues.end (); ++i )
        {
                Label* label = *i;

                BITMAP * mark = ( activeOptionWithValue == label ) ? this->chosenOptionImage : this->optionImage ;
                if ( mark != 0 )
                        draw_sprite( where, mark, getX (), getY () + dy );

                label->moveTo( getX () + dx, getY () + dy );
                label->draw( where );

                // update vertical offset
                dy += label->getHeight() - ( label == activeOptionWithValue ? 8 : 4 );

                // adjust spacing between lines
                dy -= label->getHeight() >> 5;
        }

        // back to initial position of menu
        setX ( previousX );
        setY ( previousY );

        std::for_each( optionsWithValues.begin (), optionsWithValues.end (), isomot::DeleteObject() );
        optionsWithValues.clear();
}

std::string MenuWithValues::getValueOf( Label* option ) const
{
        size_t numberOf = 0;
        for ( std::list< Label* >::const_iterator o = options.begin (); o != options.end (); ++o, numberOf++ )
        {
                if ( ( *o ) == option )
                {
                        if ( numberOf >= listOfValues.size() )
                                return "(o)";

                        return listOfValues[ numberOf ] ;
                }
        }

        return "";
}

void MenuWithValues::setValueOf( Label* option, std::string value )
{
        while ( listOfValues.size() < options.size() )
        {
                listOfValues.push_back( "" );
        }

        size_t numberOf = 0;
        for ( std::list< Label* >::const_iterator o = options.begin (); o != options.end (); ++o, numberOf++ )
        {
                if ( ( *o ) == option )
                {
                        listOfValues[ numberOf ] = value ;
                        break;
                }
        }
}

unsigned int MenuWithValues::getWidthOfMenu () const
{
        unsigned int widthOfMenu = 0;

        unsigned int maxLetters = 0;
        for ( std::list< Label* >::const_iterator it = options.begin (); it != options.end (); ++it )
        {
                unsigned int length = utf8StringLength( ( *it )->getText() );
                if ( length > maxLetters ) maxLetters = length;
        }

        for ( std::list< Label * >::const_iterator o = options.begin () ; o != options.end () ; ++o )
        {
                std::string textOfOption = ( *o )->getText();
                for ( unsigned int i = utf8StringLength( textOfOption ); i < maxLetters + minSpacesBeforeValue; i++ )
                        textOfOption = textOfOption + symbolToFill;

                Label optionWithValue( textOfOption + getValueOf( *o ), ( *o )->getFontName(), ( *o )->getColor(), ( *o )->getSpacing() );
                unsigned int theWidth = optionWithValue.getWidth() + ( this->optionImage != 0 ? this->optionImage->w : 0 ) ;
                if ( theWidth > widthOfMenu ) widthOfMenu = theWidth ;
        }

        return widthOfMenu;
}

void MenuWithValues::previousOption ()
{
        for ( std::list< Label* >::const_iterator o = options.begin (); o != options.end (); ++o )
        {  // compare by text of option here
                if ( ( *o )->getText() == activeOption->getText() )
                {
                        activeOption = ( o == options.begin() ? *( --options.end() ) : *( --o ) );
                        break;
                }
        }
}

void MenuWithValues::nextOption ()
{
        for ( std::list< Label* >::const_iterator o = options.begin (); o != options.end (); ++o )
        {  // compare by text of option here
                if ( ( *o )->getText() == activeOption->getText() )
                {
                        ++o ;
                        activeOption = ( o == options.end() ? *options.begin() : *o );
                        break;
                }
        }
}

}
