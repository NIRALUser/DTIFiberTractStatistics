#include "../csv.h"

int main()
{
    csv csvBaselineHeader ;
    csvBaselineHeader.read( "/NIRAL/work/jeanyves/FiberPostProcess/src/Testing/Data/initHeader.csv" ) ;
    csv csvTestInitHeader ;
    std::vector< std::vector< std::string > > header ;
    std::vector< std::string > buff ;
    char logFileName[] = "log.csv" ;
    buff.push_back("Fiber File ") ;
    buff.push_back( logFileName ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    buff.push_back( "Mask Input:" ) ;
    header.push_back( buff ) ;
    buff.clear() ;
    csvTestInitHeader.initHeader( header ) ;
    if(csvTestInitHeader != csvBaselineHeader )
    {
        return 1 ;
    }
    return 0 ;
}
