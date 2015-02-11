#include "utils.h"

std::string ExtensionOfFile( std::string fileName )
{
    std::string extension ;
    extension = fileName.substr( fileName.find_last_of( "." ) + 1 ,
                                 fileName.size() - fileName.find_last_of( "." ) + 1 ) ;
    return extension ;
}

std::string ChangeEndOfFileName (std::string fileName, std::string change )
{
    std::string extension = ExtensionOfFile( fileName ) ;
    fileName.replace( fileName.end() - extension.length() - 1 , fileName.end() , change ) ;
    return fileName;
}

std::vector< std::vector < std::string  > > ConvertArray( std::vector< std::vector< float > > array )
{
    std::vector< std::vector < std::string  > > buffer ;
    for( int i = 0 ; i < array.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < array[i].size() ; j++ )
        {
            buff.push_back( Convert( array[ i ][ j ] ) ) ;
        }
        buffer.push_back( buff ) ;
    }
    return buffer ;
}


int FindMaxNbOfCols( std::vector< std::vector< std::string > > vecData )
{
    int max = 0 ;
    for( int i = 0 ; i < vecData.size() ; i++ )
    {
        if( vecData[ i ].size() > max )
        {
            max = vecData[ i ].size() ;
        }
    }
    return max ;
}
