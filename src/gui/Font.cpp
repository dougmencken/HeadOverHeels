
#include "Font.hpp"
#include "Ism.hpp"

#include <iostream>

#include <loadpng.h>


namespace gui
{

Font::Font( const std::string& fontName, const std::string& fontFile, int color, bool doubleSize ) :
        fontName( fontName )
{
        std::string nameOfFile = fontFile;
        std::cout << "reading from file \"" << nameOfFile << "\" to create font \"" << fontName << "\"" << std::endl ;

        BITMAP * bitmapFont = load_png( isomot::pathToFile( fontFile ), 0 );
        if ( bitmapFont == 0 )
        {
                std::cerr << "can’t get font \"" << fontName << "\" from file \"" << nameOfFile << "\"" << std::endl ;
                return;
        }

        // double sized font
        if ( doubleSize )
        {
                BITMAP* bigfont = create_bitmap_ex( bitmap_color_depth( bitmapFont ), bitmapFont->w, bitmapFont->h << 1 );
                stretch_blit( bitmapFont, bigfont, 0, 0, bitmapFont->w, bitmapFont->h, 0, 0, bigfont->w, bigfont->h );
                destroy_bitmap( bitmapFont );
                bitmapFont = bigfont;
        }

        // colorize letters
        if ( color != makecol( 255, 255, 255 ) )
        {
                for ( int x = 0; x < bitmapFont->w; x++ )
                {
                        for ( int y = 0; y < bitmapFont->h; y++ )
                        {
                                if ( ( ( int* )bitmapFont->line[ y ] )[ x ] == makecol( 255, 255, 255 ) )
                                {
                                        ( ( int* )bitmapFont->line[ y ] )[ x ] = color;
                                }
                        }
                }
        }

        // decompose letters
        if ( bitmapFont != 0 )
        {
                this->charWidth = bitmapFont->w / CharactersPerRow;
                this->charHeight = bitmapFont->h / RowsPerBitmapFont;

                for ( int y = 0; y < bitmapFont->h; y += this->charHeight )
                {
                        for ( int x = 0; x < bitmapFont->w; x += this->charWidth )
                        {
                                BITMAP* bitmapChar = create_bitmap_ex( 32, charWidth, charHeight );
                                blit( bitmapFont, bitmapChar, x, y, 0, 0, charWidth, charHeight );
                                charVector.push_back( bitmapChar );
                        }
                }
        }

        destroy_bitmap( bitmapFont );
}

Font::~Font( )
{
        std::for_each( charVector.begin(), charVector.end(), destroy_bitmap );
}

BITMAP* Font::getPictureOfLetter( const std::string& letter )
{
        for ( int i = 0; i < HowManyLetters; i++ )
        {
                if ( letter.compare( gui::letters[ i ] ) == 0 )
                {
                        return charVector.at( i );
                }
        }

        // flow is here because letter isn’t found
        // in such case return '?'
        return charVector.at( '?' - 32 );
}

}
