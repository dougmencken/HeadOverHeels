
#include "ColorCyclingLabel.hpp"
#include "Color.hpp"

#include <iostream>


namespace gui
{

ColorCyclingLabel::ColorCyclingLabel( const std::string & text, const std::string & family, int spacing )
        : Label( text, family, "white", spacing )
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
        updateImageOfLabel( getText() );

        Label::draw ();
}

void ColorCyclingLabel::updateImageOfLabel( const std::string & text )
{
        if ( colorCyclingTimer->getValue() < 0.2 && imageOfLetters != nilPointer ) return ;

        delete imageOfLetters;

        Font & font = Label::fontByFamilyAndColor( getFontFamily(), getColor() );
        unsigned int howManyLetters = utf8StringLength( text );
        imageOfLetters = new Picture( howManyLetters * ( font.getWidthOfLetter( "O" ) + getSpacing() ), font.getHeightOfLetter( "I" ) );

        if ( ! text.empty() )
        {
                const size_t numberOfColors = 3;
                const std::string cyclingColors[ numberOfColors ] = {  "cyan", "magenta", "green"  }; // original speccy sequence

                Font & fontToUse = font ;

                size_t letterInString = 0 ;

                std::string::const_iterator iter = text.begin ();
                while ( iter != text.end () )
                {
                        // pick new font with color for this letter
                        fontToUse = Label::fontByFamilyAndColor( font.getFamily(), cyclingColors[ cycle ] );

                        // cycle in the sequence of colors
                        ++ cycle ;
                        if ( cycle >= numberOfColors ) cycle = 0 ;

                        size_t from = std::distance( text.begin (), iter );
                        size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // string iterator increments here
                        std::string utf8letter = text.substr( from, howMany );

                        // draw letter
                        Picture * letter = fontToUse.getPictureOfLetter( utf8letter ) ;
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
