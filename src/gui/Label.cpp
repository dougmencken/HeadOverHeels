
#include "Label.hpp"
#include "Color.hpp"
#include "Action.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"

#include <iostream>
#include <cassert>


namespace gui
{

Label::Label( const std::string& text )
: Widget( ),
        imageOfLetters( nilPointer ),
        text( text ),
        fontFamily( "plain" ),
        color( "white" ),
        spacing( 0 ),
        myAction( nilPointer )
{
        createImageOfLabel( text, Label::getFontByFamilyAndColor( fontFamily, color ) );
}

Label::Label( const std::string& text, const std::string& family, const std::string& color, int spacing )
: Widget( ),
        imageOfLetters( nilPointer ),
        text( text ),
        fontFamily( family ),
        color( color ),
        spacing( spacing ),
        myAction( nilPointer )
{
        if ( family.empty() ) this->fontFamily = "plain";
        if ( color.empty() ) this->color = "white";

        createImageOfLabel( text, Label::getFontByFamilyAndColor( family, color ) );
}

Label::~Label( )
{
        delete imageOfLetters ;
}

/* static */
Font* Label::getFontByFamilyAndColor( const std::string& family, const std::string& color )
{
        Font* found = GuiManager::getInstance().findFontByFamilyAndColor( family, color == "multicolor" ? "white" : color ) ;
        assert( found != nilPointer );
        return found;
}

void Label::update()
{
        createImageOfLabel( text, Label::getFontByFamilyAndColor( fontFamily, color ) );
}

void Label::changeFontFamily ( const std::string& family )
{
        this->fontFamily = family;
        update ();
}

void Label::changeColor ( const std::string& color )
{
        this->color = color;
        update ();
}

void Label::changeFontFamilyAndColor( const std::string& family, const std::string& color )
{
        this->fontFamily = family;
        this->color = color;
        update ();
}

void Label::draw( const allegro::Pict& where )
{
        if ( imageOfLetters != nilPointer )
        {
                allegro::drawSprite( where, imageOfLetters->getAllegroPict(), getX(), getY() );
        }
}

void Label::handleKey( const std::string& key )
{
        if ( myAction != nilPointer && ( key == "Enter" || key == "Space" ) )
        {
                myAction->doIt ();
        }
}

void Label::createImageOfLabel( const std::string& text, Font * font )
{
        std::string nameOfImage = "image of label \"" + text + "\" using " + this->fontFamily + " font colored " + this->color ;
        ///std::cout << "creating " << nameOfImage << std::endl ;

        // re-create image of letters
        delete imageOfLetters ;
        imageOfLetters = new Picture( getWidth(), getHeight() );
        imageOfLetters->setName( nameOfImage );

        if ( ! text.empty() )
        {
                const size_t numberOfColors = 3;
                std::string multiColors[ numberOfColors ] = {  "cyan", "yellow", "orange"  }; // sequence of colors for multi~color labels
                /* if ( iso::GameManager::getInstance().isSimpleGraphicSet() )
                        multiColors[ 2 ] = "white"; // original speccy sequence is “ cyan yellow white ” */

                unsigned short cycle = 0; // position in that sequence for character to draw
                Font* fontToUse = font ;

                size_t charPos = 0; // position of character in the string which for utf-8 isn't always the same as character’s offset in bytes

                std::string::const_iterator iter = text.begin ();
                while ( iter != text.end () )
                {
                        if ( this->color == "multicolor" )
                        {
                                // pick new font with color for this letter
                                fontToUse = Label::getFontByFamilyAndColor( font->getFamily(), multiColors[ cycle ] );
                                if ( fontToUse == nilPointer ) fontToUse = font ;

                                // cycle in sequence of colors
                                cycle++;
                                if ( cycle >= numberOfColors ) cycle = 0;
                        }

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
}

bool EqualXYOfLabel::operator() ( const Label* label, std::pair < int, int > thatXY ) const
{
        return ( label->getY () == thatXY.second && label->getX () == thatXY.first );
}

bool EqualTextOfLabel::operator() ( const Label* label, const std::string& text ) const
{
        return ( label->getText() == text );
}

}
