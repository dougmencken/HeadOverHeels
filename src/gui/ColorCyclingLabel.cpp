
#include "ColorCyclingLabel.hpp"
#include "Color.hpp"

#include <iostream>


namespace gui
{

ColorCyclingLabel::ColorCyclingLabel( const std::string& text, const std::string& family, int spacing )
        : Label( text, family, "white", spacing )
        , colorCyclingTimer( nilPointer )
        , cycle( 0 )
{
        colorCyclingTimer = new Timer ();
        colorCyclingTimer->go();
}

ColorCyclingLabel::~ColorCyclingLabel( )
{
        delete colorCyclingTimer ;
}

void ColorCyclingLabel::draw( allegro::Pict* where )
{
        updateImageOfLabel( getText(), Label::getFontByFamilyAndColor( getFontFamily(), getColor() ) );

        Label::draw( where );
}

void ColorCyclingLabel::updateImageOfLabel( const std::string& text, Font * font )
{
        if ( colorCyclingTimer->getValue() > 0.2 || imageOfLetters == nilPointer )
        {
                delete imageOfLetters;
                imageOfLetters = new Picture( getWidth(), getHeight() );

                if ( ! text.empty() )
                {
                        const size_t numberOfColors = 3;
                        const std::string cyclingColors[ numberOfColors ] = {  "cyan", "magenta", "green"  }; // original speccy sequence

                        Font* fontToUse = font ;

                        size_t charPos = 0; // position of character in the string which for utf-8 isn't always the same as character’s offset in bytes

                        std::string::const_iterator iter = text.begin ();
                        while ( iter != text.end () )
                        {
                                // pick new font with color for this letter
                                fontToUse = Label::getFontByFamilyAndColor( font->getFamily(), cyclingColors[ cycle ] );
                                if ( fontToUse == nilPointer )
                                {
                                        std::cerr << "can’t get font with family \"" << font->getFamily() << "\"" <<
                                                        " for color \"" << cyclingColors[ cycle ] << "\"" << std::endl ;
                                        fontToUse = font ;
                                }

                                // cycle in sequence of colors
                                cycle++ ;
                                if ( cycle >= numberOfColors ) cycle = 0;

                                size_t from = std::distance( text.begin (), iter );
                                size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // string iterator increments here
                                std::string utf8letter = text.substr( from, howMany );

                                // draw letter
                                allegro::bitBlit(
                                        fontToUse->getPictureOfLetter( utf8letter )->getAllegroPict(),
                                        imageOfLetters->getAllegroPict(),
                                        0,
                                        0,
                                        charPos * ( fontToUse->getCharWidth() + getSpacing() ),
                                        0,
                                        fontToUse->getCharWidth(),
                                        fontToUse->getCharHeight()
                                );

                                charPos ++;
                        }
                }

                colorCyclingTimer->reset ();
        }
}

}
