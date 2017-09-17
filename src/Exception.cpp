
#include "Exception.hpp"


namespace isomot
{

Exception::Exception( const std::string& string ) :
        message( string )
{
}

Exception::~Exception() throw ()
{
}

const char* Exception::what() const throw ()
{
        return message.c_str ();
}

void Exception::raise() const
{
        throw *this;
}

Exception* Exception::clone() const
{
        return new Exception( *this );
}

ENullPointer::ENullPointer() : Exception( "null pointer found" )
{
        std::cout << __FILE__ << ":" << __LINE__ << std::endl ;
}

}
