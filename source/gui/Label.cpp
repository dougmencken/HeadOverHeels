
#include "Label.hpp"

#include "Color.hpp"
#include "Action.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"


namespace gui
{

Label::Label( const std::string & theText )
        : Widget( )
        , text( theText )
        , howManyLetters( utf8StringLength( theText ) )
        , font( new Font( "white" ) )
        , cyclicallyColoredLetters( false )
        , spacing( 0 )
        , myAction( nilPointer )
{
}

Label::Label( const std::string & theText, Font * theFont, bool multicolor )
        : Widget( )
        , text( theText )
        , howManyLetters( utf8StringLength( theText ) )
        , font( theFont != nilPointer ? theFont : new Font( "white" ) )
        , cyclicallyColoredLetters( multicolor )
        , spacing( 0 )
        , myAction( nilPointer )
{
}

Label::~Label( )
{
        delete font ;
}

void Label::draw ()
{
        Picture* image = createImageOfLabel() ;
        if ( image != nilPointer ) allegro::drawSprite( image->getAllegroPict(), getX(), getY() );
        delete image ;
}

void Label::handleKey( const std::string & key )
{
        if ( myAction != nilPointer && ( key == "Enter" || key == "Space" ) )
                myAction->doIt ();
}

/* static */
Picture* Label::createImageOfString ( const std::string & text, unsigned int textLength, int spacing,
                                        const Font & font, std::vector< std::string > colorCycle )
{
        if ( text.empty() ) return new Picture( font.getWidthOfLetter(), font.getHeightOfLetter() ) ;

        Picture* imageOfString = new Picture( textLength * ( font.getWidthOfLetter() + spacing ) - spacing,
                                                font.getHeightOfLetter() );

        std::string kindOFont = font.isDoubleHeight() ? "double-height font" : "font" ;
        std::string nameOfImage = "image of label \"" + text + "\" using the game’s " + kindOFont + " colored " + font.getColor() ;
        imageOfString->setName( nameOfImage );

        unsigned short color = 0 ; // current position in the sequence of colors
        unsigned int letterInLine = 0 ;

        std::string::const_iterator iter = text.begin ();
        while ( iter != text.end () ) {
                size_t from = std::distance( text.begin (), iter );
                size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // the string iterator increments here
                std::string utf8letter = text.substr( from, howMany );

                // add image of this letter
                Font forLetter( colorCycle[ color ], font.isDoubleHeight() );
                Picture* letter = forLetter.getPictureOfLetter( utf8letter ) ;
                if ( letter != nilPointer )
                        allegro::bitBlit(
                            letter->getAllegroPict(),
                            imageOfString->getAllegroPict(),
                            0,
                            0,
                            letterInLine * ( letter->getWidth() + spacing ),
                            0,
                            letter->getWidth(),
                            letter->getHeight()
                        ) ;

                letterInLine ++ ;

                if ( colorCycle.size() > 1 )
                {       // cycle in the sequence of colors
                        ++ color ;
                        if ( color >= colorCycle.size() ) color = 0 ;
                }
        }

        return imageOfString ;
}

std::vector< std::string > Label::getColorCycle () const
{
        std::vector< std::string > colorCycle ;
        if ( areLettersCyclicallyColored() ) {
                colorCycle.push_back( "cyan" );
                colorCycle.push_back( "yellow" );
                // the original speccy sequence is “ cyan yellow white ”
                // “ cyan yellow orange ” is the amstrad cpc sequence
                colorCycle.push_back( GameManager::getInstance().isSimpleGraphicsSet() ? "white" : "orange" );
        } else
                colorCycle.push_back( getFont().getColor() );

        return colorCycle ;
}

}
