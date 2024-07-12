
#include "Font.hpp"

#include "ospaths.hpp"

#include <iostream>
#include <fstream>


namespace gui
{

/* static */ Picture * Font::imageOfFont = nilPointer ;

/* static */ unsigned int Font::howManyLetters = 0 ;

/* static */ std::map< std::string /* letter */, unsigned int /* index */ > * Font::mapOfLetters = nilPointer ;


Font::Font( const std::string & color, bool doubleHeightStretching )
        : fontColor( color.empty() ? "white" : color )
        , doubleHeight( doubleHeightStretching )
{
        if ( Font::imageOfFont == nilPointer )
        {
                std::string nameOfFontFile = "font.png" ;
                autouniqueptr< allegro::Pict > fontFromFile(
                                        allegro::Pict::fromPNGFile(
                                                ospaths::pathToFile( ospaths::sharePath(), nameOfFontFile ) ) );
                if ( ! fontFromFile->isNotNil() ) {
                        std::cerr << "oops, can’t get the image of letters from \"" << nameOfFontFile << "\"" << std::endl ;
                        return ;
                }

                autouniqueptr< Picture > blackLetters( new Picture( * fontFromFile ) );

                autouniqueptr< Picture > whiteLetters( new Picture( * fontFromFile ) );
                Color::invertColors( * whiteLetters );

                // background white or black to the color of transparency
                Color::replaceColorAnyAlpha( * blackLetters, Color::whiteColor(), Color::keyColor() );
                Color::replaceColorAnyAlpha( * whiteLetters, Color::blackColor(), Color::keyColor() );

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

        // read the letters file once for all fonts
        if ( Font::mapOfLetters == nilPointer )
                /* bool mapOfLettersOkay = */ Font::readLettersFile() ;

        if ( Font::howManyLetters != Font::Lines_In_Font * Font::Letters_Per_Line )
                std::cout << "hmmm, the letters file lists more or less letters"
                                << " (" << Font::howManyLetters << ") than the picture of font has"
                                        << " (" << ( Font::Lines_In_Font * Font::Letters_Per_Line ) << ")"
                                                << std::endl ;
}

/* static */
bool Font::readLettersFile ()
{
        Font::howManyLetters = 0 ;

        std::string fileWithLetters = "letters.utf8" ;
        std::ifstream lettersFile ( ospaths::pathToFile( ospaths::sharePath(), fileWithLetters ), std::ifstream::binary );
        if ( ! lettersFile )    /* no comparison with nil here, see https://gcc.gnu.org/gcc-6/porting_to.html
                                 “ The change to iostream classes also affects code that tries
                                   to check for stream errors by comparing to NULL or 0.
                                   Such code should be changed to simply test the stream directly,
                                   instead of comparing it to a null pointer ” */
        {
                std::cerr << "can’t read file \"" << fileWithLetters << "\" with the list of letters drawn in the font" << std::endl ;
                return false ;
        }

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
                        Font::howManyLetters ++ ;
                }
        }

        std::cout << "file \"" << fileWithLetters << "\" lists " << Font::howManyLetters << " letters" << std::endl ;
        Font::mapOfLetters = new std::map< std::string /* letter */, unsigned int /* index */ > () ;

        unsigned int inTable = 0;
        for ( unsigned int inBuf = 0 ; inBuf < length ; )
        {
                unsigned char c = static_cast< unsigned char >( buffer[ inBuf ] );

                if ( c == 0 ) {
                        ++ inTable ;
                        ++ inBuf ;
                }
                else {
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

                        Font::mapOfLetters->insert( std::make_pair( std::string( letter ), inTable ) );
                        inTable ++ ;
                }
        }

        return true ;
}

Picture* Font::getPictureOfLetter( const std::string & letter ) const
{
        if ( Font::imageOfFont != nilPointer && Font::mapOfLetters != nilPointer )
        {
                std::string letterInTable = letter ;

                // render cyrillic ё as e diaeresis (e umlaut)
                if ( letter == "\u0451" ) letterInTable = "\u00EB" ; // \u0451 cyrillic small letter io — \u00EB latin small letter e with diaeresis
                if ( letter == "\u0401" ) letterInTable = "\u00CB" ; // \u0401 cyrillic capital letter IO — \u00CB latin capital letter E with diaeresis

                if ( Font::mapOfLetters->find( letterInTable ) != Font::mapOfLetters->end() )
                {
                        unsigned int letterWidth = getWidthOfLetter() ;
                        unsigned int letterHeight = getHeightOfLetter() ;
                        Picture* letter = new Picture( letterWidth, letterHeight );

                        unsigned int letterHeightSingle = isDoubleHeight() ? ( letterHeight >> 1 ) : letterHeight ;

                        unsigned int n = Font::mapOfLetters->operator[]( letterInTable );
                        unsigned int x = ( n % Font::Letters_Per_Line ) * letterWidth ;
                        unsigned int y = ( n / Font::Letters_Per_Line ) * letterHeightSingle ;

                        if ( ! isDoubleHeight() )
                                allegro::bitBlit( Font::imageOfFont->getAllegroPict(), letter->getAllegroPict(),
                                                        x, y, 0, 0, letterWidth, letterHeight );
                        else
                                allegro::stretchBlit( Font::imageOfFont->getAllegroPict(), letter->getAllegroPict(),
                                                        x, y, letterWidth, letterHeightSingle,
                                                        0, 0, letterWidth, letterHeight );

                        letter->colorizeWhite( Color::byName( getColor() ) );

                        letter->setName( "image of letter “" + letterInTable + "” for the "
                                                + ( isDoubleHeight() ? "double-height " : "" ) + "game’s font colored " + getColor() );

                        return letter ;
                }
        }

        // return '?' when letter isn’t found
        return getPictureOfLetter( "?" );
}

}
