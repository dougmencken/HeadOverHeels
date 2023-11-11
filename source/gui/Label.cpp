
#include "Label.hpp"
#include "Color.hpp"
#include "Action.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"

#include <iostream>
#include <cassert>


namespace gui
{

Label::Label( const std::string & text )
: Widget( ),
        imageOfLetters( nilPointer ),
        text( text ),
        fontFamily( "plain" ),
        color( "white" ),
        spacing( 0 ),
        myAction( nilPointer )
{
        createImageOfLabel( text );
}

Label::Label( const std::string & text, const std::string & family, const std::string & color, int spacing )
: Widget( ),
        imageOfLetters( nilPointer ),
        text( text ),
        fontFamily( family ),
        color( color ),
        spacing( spacing ),
        myAction( nilPointer )
{
        if ( family.empty() ) this->fontFamily = "plain" ;
        if ( color.empty() ) this->color = "white" ;

        createImageOfLabel( text );
}

Label::~Label( )
{
        delete imageOfLetters ;
}

/* static */
Font & Label::fontByFamilyAndColor( const std::string & family, const std::string & color )
{
        Font* found = GuiManager::getInstance().getOrCreateFontByFamilyAndColor( family, color == "multicolor" ? "white" : color ) ;
        assert( found != nilPointer );
        return *found ;
}

void Label::update()
{
        createImageOfLabel( this->text );
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
        Font & font = Label::fontByFamilyAndColor( getFontFamily(), getColor() );

        // re-create the image of letters
        delete imageOfLetters ;

        unsigned int howManyLetters = utf8StringLength( text );
        imageOfLetters = new Picture( howManyLetters * ( font.getWidthOfLetter( "O" ) + getSpacing() ), font.getHeightOfLetter( "I" ) );

        std::string nameOfImage = "image of label \"" + text + "\" using " + getFontFamily () + " font colored " + getColor () ;
        imageOfLetters->setName( nameOfImage );

        if ( ! text.empty() )
        {
                static const size_t colors_in_multicolor = 3;
                // the sequence of colors for a multicolor label
                static std::string multiColors[ colors_in_multicolor ] = {  "cyan", "yellow", "orange"  }; // the amstrad cpc sequence
                /* if ( GameManager::getInstance().isSimpleGraphicsSet() )
                        multiColors[ 2 ] = "white" ; // the original speccy sequence is “ cyan yellow white ” */

                unsigned short cycle = 0; // position in that sequence for character to draw
                Font & fontToUse = font ;

                unsigned int letterInLine = 0 ;

                std::string::const_iterator iter = text.begin ();
                while ( iter != text.end () )
                {
                        if ( this->color == "multicolor" )
                        {
                                // to color the letter, pick the new font
                                fontToUse = Label::fontByFamilyAndColor( font.getFamily(), multiColors[ cycle ] );

                                // cycle in the sequence of colors
                                ++ cycle ;
                                if ( cycle >= colors_in_multicolor ) cycle = 0 ;
                        }

                        size_t from = std::distance( text.begin (), iter );
                        size_t howMany = incUtf8StringIterator ( iter, text.end () ) ; // the string iterator increments here
                        std::string utf8letter = text.substr( from, howMany );

                        // draw letter
                        Picture* letter = fontToUse.getPictureOfLetter( utf8letter ) ;
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
