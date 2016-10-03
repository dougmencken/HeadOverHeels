#include "Menu.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "FontManager.hpp"
#include "Label.hpp"
#include "Font.hpp"

namespace gui
{

Menu::Menu(unsigned int x, unsigned int y)
: Widget(x, y),
  secondColumnX(0),
  rows(0xffff),
  activeOption(0),
  optionImage(GuiManager::getInstance()->findImage("option")),
  selectedOptionImage(GuiManager::getInstance()->findImage("selected-option")),
  selectedOptionImageMini(GuiManager::getInstance()->findImage("selected-option-mini"))
{

}

Menu::Menu(unsigned int x, unsigned int y, unsigned int secondColumnX, unsigned short rows)
: Widget(x, y),
  secondColumnX(secondColumnX),
  rows(rows),
  activeOption(0),
  optionImage(GuiManager::getInstance()->findImage("option")),
  selectedOptionImage(GuiManager::getInstance()->findImage("selected-option")),
  selectedOptionImageMini(GuiManager::getInstance()->findImage("selected-option-mini"))
{

}

Menu::~Menu()
{
  std::for_each(options.begin(), options.end(), DeleteObject());
  options.clear();
}

void Menu::draw(BITMAP* destination)
{
  int dx(this->optionImage->w), dy(0);
  int count = 0;

  // Para cada etiqueta
  for(std::list<Label*>::iterator i = options.begin(); i != options.end(); ++i)
  {
    Label* label = *i;
    std::string fontName(label->getFontName());
    std::string color(label->getColor());

    // Establece el tipo de letra dependiendo de si la opción está o no seleccionada
    if(label == this->activeOption)
    {
      if(this->secondColumnX == 0)
      {
        label->changeFont("big", label->getColor());
      }
      else
      {
        label->changeFont("regular", "orange");
      }
    }
    else
    {
      label->changeFont("regular", "white");
    }

    // Dibuja la opción de menú en un menú de una sola columna o si es un menú de
    // doble columna, dibuja la opción en la primera columna
    if(count <= this->rows)
    {
      // Dibuja la viñeta
      BITMAP* bullet = 0;
      if(this->activeOption == label)
      {
        bullet = this->secondColumnX == 0 ? this->selectedOptionImage : this->selectedOptionImageMini;
      }
      else
      {
        bullet = this->optionImage;
      }
      draw_sprite(destination, bullet, x, y + dy);
      // Ajusta la posición de la etiqueta
      label->changePosition(this->x + dx, this->y + dy);
      // Dibuja la etiqueta
      label->draw(destination);
      // Actualiza la diferencia de la altura
      dy += label->getFont()->getCharHeight() - (label == this->activeOption && this->secondColumnX == 0 ? 8 : 4);
    }
    // Dibuja la opción de menú en la segunda columna
    else
    {
      // Altura inicial de la segunda columna
      if(count == this->rows + 1)
      {
        dy = 0;
      }

      // Dibuja la viñeta
      draw_sprite(destination, (label == this->activeOption ? this->selectedOptionImageMini : this->optionImage), x + this->secondColumnX, y + dy);
      // Ajusta la posición de la etiqueta
      label->changePosition(this->x + dx + this->secondColumnX, this->y + dy);
      // Dibuja la etiqueta
      label->draw(destination);
      // Actualiza la diferencia de la altura
      dy += label->getFont()->getCharHeight() - 4;
    }

    // Reducción del interlineado
    dy -= label->getFont()->getCharHeight() >> 5;

    // Si el menú es de doble columna, cuenta el número de filas
    if(this->secondColumnX != 0)
    {
      count++;
    }
  }
}

void Menu::handleKey(int key)
{
  switch(key >> 8)
  {
    case KEY_UP:
      this->previousOption();
      break;

    case KEY_DOWN:
      this->nextOption();
      break;

    default:
      if(this->sucessor != 0)
      {
        this->sucessor->handleKey(key);
      }
  }
}

void Menu::addOption(Label* label)
{
  options.push_back(label);
}

void Menu::addActiveOption(Label* label)
{
  this->addOption(label);
  this->sucessor = label;
  this->activeOption = label;
}

void Menu::changeOption(const std::string& text, Label* label)
{
  std::list<Label*>::iterator i = std::find_if(options.begin(), options.end(), std::bind2nd(EqualLabel(), text));
  assert(i != options.end());

  Label* oldLabel = *i;
  i = options.erase(i);
  label->setAction(oldLabel->getAction());
  options.insert(i, label);

  if(this->activeOption == oldLabel)
  {
    this->sucessor = label;
    this->activeOption = label;
  }

  delete oldLabel;
}

void Menu::previousOption()
{
  std::list<Label*>::iterator i = std::find_if(options.begin(), options.end(), std::bind2nd(EqualLabel(), this->activeOption->getText()));
  assert(i != options.end());
  this->activeOption = (i == options.begin() ? *options.rbegin() : *(--i));
  this->sucessor = this->activeOption;
}

void Menu::nextOption()
{
  std::list<Label*>::iterator i = std::find_if(options.begin(), options.end(), std::bind2nd(EqualLabel(), this->activeOption->getText()));
  assert(i != options.end());
  this->activeOption = (++i == options.end() ? *options.begin() : *i);
  this->sucessor = this->activeOption;
}

}
