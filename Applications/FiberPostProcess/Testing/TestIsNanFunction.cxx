#include <cmath>
#include <limits>
#ifdef _WIN32
#include <float.h>
#endif
int main( int argc , char* argv[] )
{
    float f = std::numeric_limits<float>::quiet_NaN() ;
#ifdef _WIN32
	if( _isnan( f ) == 0 )
    {
        return 1 ;
    }
#else
    if( std::isnan( f ) == 0 )
    {
        return 1 ;
    }
#endif
    return 0 ;
}
