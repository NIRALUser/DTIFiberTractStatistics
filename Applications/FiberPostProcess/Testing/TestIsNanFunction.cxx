#include <cmath>


int main( int argc , char* argv[] )
{
    float f = 0.0 / 0.0 ;
    if( std::isnan( f ) == 0 )
    {
        return 1 ;
    }
    return 0 ;
}
