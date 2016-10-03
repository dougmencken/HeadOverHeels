#include "Ism.hpp"

namespace isomot
{

void sleep(unsigned long miliseconds)
{
#ifdef __WIN32
  Sleep(miliseconds);
#else
  std::modulus<unsigned long> modulus;
  unsigned long remainder = modulus(miliseconds, 1000);
  timespec pause;
  pause.tv_sec = miliseconds / 1000;
  pause.tv_nsec = remainder * 1000000;
  nanosleep(&pause, 0);
#endif
}

void copyFile(const std::string& source, const std::string& destination)
{
  std::ifstream inputStream(source.c_str());
  std::ofstream outputStream(destination.c_str());
  std::string temp;

  while(getline(inputStream, temp))
  {
    outputStream << temp << std::endl;
  }
}

std::string HomePath;
std::string homePath()
{
  if(HomePath.empty())
  {
#ifdef __WIN32
      HomePath = sharePath();
#else
      char* cpath = getenv("HOME");
      assert(cpath != 0);
      HomePath = std::string(cpath) + "/.headoverheels/";
      if(!file_exists(HomePath.c_str(), FA_DIREC, 0))
      {
        mkdir(HomePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        mkdir((HomePath + "savegame/").c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        copyFile(sharePath() + "configuration.xsd", HomePath + "configuration.xsd");
        copyFile(sharePath() + "configuration.xml", HomePath + "configuration.xml");
        copyFile(sharePath() + "savegame/savegame.xsd", HomePath + "savegame/savegame.xsd");
      }
#endif
  }

  return HomePath;
}

std::string SharePath;
std::string sharePath()
{
  if(SharePath.empty())
  {
    char cpath[256];
    get_executable_name(cpath, 256);
    char* filename = get_filename(cpath);
    SharePath = std::string(cpath, strlen(cpath) - strlen(filename) - strlen("bin/"));
    SharePath += "share/headoverheels/";
  }

  return SharePath;
}

}
