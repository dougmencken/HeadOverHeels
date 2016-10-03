#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GuiManager.hpp"
#include "ConfigurationManager.hpp"

using gui::SelectLanguage;

SelectLanguage::SelectLanguage(BITMAP* destination, const std::string& language)
: Action(),
  destination(destination),
  language(language)
{

}

void SelectLanguage::execute()
{
  ConfigurationManager* configuration = GuiManager::getInstance()->getConfigurationManager();
  configuration->setLanguage(language);
  configuration->write();

  GuiManager::getInstance()->assignLanguage(language);

  CreateMainMenu mainMenu(this->destination);
  mainMenu.execute();
}
