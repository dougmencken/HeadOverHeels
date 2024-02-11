
#include "ColorCyclingLabel.hpp"

#include "Color.hpp"
#include "UnicodeUtilities.hpp"

#include <iostream>


namespace gui
{

ColorCyclingLabel::ColorCyclingLabel( const std::string & text, bool doubleHeight, int spacing )
        : Label( text, Font::fontByNameAndColor( doubleHeight ? "big" : "", "white" ), spacing )
        , cycle( 0 )
        , colorCyclingTimer( new Timer () )
{
        colorCyclingTimer->go();
}

ColorCyclingLabel::~ColorCyclingLabel( )
{
}

void ColorCyclingLabel::draw ()
{
        updateImageOfLabel() ;

        Label::draw ();
}

void ColorCyclingLabel::updateImageOfLabel ()
{
        if ( colorCyclingTimer->getValue() < 0.2 && imageOfLetters != nilPointer ) return ;

        delete imageOfLetters ;

        const std::string & text = getText() ;
        unsigned int howManyLetters = utf8StringLength( text );
        const Font & font = getFont() ;

        imageOfLetters = new Picture( howManyLetters * ( font.getWidthOfLetter( "O" ) + getSpacing() ), font.getHeightOfLetter( "I" ) );

        if ( ! text.empty() )
        {
                const size_t numberOfColors = 3;
                const std::string cyclingColors[ numberOfColors ] = {  "cyan", "magenta", "green"  }; // original speccy sequence

                std::string fontColor = font.getColor() ;

                size_t letterInString = 0 ;

                std::string::const_iterator iter = text.begin ();
                while ( iter != text.end () )
                {
                        fontColor = cyclingColors[ cycle ] ;

                        // cycle in the sequence of colors
                        ++ cycle ;
                        if ( cycle >= numberOfColors ) cycle = 0 ;

                        size_t from = std::distance( text.begin (), iter );
                        size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // string iterator increments here
                        std::string utf8letter = text.substr( from, howMany );

                        // draw letter
                        Picture * letter = Font::fontByNameAndColor( font.getName(), fontColor ).getPictureOfLetter( utf8letter ) ;
                        if ( letter != nilPointer )
                                allegro::bitBlit(
                                        letter->getAllegroPict(),
                                        imageOfLetters->getAllegroPict(),
                                        0,
                                        0,
                                        letterInString * ( letter->getWidth() + getSpacing() ),
                                        0,
                                        letter->getWidth(),
                                        letter->getHeight()
                                );

                        letterInString ++ ;
                }
        }

        colorCyclingTimer->reset ();
}

}
