
#include "MenuWithValues.hpp"

#include "GamePreferences.hpp"
#include "Label.hpp"
#include "UnicodeUtilities.hpp"

#include <algorithm> // std::for_each


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

void MenuWithValues::draw ()
{
        if ( getActiveOption() == nilPointer ) resetActiveOption ();

        const std::vector< Label* > & options = getEveryOption ();

        // get the longest option
        unsigned int maxLetters = 0;
        unsigned int maxPixels = 0;

        for ( std::vector< Label* >::const_iterator it = options.begin (); it != options.end (); ++ it )
        {
                Label* option = *it ;
                if ( option->getWidth() > maxPixels )
                {
                        maxPixels = option->getWidth();
                        maxLetters = utf8StringLength( option->getText() );
                }
        }

        // the chosen option’s font is double-height stretched
        for ( unsigned int o = 0 ; o < options.size (); ++ o )
        {
                Label* option = options[ o ] ;
                option->getFontToChange().setDoubleHeight( option == getActiveOption() ) ;
        }

        std::vector< Label * > optionsWithValues ;
        Label* activeOptionWithValue = nilPointer ;

        for ( unsigned int o = 0 ; o < options.size (); ++ o )
        {
                Label* option = options[ o ] ;
                const std::string & textOfOption = option->getText() ;

                std::string filler ;
                unsigned int lengthOfText = utf8StringLength( textOfOption );
                for ( unsigned int off = lengthOfText; off < maxLetters + minSpacesBeforeValue; off++ )
                        filler += symbolToFill ;

                std::string withValue = textOfOption + filler + getValueOf( textOfOption );
                optionsWithValues.push_back( new Label( withValue, new Font( option->getFont() ), option->getSpacing() ) );

                if ( option == getActiveOption() )
                        activeOptionWithValue = optionsWithValues.back() ;
        }

        // condense menu options so they fit on the screen
        // hey but how about alignment, need to condense every option to the same as for the most condensed one
        /* for ( std::vector< Label* >::const_iterator o = optionsWithValues.begin (); o != optionsWithValues.end (); ++ o )
        {
                Label* option = *o ;
                while ( option->getWidth() > ( GamePreferences::getScreenWidth() - 100 ) )
                        option->setSpacing( option->getSpacing() - 1 );
        } */

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( GamePreferences::getScreenWidth() - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( GamePreferences::getScreenHeight() - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( Menu::getPictureBeforeOption().getWidth () );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( unsigned int i = 0 ; i < optionsWithValues.size() ; ++ i )
        {
                Label* label = optionsWithValues[ i ];

                const Picture & mark = ( activeOptionWithValue == label ) ? Menu::getPictureBeforeChosenOption() : Menu::getPictureBeforeOption() ;
                allegro::drawSprite( mark.getAllegroPict(), getX (), getY () + dy );

                label->moveTo( getX () + dx, getY () + dy );
                label->draw ();

                // update vertical offset
                dy += label->getHeight() - ( label == activeOptionWithValue ? 8 : 4 );

                // adjust spacing between lines
                dy -= label->getHeight() >> 5;
        }

        // back to initial position of menu
        setX ( previousX );
        setY ( previousY );

        std::for_each( optionsWithValues.begin (), optionsWithValues.end (), DeleteIt() );
        optionsWithValues.clear();
}

std::string MenuWithValues::getValueOf( const std::string & textOfOption ) const
{
        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int row = 0 ; row < options.size (); ++ row )
        {
                if ( options[ row ]->getText() == textOfOption )
                {
                        assert( row < listOfValues.size() );

                        return listOfValues[ row ] ;
                }
        }

        return "";
}

void MenuWithValues::setValueOf( const Label * const option, const std::string & value )
{
        while ( listOfValues.size() < getEveryOption().size() ) // equalize sizes
                listOfValues.push_back( "" );

        const std::vector< Label* > & options = getEveryOption ();
        for ( unsigned int o = 0 ; o < options.size () ; ++ o )
        {
                if ( options[ o ] == option )
                {
                        listOfValues[ o ] = value ;
                        break;
                }
        }
}

unsigned int MenuWithValues::getWidthOfMenu () const
{
        const std::vector< Label* > & options = getEveryOption ();

        // at first get length of the longest option
        unsigned int maxLetters = 0 ;
        for ( std::vector< Label* >::const_iterator o = options.begin (); o != options.end (); ++ o )
        {
                unsigned int length = utf8StringLength( ( *o )->getText() );
                if ( length > maxLetters ) maxLetters = length;
        }

        unsigned int widthOfMenu = 0 ;

        for ( unsigned int o = 0 ; o < options.size () ; ++ o )
        {
                Label* option = options[ o ];
                const std::string & textOfOption = option->getText();
                std::string filler ;
                for ( unsigned int i = utf8StringLength( textOfOption ); i < maxLetters + minSpacesBeforeValue ; i++ )
                        filler += symbolToFill ;

                Label optionWithValue( textOfOption + filler + getValueOf( textOfOption ), new Font( option->getFont() ), option->getSpacing() );

                unsigned int theWidth = optionWithValue.getWidth() + Menu::getPictureBeforeOption().getWidth() ;
                if ( theWidth > widthOfMenu ) widthOfMenu = theWidth ;
        }

        return widthOfMenu ;
}

unsigned int MenuWithValues::getIndexOfActiveOption () const
{
        if ( getActiveOption() == nilPointer ) return 0 ;

        const std::vector< Label* > & options = getEveryOption ();
        unsigned int o = 0 ;
        for ( ; o < options.size (); ++ o )
                // compare by the text of option here
                if ( options[ o ]->getText() == getActiveOption()->getText() )
                        return o ;

        // if can't find the active menu option
        return 0 ;
}

}
