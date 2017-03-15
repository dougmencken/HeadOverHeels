
#include "FontManager.hpp"
#include "Gui.hpp"
#include "Font.hpp"


namespace gui
{

FontManager* FontManager::instance = 0;

FontManager::FontManager( )
{

}

FontManager::~FontManager( )
{
        std::for_each( this->fonts.begin (), this->fonts.end (), DeleteObject() );
        this->fonts.clear();
}

FontManager* FontManager::getInstance()
{
        if ( instance == 0 )
        {
                instance = new FontManager();
        }

        return instance;
}

void FontManager::createFont( const std::string& fontName, const std::string& fontFile, int color, bool doubleSize )
{
        this->fonts.push_back( new Font( fontName, fontFile, color, doubleSize ) );
}

Font* FontManager::findFont( const std::string& font, const std::string& color )
{
        std::list< Font * >::iterator i = std::find_if( this->fonts.begin (), this->fonts.end (), std::bind2nd( EqualFont(), font + "-" + color ) );
        return ( i != this->fonts.end() ? *i : 0 );
}

bool EqualFont::operator() ( const Font* font, const std::string& fontName ) const
{
        return ( font->getFontName().compare( fontName ) == 0 );
}

}
