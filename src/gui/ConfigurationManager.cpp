#include "ConfigurationManager.hpp"

using gui::ConfigurationManager;
using isomot::GameKey;

ConfigurationManager::ConfigurationManager(const std::string& fileName)
: fileName(fileName)
{

}

void ConfigurationManager::read()
{
  // Carga el archivo XML especificado y almacena los datos XML en los diferentes atributos
  try
  {
    std::auto_ptr<cxml::ConfigurationXML> configurationXML(cxml::configuration(fileName.c_str()));

    // Almacena el idioma seleccionado
    this->language = configurationXML->language();

    // Almacena la configuración del teclado
    this->keys[static_cast<int>(isomot::KeyNorth)] = configurationXML->keyboard().left();
    this->keys[static_cast<int>(isomot::KeySouth)] = configurationXML->keyboard().right();
    this->keys[static_cast<int>(isomot::KeyEast)] = configurationXML->keyboard().up();
    this->keys[static_cast<int>(isomot::KeyWest)] = configurationXML->keyboard().down();
    this->keys[static_cast<int>(isomot::KeyTake)] = configurationXML->keyboard().take();
    this->keys[static_cast<int>(isomot::KeyJump)] = configurationXML->keyboard().jump();
    this->keys[static_cast<int>(isomot::KeyShoot)] = configurationXML->keyboard().shoot();
    this->keys[static_cast<int>(isomot::KeyTakeAndJump)] = configurationXML->keyboard().takeandjump();
    this->keys[static_cast<int>(isomot::KeySwap)] = configurationXML->keyboard().swap();
    this->keys[static_cast<int>(isomot::KeyHalt)] = configurationXML->keyboard().halt();

    // Almacena los niveles de volumen
    this->soundFxVolume = configurationXML->volume().fx();
    this->musicVolume = configurationXML->volume().music();
  }
  catch(const xml_schema::exception& e)
  {
    std::cout << e << std::endl;
  }
}

void ConfigurationManager::write()
{
  try
  {
    cxml::keyboard userKeys(this->keys[static_cast<int>(isomot::KeyNorth)],
                            this->keys[static_cast<int>(isomot::KeySouth)],
                            this->keys[static_cast<int>(isomot::KeyEast)],
                            this->keys[static_cast<int>(isomot::KeyWest)],
                            this->keys[static_cast<int>(isomot::KeyTake)],
                            this->keys[static_cast<int>(isomot::KeyJump)],
                            this->keys[static_cast<int>(isomot::KeyShoot)],
                            this->keys[static_cast<int>(isomot::KeyTakeAndJump)],
                            this->keys[static_cast<int>(isomot::KeySwap)],
                            this->keys[static_cast<int>(isomot::KeyHalt)]);

    cxml::volume userVolume(this->soundFxVolume, this->musicVolume);

    // Creación de la configuración
    cxml::ConfigurationXML configurationXML(this->language, userKeys, userVolume);

    // Información del esquema
    xml_schema::namespace_infomap map;
    map[""].name = "";
    map[""].schema = "configuration.xsd";

    // Creación del archivo
    std::ofstream outputFile(fileName.c_str());
    cxml::configuration(outputFile, configurationXML, map);
  }
  catch(const xml_schema::exception& e)
  {
    std::cout << e << std::endl;
  }
}
