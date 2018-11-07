
#include "Font.hpp"
#include "Ism.hpp"

#include <iostream>
#include <fstream>
#include <algorithm> // std::for_each


namespace gui
{

/* static */ Picture * Font::imageOfFont = nilPointer ;

/* static */ unsigned int Font::howManyLetters = 0 ;

/* static */ std::string * Font::tableOfLetters = nilPointer ;


Font::Font( const std::string& name, const Color& color, bool doubleHeight ) :
        fontName( name ),
        fontColor( color )
{
        if ( Font::imageOfFont == nilPointer )
        {
                std::string nameOfFontFile = iso::sharePath() + "font.png" ;
                autouniqueptr< allegro::Pict > fontFromFile( allegro::Pict::fromPNGFile( iso::pathToFile( nameOfFontFile ) ) );
                if ( ! fontFromFile->isNotNil() )
                {
                        std::cerr << "oops, can’t get letters of fonts from file \"" << nameOfFontFile << "\"" << std::endl ;
                        return ;
                }

                imageOfFont = new Picture( fontFromFile->getW(), fontFromFile->getH() );

                autouniqueptr< Picture > blackLetters( new Picture( *fontFromFile.get() ) );
                blackLetters->colorize( Color::blackColor() );

                const unsigned int offsetOfTint = 1;
                allegro::bitBlit(
                        blackLetters->getAllegroPict(),
                        imageOfFont->getAllegroPict(),
                        0, 0,
                        offsetOfTint, offsetOfTint,
                        imageOfFont->getWidth() - offsetOfTint, imageOfFont->getHeight() - offsetOfTint
                );

                allegro::drawSprite( imageOfFont->getAllegroPict(), *fontFromFile.get(), 0, 0 );

                imageOfFont->setName( "image of font’s letters" );
        }

        Picture* lettersOfFont = new Picture( *imageOfFont );
        lettersOfFont->setName( "picture of letters to make " + name + " font" );

        std::string justFamily = name.substr( name.find( "." ) + 1 );
        std::string justColor = name.substr( 0, name.find( "." ) );

        // double height font
        if ( doubleHeight )
        {
                Picture* bigfont = new Picture( lettersOfFont->getWidth(), lettersOfFont->getHeight() << 1 ) ;
                allegro::stretchBlit( lettersOfFont->getAllegroPict(), bigfont->getAllegroPict(),
                                        0, 0, lettersOfFont->getWidth(), lettersOfFont->getHeight(),
                                        0, 0, bigfont->getWidth(), bigfont->getHeight() );
                delete lettersOfFont ;
                lettersOfFont = bigfont ;
                lettersOfFont->setName( "picture of stretched double height letters to make " + name + " font" );
        }

        // colorize letters
        if ( color != Color::whiteColor () )
        {
                lettersOfFont->colorize( color );
                lettersOfFont->setName( lettersOfFont->getName() + ", yet colored " + justColor );
        }

        // read table of letters once for all fonts
        if ( tableOfLetters == nilPointer )
        {
                howManyLetters = 0;

                std::string file = iso::sharePath() + "letters.utf8";
                std::ifstream lettersFile ( iso::pathToFile( file ), std::ifstream::binary );
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

        this->charWidth = lettersOfFont->getWidth() / lettersPerRow;
        this->charHeight = lettersOfFont->getHeight() / rowsInFont;

        size_t positionInTable = 0;

        for ( unsigned int y = 0; y < lettersOfFont->getHeight(); y += this->charHeight )
        {
                for ( unsigned int x = 0; x < lettersOfFont->getWidth(); x += this->charWidth )
                {
                        Picture* letter = new Picture( charWidth, charHeight );
                        if ( tableOfLetters[ positionInTable ] != "" )
                                allegro::bitBlit( lettersOfFont->getAllegroPict(), letter->getAllegroPict(), x, y, 0, 0, charWidth, charHeight );
                        letter->setName( "image of letter \'" + tableOfLetters[ positionInTable++ ] + "\' for " + justFamily + " font colored " + justColor );
                        letters.push_back( letter );
                }
        }

        delete lettersOfFont ;
}

Font::~Font( )
{
        for ( std::vector < Picture * >::iterator it = letters.begin () ; it != letters.end () ; ++ it )
        {
                delete *it ;
        }
}

Picture* Font::getPictureOfLetter( const std::string& letter )
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
