
#include "Font.hpp"
#include "Ism.hpp"

#include <iostream>

#include <loadpng.h>


namespace gui
{

/* static */ unsigned int Font::howManyLetters = 0 ;

/* static */ std::string * Font::tableOfLetters = 0 ;


Font::Font( const std::string& fontName, const std::string& fontFile, int color, bool doubleHeight ) :
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

        // double height font
        if ( doubleHeight )
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

        // read table of letters once for all fonts
        if ( tableOfLetters == 0 )
        {
                howManyLetters = 0;

                std::string file = isomot::sharePath() + "letters.utf8";
                std::ifstream lettersFile ( isomot::pathToFile( file ), std::ifstream::binary );
                if ( lettersFile != 0 )
                {
                        lettersFile.seekg( 0, lettersFile.end );
                        unsigned int length = lettersFile.tellg();
                        lettersFile.seekg( 0, lettersFile.beg );

                        char * buffer = new char [ length ];
                        lettersFile.read( buffer, length );
                        lettersFile.close ();

                        for ( unsigned int at = 0 ; at < length ; at ++ )
                        {
                                unsigned char c = static_cast< unsigned char >( buffer[ at ] );
                                if ( ( c == 0 ) || ( ( c & 0x80 ) == 0 ) || ( ( c & 0xC0 ) == 0xC0 ) )
                                {
                                        howManyLetters++;
                                }
                        }

                        std::cout << "file \"" << file << "\" has table for " << howManyLetters << " letters" << std::endl ;
                        tableOfLetters = new std::string[ howManyLetters ];

                        unsigned int inTable = 0;
                        for ( unsigned int inBuf = 0 ; inBuf < length ; )
                        {
                                unsigned char c = static_cast< unsigned char >( buffer[ inBuf ] );

                                if ( c == 0 )
                                {
                                        tableOfLetters[ inTable++ ] = "";
                                        inBuf ++;
                                }
                                else
                                {
                                        char letter[ 5 ];
                                        unsigned int bytesInLetter = 0;

                                        do {
                                                letter[ bytesInLetter++ ] = c;
                                                c = static_cast< unsigned char >( buffer[ ++inBuf ] );
                                        }
                                        while ( ( ( c & 0x80 ) != 0 ) && ( ( c & 0xC0 ) != 0xC0 )
                                                        && ( bytesInLetter < 4 ) && ( inBuf < length ) ) ;

                                        letter[ bytesInLetter ] = 0;

                                        tableOfLetters[ inTable ] = std::string( letter );
                                        inTable++ ;
                                }
                        }

                        delete[] buffer;
                }
                else
                {
                        std::cerr << "file \"" << file << "\" with table of letters is absent" << std::endl ;
                }
        }

        // decompose letters
        if ( bitmapFont != 0 )
        {
                const unsigned int lettersPerRow = 16;
                const unsigned int rowsInFont = 21;

                if ( rowsInFont * lettersPerRow != howManyLetters )
                        std::cout << "hmmm, table of letters has more or less letters than picture of font" << std::endl ;

                this->charWidth = bitmapFont->w / lettersPerRow;
                this->charHeight = bitmapFont->h / rowsInFont;

                for ( int y = 0; y < bitmapFont->h; y += this->charHeight )
                {
                        for ( int x = 0; x < bitmapFont->w; x += this->charWidth )
                        {
                                BITMAP* bitmapChar = create_bitmap_ex( 32, charWidth, charHeight );
                                blit( bitmapFont, bitmapChar, x, y, 0, 0, charWidth, charHeight );
                                letters.push_back( bitmapChar );
                        }
                }
        }

        destroy_bitmap( bitmapFont );
}

Font::~Font( )
{
        std::for_each( letters.begin(), letters.end(), destroy_bitmap );
}

BITMAP* Font::getPictureOfLetter( const std::string& letter )
{
        for ( unsigned int i = 0; i < howManyLetters; i++ )
        {
                if ( letter.compare( tableOfLetters[ i ] ) == 0 )
                {
                        return letters.at( i );
                }
        }

        // return '?' when letter isn’t found
        return letters.at( '?' - 32 );
}

}
