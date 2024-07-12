
#include "ColorCyclingLabel.hpp"

#include "Color.hpp"
#include "UnicodeUtilities.hpp"

#include <iostream>


namespace gui
{

ColorCyclingLabel::ColorCyclingLabel( const std::string & text, bool doubleHeight )
        : Label( text, new Font( "white", doubleHeight ) )
        , colorCyclingTimer( new Timer () )
{
        colorCyclingTimer->go();
}

/* static */ const double ColorCyclingLabel::single_interval = 0.2 ;

std::vector< std::string > ColorCyclingLabel::getColorCycle () const
{
        double time = this->colorCyclingTimer->getValue() ;
        unsigned int cycle = static_cast< unsigned int >( time * ( 1 / ColorCyclingLabel::single_interval ) );

        // the original speccy sequence “ cyan magenta green ”
        static const size_t numberOfColors = 3 ;
        static const std::string sequence[ numberOfColors ] = {  "cyan", "magenta", "green"  };

        std::vector< std::string > colorCycle ;
        colorCycle.push_back( sequence[ cycle % numberOfColors ] );
        colorCycle.push_back( sequence[ ( cycle + 1 ) % numberOfColors ] );
        colorCycle.push_back( sequence[ ( cycle + 2 ) % numberOfColors ] );

        return colorCycle ;
}

}
