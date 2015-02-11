#include "../csv.h"

int main( int argc , char* argv[] )
{
    csv csvTestInitRowsId ;
    std::vector< std::string > rowsId ;
    for( int i = 0 ; i < 3 ; i++ )
    {
        rowsId.push_back( "FIBER " + Convert( i ) ) ;
    }
    csvTestInitRowsId.initRowsId( rowsId ) ;
    for( int i = 0 ; i < 3 ; i++ )
    {
        if( (csvTestInitRowsId.getRowsId())[ i ] != rowsId[ i ] )
        {
            return 1 ;
        }
    }
    return 0 ;
}
