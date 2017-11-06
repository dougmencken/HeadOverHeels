
#ifndef __WIN32

#include "LinuxHPC.hpp"


HPC::HPC()
{
        trestart.tv_sec = 0;
        trestart.tv_usec = 0;
        tstop.tv_sec = 0;
        tstop.tv_usec = 0;
}

HPC::~HPC()
{

}

void HPC::start()
{
        gettimeofday( &trestart, &tz );
}

double HPC::getValue()
{
        gettimeofday( &tstop, &tz );

        double t1 = double( trestart.tv_sec ) + double( trestart.tv_usec ) / 1000000;
        double t2 = double( tstop.tv_sec ) + double( tstop.tv_usec ) / 1000000;

        return t2 - t1;
}

void HPC::reset()
{
        gettimeofday( &trestart, &tz );
}

void HPC::stop()
{
        gettimeofday( &tstop, &tz );
}

void HPC::restart()
{
        gettimeofday( &tstop, &tz );
}

#endif
