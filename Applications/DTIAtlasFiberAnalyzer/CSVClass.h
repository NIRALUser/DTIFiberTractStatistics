#ifndef _CSVCLASS_H_
#define _CSVCLASS_H_

//Standard librairies
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

typedef std::vector<std::string> vstring;
typedef std::vector<vstring> v2string;

class CSVClass
{
	public:
		CSVClass(std::string filename, bool debug=false);
		bool loadCSVFile(std::string csvfilename);
		void LineInVector(std::string line);
		void SaveFile(std::string filename);
		void clearData();
		std::vector< vstring >* getData();
		void AddData(std::string StringColumn, int row, int column);
		void SetFilename(std::string filename);
		std::string getFilename();
		unsigned int getRowSize();
		unsigned int getColSize(int row);
		
	private:
		/* debug */
		bool m_debug;
		
		/*vector with data */
		std::vector< vstring > m_data;
		
		/* Input */
		std::string m_csvfilename;
};

#endif
