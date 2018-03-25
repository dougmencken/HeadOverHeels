
#include "Font.hpp"
#include "Color.hpp"
#include "Ism.hpp"

#include <iostream>
#include <algorithm> // std::for_each

#include <loadpng.h>


namespace gui
{

/* static */ unsigned int Font::howManyLetters = 0 ;

/* static */ std::string * Font::tableOfLetters = nilPointer ;


Font::Font( const std::string& name, const BITMAP * pictureOfLetters, Color * color, bool doubleHeight ) :
        fontName( name ),
        fontColor( color )
{
        if ( pictureOfLetters == nilPointer )
        {
                std::cerr << "picture of letters is nil" << std::endl ;
                return;
        }

        BITMAP* lettersOfFont = create_bitmap( pictureOfLetters->w, pictureOfLetters->h );
        blit( const_cast< BITMAP* >( pictureOfLetters ), lettersOfFont, 0, 0, 0, 0, lettersOfFont->w, lettersOfFont->h );

        // double height font
        if ( doubleHeight )
        {
                BITMAP* bigfont = create_bitmap_ex( bitmap_color_depth( lettersOfFont ), lettersOfFont->w, lettersOfFont->h << 1 );
                stretch_blit( lettersOfFont, bigfont, 0, 0, lettersOfFont->w, lettersOfFont->h, 0, 0, bigfont->w, bigfont->h );
                allegro::destroyBitmap( lettersOfFont );
                lettersOfFont = bigfont;
        }

        // colorize letters
        if ( color != Color::whiteColor () )
        {
                Color::colorizePicture( lettersOfFont, color ) ;
        }

        // read table of letters once for all fonts
        if ( tableOfLetters == nilPointer )
        {
                howManyLetters = 0;

                std::string file = isomot::sharePath() + "letters.utf8";
                std::ifstream lettersFile ( isomot::pathToFile( file ), std::ifstream::binary );
                if ( lettersFile )      /* no comparison with nil here, see https://gcc.gnu.org/gcc-6/porting_to.html
                                         “ The change to iostream classes also affects code that tries
                                           to check for stream errors by comparing to NULL or 0.
                                           Such code should be changed to simply test the stream directly,
                                           instead of comparing it to a null pointer ” */
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

                        delete buffer;
                }
                else
                {
                        std::cerr << "file \"" << file << "\" with table of letters is absent" << std::endl ;
                }
        }

        // decompose letters
        const unsigned int lettersPerRow = 16;
        const unsigned int rowsInFont = 21;

        if ( rowsInFont * lettersPerRow != howManyLetters )
                std::cout << "hmmm, table of letters has more or less letters than picture of font" << std::endl ;

        this->charWidth = lettersOfFont->w / lettersPerRow;
        this->charHeight = lettersOfFont->h / rowsInFont;

        for ( int y = 0; y < lettersOfFont->h; y += this->charHeight )
        {
                for ( int x = 0; x < lettersOfFont->w; x += this->charWidth )
                {
                        BITMAP* letter = create_bitmap_ex( 32, charWidth, charHeight );
                        blit( lettersOfFont, letter, x, y, 0, 0, charWidth, charHeight );
                        letters.push_back( letter );
                }
        }

        allegro::destroyBitmap( lettersOfFont );
}

Font::~Font( )
{
        for ( std::vector < BITMAP * >::iterator it = letters.begin () ; it != letters.end () ; ++ it )
        {
                allegro::destroyBitmap( *it );
        }
}

std::string Font::getFamily() const
{
        const char * family = strrchr ( fontName.c_str () , '.' );
        if ( family == nilPointer ) return fontName;
        return std::string( family + /* to skip that dot */ 1 );
}

BITMAP* Font::getPictureOfLetter( const std::string& letter )
{
        for ( unsigned int i = 0; i < howManyLetters; i++ )
        {
                if ( letter == tableOfLetters[ i ] )
                {
                        return letters.at( i );
                }
        }

        // return '?' when letter isn’t found
        return letters.at( '?' - 32 );
}

}
