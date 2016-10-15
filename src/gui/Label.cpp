#include "Label.hpp"
#include "Screen.hpp"
#include "FontManager.hpp"
#include "Font.hpp"
#include "actions/Action.hpp"

namespace gui
{

Label::Label( const std::string& text )
: Widget( 0, 0 ),
  text( text ),
  fontName( "regular" ),
  color( "white" ),
  font( 0 ),
  buffer( 0 )
{
        this->createBitmapLabel( this->text, this->fontName, this->color );
}

Label::Label( unsigned int x, unsigned int y, const std::string& text )
: Widget( x, y ),
  text( text ),
  fontName( "regular" ),
  color( "white" ),
  font( 0 ),
  buffer( 0 )
{
        this->createBitmapLabel( this->text, this->fontName, this->color );
}

Label::Label( unsigned int x, unsigned int y, const std::string& text, const std::string& fontName, const std::string& color, int spacing )
: Widget( x, y ),
  text( text ),
  fontName( fontName ),
  color( color ),
  font( 0 ),
  buffer( 0 )
{
        this->createBitmapLabel( this->text, this->fontName, this->color, spacing );
}

Label::~Label( )
{
        destroy_bitmap( this->buffer );
}

void Label::changeFont( const std::string& fontName, const std::string& color )
{
        this->fontName = fontName;
        this->color = color;
        this->createBitmapLabel( this->text, fontName, color );
}

void Label::draw( BITMAP* where )
{
        // Dibuja la cadena en la imagen destino
        draw_sprite( where, this->buffer, this->x, this->y );
}

void Label::handleKey( int key )
{
        if ( key >> 8 == KEY_ENTER && action != 0 )
        {
                action->doIt ();
        }
}

BITMAP* Label::createBitmapLabel(const std::string& text, const std::string& fontName, const std::string& color, int spacing)
{
  int colorIndex(0);
  // Colores empleados en una etiqueta multicolor
  std::string colorFont[] = { "cyan", "yellow", "orange" };

  // Se selecciona la fuente usada para representar el texto
  this->font = FontManager::getInstance()->findFont(fontName, color.compare("multicolor") == 0 ? "white" : color);
  assert(this->font);

  // Si ya está la etiqueta creada se destruye y se vuelve a crear
  if(buffer != 0)
  {
    destroy_bitmap(this->buffer);
  }
  buffer = create_bitmap_ex(32, text.size() * (font->getCharWidth() + spacing), font->getCharHeight());
  clear_to_color(buffer, makecol(255, 0, 255));

  // Número de caracteres en la cadena. No tiene porque coincidir con la longitud de la mísma
  size_t charNum = 0;

  // Descodificación de la cadena UTF-8
  for(size_t i = 0; i < text.size(); i++)
  {
    std::string str = text.substr(i, 1);

    if(str.compare("\300") >= 0 && str.compare("\340") < 0)
    {
      str = text.substr(i, 2);
      i++;
    }
    else if(str.compare("\340") >= 0 && str.compare("\360") < 0)
    {
      str = text.substr(i, 3);
      i += 2;
    }
    else if(str.compare("\360") >= 0)
    {
      str = text.substr(i, 4);
      i += 3;
    }

    // Si la etiqueta es multicolor se selecciona la fuente adecuada
    if(color.compare("multicolor") == 0)
    {
      this->font = FontManager::getInstance()->findFont(fontName, colorFont[colorIndex]);
      assert(this->font);
      colorIndex = (colorIndex == 2 ? 0 : colorIndex + 1);
    }

    // Copia del carácter a la imagen de la etiqueta
    blit(this->font->getChar(str), buffer, 0, 0, charNum * (this->font->getCharWidth() + spacing), 0, this->font->getCharWidth(), this->font->getCharHeight());
    charNum++;
  }

  // Si la longitud de la cadena no coincide con el número de caracteres de la misma
  // (caso del cirílico) entonces se ajusta el buffer al tamaño real
  if(charNum != text.size())
  {
    BITMAP* bitmap = create_bitmap_ex(32, charNum * (font->getCharWidth() + spacing), font->getCharHeight());
    blit(this->buffer, bitmap, 0, 0, 0, 0, bitmap->w, bitmap->h);
    destroy_bitmap(this->buffer);
    this->buffer = bitmap;
  }

  return this->buffer;
}

Font* Label::getFont() const
{
  return this->font;
}

bool EqualLabel::operator()(const Label* label, const std::string& text) const
{
  return (label->getText().compare(text) == 0);
}

}
