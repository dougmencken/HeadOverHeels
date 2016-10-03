#include "BonusManager.hpp"

namespace isomot
{

BonusManager::BonusManager()
{

}

BonusManager::~BonusManager()
{
  bonusRooms.clear();
}

BonusManager* BonusManager::instance = 0;
BonusManager* BonusManager::getInstance()
{
  if(instance == 0)
  {
    instance = new BonusManager();
  }

  return instance;
}

void BonusManager::destroyBonus(const std::string& fileName, const short label)
{
  // Se busca la sala
  std::list<BonusRoom>::iterator result = std::find(bonusRooms.begin(), bonusRooms.end(), fileName);

  // Si se encuentra se añade como ausente el bonus, en caso contrario se crea un registro
  // con la nueva sala y se añade como ausente el bonus
  if(result != bonusRooms.end())
  {
    (*result).destroyBonus(label);
  }
  else
  {
    BonusRoom bonusRoom(fileName);
    bonusRoom.destroyBonus(label);
    bonusRooms.push_back(bonusRoom);
  }
}

bool BonusManager::isPresent(const std::string& fileName, const short label)
{
  // Se busca la sala
  std::list<BonusRoom>::iterator result = std::find(bonusRooms.begin(), bonusRooms.end(), fileName);

  // El bonus está presente en la sala si el registro de la sala no existe o el bonus no está presente en la sala
  return (result == bonusRooms.end() || (result != bonusRooms.end() && !(*result).isPresent(label)));
}

void BonusManager::load(const sgxml::bonus::room_sequence& roomSequence)
{
  for(sgxml::bonus::room_const_iterator i = roomSequence.begin(); i != roomSequence.end(); ++i)
  {
    this->destroyBonus((*i).filename(), (*i).label());
  }
}

void BonusManager::save(sgxml::bonus::room_sequence& roomSequence)
{
  for(std::list<BonusRoom>::iterator r = this->bonusRooms.begin(); r !=this->bonusRooms.end(); ++r)
  {
    for(std::list<short>::iterator b = (*r).getBonusItems().begin(); b != (*r).getBonusItems().end(); ++b)
    {
      roomSequence.push_back(sgxml::room(*b, (*r).getFileName()));
    }
  }
}

void BonusManager::reset()
{
  this->bonusRooms.clear();
}

BonusRoom::BonusRoom(const std::string& fileName)
{
  this->fileName = fileName;
}

BonusRoom::~BonusRoom()
{
  bonusItems.clear();
}

void BonusRoom::destroyBonus(const short label)
{
  bonusItems.push_back(label);
}

bool BonusRoom::isPresent(short label)
{
  return std::find(bonusItems.begin(), bonusItems.end(), label) != bonusItems.end();
}

bool BonusRoom::operator==(const std::string& fileName)
{
  return (this->fileName.compare(fileName) == 0);
}

}
