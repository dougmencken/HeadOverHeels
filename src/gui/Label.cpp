
#include "Label.hpp"
#include "Screen.hpp"
#include "FontManager.hpp"
#include "Action.hpp"


namespace gui
{

Label::Label( const std::string& text )
: Widget( ),
        text( text ),
        fontName( "regular" ),
        color( "white" ),
        font( 0 ),
        spacing( 0 ),
        buffer( 0 ),
        myAction( 0 )
{
        this->font = FontManager::getInstance()->findFont( fontName, color );
        this->createBitmapOfLabel( this->text, this->fontName, this->color );
}

Label::Label( const std::string& text, const std::string& fontName, const std::string& color, int spacing )
: Widget( ),
        text( text ),
        fontName( fontName ),
        color( color ),
        font( 0 ),
        spacing( spacing ),
        buffer( 0 )
{
        this->font = FontManager::getInstance()->findFont( fontName, color.compare( "multicolor" ) == 0 ? "white" : color );
        this->createBitmapOfLabel( this->text, this->fontName, this->color );
}

Label::~Label( )
{
        destroy_bitmap( this->buffer );
}

void Label::changeFontAndColor( const std::string& fontName, const std::string& color )
{
        this->fontName = fontName;
        this->color = color;
        this->font = FontManager::getInstance()->findFont( fontName, color.compare( "multicolor" ) == 0 ? "white" : color );
        this->createBitmapOfLabel( this->text, this->fontName, this->color );
}

void Label::draw( BITMAP* where )
{
        draw_sprite( where, this->buffer, this->getX (), this->getY () );
}

void Label::handleKey( int key )
{
        if ( key >> 8 == KEY_ENTER && myAction != 0 )
        {
                myAction->doIt ();
        }
}

BITMAP * Label::createBitmapOfLabel( const std::string& text, const std::string& fontName, const std::string& color )
{
        // re-create buffer
        if ( this->buffer != 0 )
        {
                destroy_bitmap( this->buffer );
        }

        buffer = create_bitmap_ex( 32, getWidth(), getHeight() );
        clear_to_color( buffer, makecol( 255, 0, 255 ) ); // magenta is “ key ” color used as transparency

        const size_t numberOfColors = 3;
        std::string multiColors[ numberOfColors ] = {  "cyan", "yellow", "orange"  }; // sequence of colors for multi~color labels
        size_t colorIndex( 0 ); // index in that sequence for color of character to draw

        size_t charPos = 0; // position of character in the string which for utf-8 isn't always the same as character’s offset in bytes

        std::string::const_iterator iter = text.begin ();
        while ( iter != text.end () )
        {
                if ( color.compare( "multicolor" ) == 0 )
                {
                        // pick a color for this character
                        this->font = FontManager::getInstance()->findFont( fontName, multiColors[ colorIndex ] );
                        assert( this->font );

                        // cycle in the sequence of colors
                        colorIndex++;
                        if ( colorIndex >= numberOfColors ) colorIndex = 0;
                }

                // draw this character
                blit(
                        this->font->getChar( text.substr(
                                std::distance( text.begin (), iter ),
                                incUtf8StringIterator ( iter, text.end () ) // string iterator increments here
                        ) ),
                        buffer,
                        0,
                        0,
                        charPos * ( this->font->getCharWidth() + spacing ),
                        0,
                        this->font->getCharWidth(),
                        this->font->getCharHeight()
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
        return ( label->getText().compare( text ) == 0 );
}

}
