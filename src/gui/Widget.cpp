#include "Widget.hpp"
#include "actions/Action.hpp"

namespace gui
{

Widget::Widget(unsigned int x, unsigned int y)
: x(x),
  y(y),
  sucessor(0),
  action(0)
{

}

Widget::~Widget()
{

}

void Widget::execute()
{
  if(this->action != 0)
  {
    this->action->execute();
  }
}

void Widget::handleKey(int key)
{
  if(this->sucessor != 0)
  {
    this->sucessor->handleKey(key);
  }
}

void Widget::setAction(Action* action)
{
  this->action = action;
}

Action* Widget::getAction() const
{
  return this->action;
}

}
