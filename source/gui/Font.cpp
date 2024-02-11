
#include "Font.hpp"

#include "ospaths.hpp"

#include <iostream>
#include <fstream>
#include <algorithm> // std::for_each


namespace gui
{

/* static */ Picture * Font::imageOfFont = nilPointer ;

/* static */ unsigned int Font::howManyLetters = 0 ;

/* static */ std::string * Font::listOfLetters = nilPointer ;

/* static */ std::vector < Font * > Font::fonts ;


Font::Font( const std::string & name, const std::string & color, bool doubleHeight ) :
        fontName( name ),
        fontColor( color )
{
        if ( Font::imageOfFont == nilPointer )
        {
                std::string nameOfFontFile = ospaths::sharePath() + "font.png" ;
                autouniqueptr< allegro::Pict > fontFromFile( allegro::Pict::fromPNGFile( ospaths::pathToFile( nameOfFontFile ) ) );
                if ( ! fontFromFile->isNotNil() )
                {
                        std::cerr << "oops, can’t get the image of letters from file \"" << nameOfFontFile << "\"" << std::endl ;
                        return ;
                }

                autouniqueptr< Picture > blackLetters( new Picture( * fontFromFile ) );

                // white to transparency
                Color::replaceColor( * blackLetters, Color::whiteColor(), Color::keyColor() );

                autouniqueptr< Picture > whiteLetters( new Picture( * blackLetters ) );

                // black to white
                Color::replaceColor( * whiteLetters, Color::blackColor(), Color::whiteColor() );

                unsigned int width  = fontFromFile->getW ();
                unsigned int height = fontFromFile->getH ();
                Font::imageOfFont = new Picture( width, height );

                const unsigned int offsetOfTint = 1 ;
                allegro::bitBlit(
                        blackLetters->getAllegroPict(),
                        Font::imageOfFont->getAllegroPict(),
                        0, 0,
                        offsetOfTint, offsetOfTint,
                        width - offsetOfTint, height - offsetOfTint
                );

                const allegro::Pict & previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( Font::imageOfFont->getAllegroPict () );
                allegro::drawSprite( whiteLetters->getAllegroPict (), 0, 0 );
                allegro::Pict::setWhereToDraw( previousWhere );

                Font::imageOfFont->setName( "image of the game’s font" );
        }

        Picture* lettersOfFont = new Picture( * Font::imageOfFont );
        std::string pictureOfWhat = "letters" ;

        if ( doubleHeight )
        {       // stretch for the double height
                Picture* bigfont = new Picture( lettersOfFont->getWidth(), lettersOfFont->getHeight() << 1 ) ;
                allegro::stretchBlit( lettersOfFont->getAllegroPict(), bigfont->getAllegroPict(),
                                        0, 0, lettersOfFont->getWidth(), lettersOfFont->getHeight(),
                                        0, 0, bigfont->getWidth(), bigfont->getHeight() );
                delete lettersOfFont ;
                lettersOfFont = bigfont ;
                pictureOfWhat = "stretched double height letters" ;
        }

        lettersOfFont->setName( "picture of " + pictureOfWhat + " to make the game’s \"" + name + "\" font" );

        // colorize letters by changing white to the font color
        if ( getColor() != "white" )
        {
                lettersOfFont->colorizeWhite( Color::byName( getColor() ) );
                lettersOfFont->setName( lettersOfFont->getName() + ", yet colored " + getColor() );
        }

        // read the list of letters once for all fonts
        if ( Font::listOfLetters == nilPointer )
        {
                howManyLetters = 0;

                std::string file = ospaths::sharePath() + "letters.utf8";
                std::ifstream lettersFile ( ospaths::pathToFile( file ), std::ifstream::binary );
                if ( lettersFile )      /* no comparison with nil here, see https://gcc.gnu.org/gcc-6/porting_to.html
                                         “ The change to iostream classes also affects code that tries
                                           to check for stream errors by comparing to NULL or 0.
                                           Such code should be changed to simply test the stream directly,
                                           instead of comparing it to a null pointer ” */
                {
                        lettersFile.seekg( 0, lettersFile.end );
                        unsigned int length = lettersFile.tellg();
                        lettersFile.seekg( 0, lettersFile.beg );

                        char buffer[ length ];
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

                        std::cout << "file \"" << file << "\" lists " << howManyLetters << " letters" << std::endl ;
                        listOfLetters = new std::string[ howManyLetters ];

                        unsigned int inTable = 0;
                        for ( unsigned int inBuf = 0 ; inBuf < length ; )
                        {
                                unsigned char c = static_cast< unsigned char >( buffer[ inBuf ] );

                                if ( c == 0 )
                                {
                                        listOfLetters[ inTable++ ] = "" ;
                                        inBuf ++ ;
                                }
                                else
                                {
                                        char letter[ 5 ];
                                        unsigned int byteInLetter = 0 ;

                                        do {
                                                letter[ byteInLetter ++ ] = c ;
                                                ++ inBuf ;
                                                if ( inBuf == length ) break ;
                                                c = static_cast< unsigned char >( buffer[ inBuf ] );
                                        }
                                        while ( ( ( c & 0x80 ) != 0 ) && ( ( c & 0xC0 ) != 0xC0 )
                                                        && ( byteInLetter < 4 ) && ( inBuf < length ) ) ;

                                        letter[ byteInLetter ] = 0 ; // end of string

                                        listOfLetters[ inTable ] = std::string( letter );
                                        inTable++ ;
                                }
                        }
                }
                else
                {
                        std::cerr << "can't read file \"" << file << "\" with the list of letters drawn in the font" << std::endl ;
                }
        }

        // decompose letters
        const unsigned int lettersPerLine = 16;
        const unsigned int linesInFont = 21;

        if ( linesInFont * lettersPerLine != howManyLetters )
                std::cout << "hmmm, the list of letters has more or less letters than the picture of font" << std::endl ;

        // the size of the font image is 272 x 609 pixels, or 16 x 21 letters 17 x 29 pixels each
        unsigned int charWidth = lettersOfFont->getWidth() / lettersPerLine ;
        unsigned int charHeight = lettersOfFont->getHeight() / linesInFont ;

        size_t positionInTable = 0;

        for ( unsigned int y = 0; y < lettersOfFont->getHeight(); y += charHeight )
        {
                for ( unsigned int x = 0; x < lettersOfFont->getWidth(); x += charWidth )
                {
                        Picture* letter = new Picture( charWidth, charHeight );
                        if ( listOfLetters[ positionInTable ] != "" )
                                allegro::bitBlit( lettersOfFont->getAllegroPict(), letter->getAllegroPict(), x, y, 0, 0, charWidth, charHeight );

                        letter->setName( "image of letter \'" + listOfLetters[ positionInTable ++ ]
                                                + "\' for \"" + getName() + "\" font colored " + getColor() );
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

Picture* Font::getPictureOfLetter( const std::string & letter ) const
{
        if ( listOfLetters != nilPointer )
        {
                std::string letterInTable = letter ;

                // render cyrillic ё as e diaeresis (e umlaut)
                if ( letter == "\u0451" ) letterInTable = "\u00EB" ; // \u0451 cyrillic small letter io — \u00EB latin small letter e with diaeresis
                if ( letter == "\u0401" ) letterInTable = "\u00CB" ; // \u0401 cyrillic capital letter IO — \u00CB latin capital letter E with diaeresis

                for ( unsigned int i = 0; i < howManyLetters; i++ )
                {
                        if ( letterInTable == listOfLetters[ i ] )
                                return letters.at( i );
                }
        }

        // return '?' when letter isn’t found
        if ( letters.size() > ( '?' - 32 ) )
                return letters.at( '?' - 32 );

        return nilPointer ;
}

/* static */
const Font & Font::fontByNameAndColor ( const std::string & name, const std::string & color )
{
        std::string fontName = ( ! name.empty() ) ? name : "plain" ;
        std::string fontColor = ( ! color.empty() ) ? color : "white" ;

        for (  std::vector< Font * >::const_iterator ifont = Font::fonts.begin (); ifont != Font::fonts.end (); ++ ifont ) {
                Font * font = ( *ifont ) ;
                if ( font->getName() == fontName && font->getColor() == fontColor )
                        return *font ;
        }

        IF_DEBUG( std::cout << "making new font \"" << fontName << "\" colored \"" << fontColor << "\"" << std::endl )

        bool doubleHeight = ( name == "big" );
        Font* newFont = new Font( fontName, fontColor, doubleHeight );
        Font::fonts.push_back( newFont ) ;

        return *newFont ;
}

}
