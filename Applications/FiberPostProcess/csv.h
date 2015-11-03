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

#endif
