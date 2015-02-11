#include "../csv.h"

int main( int argc , char* argv[] )
{
    csv csvTestInitColsId ;
    std::vector< std::string > colsId ;
    for( int i = 0 ; i < 3 ; i++ )
    {
        colsId.push_back( "CELLS " + Convert( i ) ) ;
    }
    csvTestInitColsId.initColsId( colsId ) ;
    for( int i = 0 ; i < 3 ; i++ )
    {
        if( (csvTestInitColsId.getColsId())[ i ] != colsId[ i ] )
        {
            return 1 ;
        }
    }
    return 0 ;
}
