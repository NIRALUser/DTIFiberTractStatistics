#ifndef CSV_H
#define CSV_H

#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "utils.h"
#include <iostream>
#include <fstream>

class csv
{
    friend bool operator==( const csv &, const csv & );
    friend bool operator!=( const csv &, const csv & );
public:
    csv() ;
    csv( char* ) ;
    void read(char * ) ;
    void write(char * ) ;
    void initData( std::vector< std::vector< std::string > > vecData ) ;
    void initHeader( std::vector< std::vector< std::string > > headerData ) ;
    void initRowsId( std::vector< std::string > rowsTitle ) ;
    void initColsId( std::vector< std::string > colsTitle ) ;
    std::vector< std::string > getRowsId() ;
    std::vector< std::string > getColsId() ;
private:
    std::vector< std::vector< std::string > > header , data ;
    std::string delimiter ;
    std::vector< std::string > rowsId , colsId ;
};

csv::csv()
{
    delimiter = "," ;
    header= std::vector< std::vector< std::string > >() ;
    data = std::vector< std::vector< std::string > >() ;
    rowsId = std::vector< std::string >() ;
    colsId = std::vector< std::string >() ;
}

csv::csv( char* fname )
{
    read( fname ) ;
}

std::vector< std::string > csv::getRowsId()
{
    return rowsId ;
}

std::vector< std::string > csv::getColsId()
{
    return colsId ;
}
void csv::initData( std::vector< std::vector< std::string > > vecData )
{
    data.clear() ;
    for( int i = 0 ; i < vecData.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < vecData[ i ].size() ; j++ )
        {
            buff.push_back( Convert( vecData[ i ][ j ] ) ) ;
        }
        data.push_back( buff ) ;
    }
}

void csv::initHeader( std::vector< std::vector< std::string > > headerData )
{
    header.clear() ;
    for( int i = 0 ; i < headerData.size() ; i++ )
    {
        std::vector< std::string > buff ;
        for( int j = 0 ; j < headerData[ i ].size() ; j++ )
        {
            buff.push_back( Convert( headerData[ i ][ j ] ) ) ;
        }
        header.push_back( buff ) ;
    }
}

void csv::initRowsId( std::vector< std::string > rowsTitle )
{
    rowsId.clear() ;
    for( int i = 0 ; i < rowsTitle.size() ; i++ )
    {
        rowsId.push_back( rowsTitle[ i ] ) ;
    }
}

void csv::initColsId( std::vector< std::string > colsTitle )
{
    colsId.clear() ;
    for( int i = 0 ; i < colsTitle.size() ; i++ )
    {
        colsId.push_back( colsTitle[ i ] ) ;
    }
}

void csv::read( char* fname )
{

    std::ifstream in ;
    in.open( fname , std::ifstream::in ) ;
    std::string s = "" ;
    std::string element = "\n" ;
    data.clear() ;
    int count= 0 ;
    char c ;
    std::string line ;
    std::vector< std::string > dataLine ;
    while( std::getline( in , line ) )
    {
        std::stringstream lineStream( line ) ;
        std::string cell ;
        while( std::getline(lineStream , cell , delimiter[0]) )
        {
            dataLine.push_back( cell ) ;
        }
        data.push_back( dataLine ) ;
        dataLine.clear() ;
    }
    in.close() ;
}

void csv::write( char* fname )
{
    std::ofstream out ;
    out.open( fname , std::ofstream::out |std::ofstream::trunc ) ;
    if( header.empty() )
    {
    }
    else
    {
        for( int i = 0 ; i < header.size() ; i++ )
        {
            for( int j = 0 ; j < header[ i ].size() ; j++ )
            {
                out << header[ i ][ j ] ;
                if( j!= header[ i ].size() - 1 )
                {
                    out << delimiter ;
                }
            }
            out << std::endl ;
        }
        out.close() ;
        out.open( fname , std::ofstream::out |std::ofstream::app ) ;
    }
    if( !colsId.empty() )
    {
        for( int k = 0 ; k < colsId.size() ; k++ )
        {
            out << colsId[ k ] ;
            if( k!= colsId.size() - 1 )
            {
                out << delimiter ;
            }
        }
        out << std::endl ;
    }
    for( int i = 0 ; i < data.size() ; i++ )
    {
        if( !rowsId.empty() )
        {
            out << rowsId[ i ] ;
            out << delimiter ;
        }
        for( int j = 0 ; j < data[ i ].size() ; j++ )
        {
            out << data[ i ][ j ] ;
            if( j!= data[ i ].size() - 1 )
            {
                out << delimiter ;
            }
        }
        out << std::endl ;
    }
    out.close() ;
}

bool operator==( const csv &d1, const csv &d2 )
{
    std::vector< std::vector< std::string > > d1Content ;
    d1Content = d1.header ;
    std::vector< std::vector< std::string > > d2Content ;
    d2Content = d2.header ;
    for( int i = 0 ; i < d1.data.size() ; i++ )
    {
        d1Content.push_back( d1.data[i] ) ;
    }
    for( int i = 0 ; i < d2.data.size() ; i++ )
    {
        d2Content.push_back( d2.data[i] ) ;
    }
    if( d1Content.size() != d2Content.size() )
    {
        return false ;
    }
    for( int i = 0 ; i < d2Content.size() ; i++ )
    {
        if( d1Content[ i ].size() != d2Content[ i ].size() )
        {
            std::cout << " d1.data[" << i << "].size() = " << d1Content[ i ].size() << " d2.data[" << i << "].size() = " << d2Content[ i ].size() << std::endl ;
            return false ;
        }
        for( int j = 0 ; j < d2Content[ i ].size() ; j++ )
        {
            if( d1Content[ i ][ j ] != d2Content[ i ][ j ] )
            {
                std::cout << " d1.data[i][j].size() = " << d1Content[ i ][ j ].size() << " d2.data[i][j].size() = " << d2Content[ i ][ j ].size() << std::endl ;
                return false ;
            }
        }
    }
    return true ;
}

bool operator!=( const csv &d1, const csv &d2 )
{
    return !( d1 == d2 ) ;
}
#endif
