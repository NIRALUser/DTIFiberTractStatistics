#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <cstring>
#include <iostream>
#include <exception>

// simple csv parser dedicated for the use of this project
// issue: the parser is not comprehensively checking integrity of csv files.

using namespace std;
class csvparser
{
public:
    struct InvalidData : public exception{
        const char* what() const throw(){
           return "Inconsistent number of columns found in the csv file!";
        }
    };

    struct ReadError: public exception{
        const char* what() const throw(){
           return "Failed to read the csv file!";
        }
    };


    struct MatchError: public exception{
        const char* what() const throw(){
           return "Failed to match headers in the csv file!";
        }
    };

    csvparser(QString filepath);
    ~csvparser();
    void read(string header1, string header2);
    bool next_line(char buf[], qint64 size);
    bool read_row(char* a, char* b);
    void close();


private:
    QFile* file;
    int h1;
    int h2;
    size_t columns;

};

#endif // CSVPARSER_H
