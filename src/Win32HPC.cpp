#ifdef __WIN32
#include "Win32HPC.hpp"

HPC::HPC()
{
	// Inicializa todos los atributos
	frequency.QuadPart = 0;
	startTime.QuadPart = 0;
	stopTime.QuadPart = 0;
	period.QuadPart = 0;
}

bool HPC::start()
{
	QueryPerformanceCounter(&startTime);
	if(!QueryPerformanceFrequency(&frequency)) return false;
	// Pasa el dato a milisegundos
	frequency.QuadPart /= 1000;
	return true;
}

double HPC::getValue()
{
  LARGE_INTEGER timer;
  QueryPerformanceCounter(&timer);

	if(stopTime.QuadPart != 0)
	{
		return double((stopTime.QuadPart - startTime.QuadPart) / frequency.QuadPart) / 1000.0;
	}

  return double((timer.QuadPart - period.QuadPart - startTime.QuadPart) / frequency.QuadPart) / 1000.0;
}

void HPC::reset()
{
  LARGE_INTEGER timer, excess;

  QueryPerformanceCounter(&timer);
  excess.QuadPart = (timer.QuadPart - startTime.QuadPart) % frequency.QuadPart;
  QueryPerformanceCounter(&startTime);
  startTime.QuadPart -= excess.QuadPart;
	period.QuadPart = 0;
}

void HPC::stop()
{
	LARGE_INTEGER timer;
  QueryPerformanceCounter(&timer);

	stopTime.QuadPart = timer.QuadPart - period.QuadPart;
}

void HPC::restart()
{
	LARGE_INTEGER timer;
  QueryPerformanceCounter(&timer);

	period.QuadPart = (timer.QuadPart - stopTime.QuadPart);
	stopTime.QuadPart = 0;
}

#endif
