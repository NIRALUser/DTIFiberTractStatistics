#include "../csv.h"

int main( int argc , char* argv[] )
{
    csv csvBaselineDataFull ;
    csvBaselineDataFull.read( "/NIRAL/work/jeanyves/FiberPostProcess/src/Testing/Data/initDataFull.csv" ) ;
    csv csvTestInitDataFull ;
    std::vector< std::vector< std::string > > header ;
    std::vector< std::vector< std::string > > data ;

    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back("Fiber File ") ;
    buff.push_back( logFileName ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask Input:" ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "test00" ) ;
    buff.push_back( "test01" ) ;
    data.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "test10" ) ;
    buff.push_back( "test11" ) ;
    data.push_back( buff ) ;
    buff.clear() ;
    csvTestInitDataFull.initHeader( header ) ;
    csvTestInitDataFull.initData( data ) ;
    if( csvTestInitDataFull != csvBaselineDataFull )
    {
        return 1 ;
    }
    return 0 ;
}
