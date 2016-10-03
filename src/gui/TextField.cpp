#include "TextField.hpp"
#include "Font.hpp"
#include "Label.hpp"

using gui::TextField;
using gui::Alignment;
using gui::Label;

TextField::TextField(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Alignment& alignment)
: Widget(x, y),
  width(width),
  height(height),
  alignment(alignment),
  delta(y)
{

}

TextField::~TextField()
{
  std::for_each(this->lines.begin(), this->lines.end(), DeleteObject());
  this->lines.clear();
}

void TextField::draw(BITMAP* destination)
{
  for(std::list<Label*>::iterator i = this->lines.begin(); i != this->lines.end(); ++i)
  {
    (*i)->draw(destination);
  }
}

void TextField::addLine(const std::string& text, const std::string& font, const std::string& color)
{
  int posX = 0;

  switch(this->alignment)
  {
    case gui::LeftAlignment:
      break;

    case gui::CenterAlignment:
      {
        std::auto_ptr<Label> label(new Label(text));
        posX = (this->width - this->x - label->getWidth()) >> 1;
      }
      break;

    case gui::RightAlignment:
      {
        std::auto_ptr<Label> label(new Label(text));
        posX = this->width - label->getWidth();
      }
      break;
  }

  Label* label = new Label(posX, delta, text, font, color);
  this->delta += label->getFont()->getCharHeight();
  lines.push_back(label);
}

void TextField::changePosition(unsigned int x, unsigned int y)
{
  for(std::list<Label*>::iterator i = this->lines.begin(); i != this->lines.end(); ++i)
  {
    Label* label = (*i);
    label->changePosition(label->getX() + x - this->x, label->getY() + y - this->y);
  }

  this->x = x;
  this->y = y;
}

