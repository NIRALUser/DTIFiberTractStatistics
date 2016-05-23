#ifndef TFATOOL_H
#define TFATOOL_H
#include <QString>
#include <QFileInfo>
#include <QtDebug>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cstddef>
#include <cctype>
#include <locale>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <csvparser.h>

using namespace std;

namespace tool{


     // to-do: struct naming and usage are problematic
     struct TractData
     {
        QString file_path;
        QString subjectID;
     };
     struct MapData
     {
         QString t12_path;
         QString def_path;
         QString subjectID;
         QString mapID;
     };

     int getvtkfiles (string dir, vector<tool::TractData> &files);
     bool checkExecutable(string path);
     bool checkExecutable(char* path);
     bool checkNewLine(string &s);
     bool checkDirExist(string path);
     void tokenize(char* str, const char* delimiter, vector<string> &results);
     void parseMapContent(QString filename, map<string,TractData> &data, string header1, string header2);
     vector<vector<string> >  parseCSV(string dir, vector<string> &attrs);
     string syscall(const char* cmd);

     // trim from start
     inline std::string &ltrim(std::string &s)
     {
             s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
             return s;
     }

     // trim from end
     inline std::string &rtrim(std::string &s)
     {
             s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
             return s;
     }

     // trim from both ends
     inline std::string &trim(std::string &s)
     {
             return ltrim(rtrim(s));
     }

}



#endif // TFATOOL_H
