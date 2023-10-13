
#include "LanguageText.hpp"


namespace gui
{

LanguageText::LanguageText( const std::string& alias )
        : alias( alias )
{

}

void LanguageText::addLine( const std::string& text )
{
        LanguageLine line;
        line.text = text;
        line.font = std::string();
        line.color = std::string();
        lines.push_back( line );
}

void LanguageText::addLine( const std::string& text, const std::string& font, const std::string& color )
{
        LanguageLine line;
        line.text = text;
        line.font = font;
        line.color = color;
        lines.push_back( line );
}

}
