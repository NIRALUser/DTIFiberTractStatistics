#include "../csv.h"

int main( int argc , char* argv[] )
{
    csv csvBaselineData ;
    csvBaselineData.read( "/NIRAL/work/jeanyves/FiberPostProcess/src/Testing/Data/initData.csv" ) ;
    csv csvTestInitData ;
    std::vector< std::vector< std::string > > data ;
    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back("Fiber File ") ;
    buff.push_back( logFileName ) ;
    data.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask Input:" ) ;
    data.push_back( buff ) ;
    buff.clear() ;
    csvTestInitData.initData( data ) ;
    if( csvTestInitData != csvBaselineData )
    {
        return 1 ;
    }
    return 0 ;
}
