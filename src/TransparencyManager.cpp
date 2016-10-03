#include "TransparencyManager.hpp"

namespace isomot
{

TransparencyManager::TransparencyManager()
{
 // Se inicializa la tabla de transparencias
  for(unsigned int i = 0; i <= 101; i++)
  {
    this->table.push_back(0);
  }
}

TransparencyManager::~TransparencyManager()
{
  this->table.clear();
}

void TransparencyManager::add(const unsigned char percent)
{
  if(percent <= 100)
  {
    this->table[percent]++;
  }
}

void TransparencyManager::remove(const unsigned char percent)
{
  if(percent <= 100)
  {
    if(this->table[percent] > 0)
    {
      this->table[percent]--;
    }
  }
}

}
