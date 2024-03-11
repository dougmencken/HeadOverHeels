
#include "Label.hpp"

#include "Color.hpp"
#include "Action.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "UnicodeUtilities.hpp"


namespace gui
{

Label::Label( const std::string & text )
: Widget( )
        , imageOfLetters( nilPointer )
        , text( text )
        , color( "white" )
        , height2x( false )
        , multicolored( false )
        , spacing( 0 )
        , myAction( nilPointer )
{
        createImageOfLabel( text );
}

Label::Label( const std::string & text, const Font & font, bool multicolor )
: Widget( )
        , imageOfLetters( nilPointer )
        , text( text )
        , color( font.getColor () )
        , height2x( font.isDoubleHeight () )
        , multicolored( multicolor )
        , spacing( 0 )
        , myAction( nilPointer )
{
        if ( this->color.empty() ) this->color = "white" ;

        createImageOfLabel( text );
}

Label::~Label( )
{
        delete imageOfLetters ;
}

void Label::update()
{
        createImageOfLabel( getText() );
}

void Label::draw ()
{
        if ( imageOfLetters != nilPointer )
                allegro::drawSprite( imageOfLetters->getAllegroPict(), getX(), getY() );
}

void Label::handleKey( const std::string& key )
{
        if ( myAction != nilPointer && ( key == "Enter" || key == "Space" ) )
        {
                myAction->doIt ();
        }
}

void Label::createImageOfLabel( const std::string & text )
{
        const Font & font = getFont() ;

        // re-create the image of letters
        delete imageOfLetters ;

        unsigned int howManyLetters = utf8StringLength( text );
        imageOfLetters = new Picture( howManyLetters * ( font.getWidthOfLetter( "O" ) + getSpacing() ), font.getHeightOfLetter( "I" ) );

        std::string kindOFont = font.isDoubleHeight() ? "double-height font" : "font" ;
        std::string nameOfImage = "image of label \"" + text + "\" using the game’s " + kindOFont + " colored " + this->color ;
        imageOfLetters->setName( nameOfImage );

        if ( ! text.empty() )
        {
                static const size_t colors_in_multicolor = 3;
                // the sequence of colors for a multicolor label
                static std::string multiColors[ colors_in_multicolor ] = {  "cyan", "yellow", "orange"  }; // the amstrad cpc sequence
                if ( GameManager::getInstance().isSimpleGraphicsSet() )
                        multiColors[ 2 ] = "white" ; // the original speccy sequence is “ cyan yellow white ”

                unsigned short cycle = 0 ; // current position in that sequence
                std::string fontColor = font.getColor() ;

                unsigned int letterInLine = 0 ;

                std::string::const_iterator iter = text.begin ();
                while ( iter != text.end () )
                {
                        if ( this->multicolored )
                        {
                                fontColor = multiColors[ cycle ] ;

                                // cycle in the sequence of colors
                                ++ cycle ;
                                if ( cycle >= colors_in_multicolor ) cycle = 0 ;
                        }

                        size_t from = std::distance( text.begin (), iter );
                        size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // the string iterator increments here
                        std::string utf8letter = text.substr( from, howMany );

                        // draw letter
                        Picture* letter = Font::fontByColorAndSize( fontColor, font.isDoubleHeight() ).getPictureOfLetter( utf8letter ) ;
                        if ( letter != nilPointer )
                                allegro::bitBlit(
                                    letter->getAllegroPict(),
                                    imageOfLetters->getAllegroPict(),
                                    0,
                                    0,
                                    letterInLine * ( letter->getWidth() + getSpacing() ),
                                    0,
                                    letter->getWidth(),
                                    letter->getHeight()
                                ) ;

                        letterInLine ++ ;
                }
        }
}

}
