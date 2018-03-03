
#include "Label.hpp"
#include "Color.hpp"
#include "Action.hpp"
#include "GuiManager.hpp"
#include "Ism.hpp"
#include <iostream>


namespace gui
{

Label::Label( const std::string& text )
: Widget( ),
        text( text ),
        fontFamily( "regular" ),
        color( "white" ),
        spacing( 0 ),
        buffer( nilPointer ),
        myAction( nilPointer )
{
        createImageOfLabel( text, Label::getFontByFamilyAndColor( fontFamily, color ) );
}

Label::Label( const std::string& text, const std::string& family, const std::string& color, int spacing )
: Widget( ),
        text( text ),
        fontFamily( family ),
        color( color ),
        spacing( spacing ),
        buffer( nilPointer ),
        myAction( nilPointer )
{
        createImageOfLabel( text, Label::getFontByFamilyAndColor( family, color ) );
}

Label::~Label( )
{
        destroy_bitmap( this->buffer );
}

/* static */
Font* Label::getFontByFamilyAndColor( const std::string& family, const std::string& color )
{
        return GuiManager::getInstance()->findFontByFamilyAndColor( family, color == "multicolor" ? "white" : color ) ;
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

void Label::draw( BITMAP* where )
{
        draw_sprite( where, this->buffer, this->getX (), this->getY () );
}

void Label::handleKey( int key )
{
        if ( key >> 8 == KEY_ENTER && myAction != nilPointer )
        {
                myAction->doIt ();
        }
}

BITMAP * Label::createImageOfLabel( const std::string& text, Font * font )
{
        // re-create buffer
        if ( this->buffer != nilPointer )
        {
                destroy_bitmap( this->buffer );
        }

        buffer = create_bitmap_ex( 32, getWidth(), getHeight() );
        clear_to_color( buffer, Color::colorOfTransparency()->toAllegroColor () );

        const size_t numberOfColors = 3;
        std::string multiColors[ numberOfColors ] = {  "cyan", "yellow", "orange"  }; // sequence of colors for multi~color labels
        /* if ( isomot::GameManager::getInstance()->isSimpleGraphicSet() )
                multiColors[ 2 ] = "white"; // original speccy sequence is “ cyan yellow white ” */

        size_t colorIndex( 0 ); // index in that sequence for character to draw
        Font* fontToUse = font ;

        size_t charPos = 0; // position of character in the string which for utf-8 isn't always the same as character’s offset in bytes

        std::string::const_iterator iter = text.begin ();
        while ( iter != text.end () )
        {
                if ( this->color == "multicolor" )
                {
                        // pick new font with color for this letter
                        fontToUse = Label::getFontByFamilyAndColor( font->getFamily(), multiColors[ colorIndex ] );
                        if ( fontToUse == nilPointer )
                        {
                                std::cerr << "can’t get font with family \"" << font->getFamily() << "\"" <<
                                                " for color \"" << multiColors[ colorIndex ] << "\"" << std::endl ;
                                fontToUse = font ;
                        }

                        // cycle in sequence of colors
                        colorIndex++;
                        if ( colorIndex >= numberOfColors ) colorIndex = 0;
                }

                size_t from = std::distance( text.begin (), iter );
                size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // string iterator increments here
                std::string utf8letter = text.substr( from, howMany );

                // draw letter
                blit(
                        fontToUse->getPictureOfLetter( utf8letter ),
                        buffer,
                        0,
                        0,
                        charPos * ( fontToUse->getCharWidth() + spacing ),
                        0,
                        fontToUse->getCharWidth(),
                        fontToUse->getCharHeight()
                );

                charPos ++;
        }

        return this->buffer;
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
