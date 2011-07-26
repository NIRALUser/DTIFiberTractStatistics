#include "CSVClass.h"


/********************************************************************************* 
 * Constructor
 ********************************************************************************/
CSVClass::CSVClass(std::string filename, bool debug)
{
	m_csvfilename = filename;
	m_debug = debug;
}

/********************************************************************************* 
 * load a CSV file in a vector<vector<string>>
 ********************************************************************************/
bool CSVClass::loadCSVFile(std::string csvfilename)
{
	m_csvfilename = csvfilename;
	std::ifstream file(csvfilename.c_str(), std::ios::in); //open the file in reading
	if(file)
	{
		std::string line;
		
		while(getline(file, line))
		{
			LineInVector(line);
		}
		file.close();
	}
	else
	{
		std::cout<<"The CSV file doesn't exist!"<<std::endl;
		return false;
	}
	
	if(m_debug)
	{
		std::cout<<"Loaded Data : "<<std::endl;
		for(unsigned int i = 0; i<m_data.size();i++)
			for(unsigned int j = 0; j<m_data[i].size();j++)
				std::cout<<m_data[i][j]<<std::endl;
	}
	
	return true;
}

/********************************************************************************* 
 * Copy the line in the vector of data
 ********************************************************************************/
void CSVClass::LineInVector(std::string line)
{
	//create vstring
	vstring column;
	//Iterator on the string line
	std::string::iterator it;
	std::string word;
	it = line.begin();
	//add each word or number between two "," in the vector
	while(it != line.end())
	{
		//if the line is at the last word, set the iterator at the end of the string
		if(line.find_first_of(",")!=std::string::npos)
		{
			word = line.substr(0, line.find_first_of(","));
			column.push_back(word);
			it = line.begin();
			//erase the first word in the line
			line = line.substr(line.find_first_of(",")+1,line.size()-line.find_first_of(",")+1);
		}
		else
		{
			column.push_back(line);
			it = line.end();
		}
	}
	m_data.push_back(column);
}


/*********************************************************************************
 * Save the CSV file
 ********************************************************************************/
void CSVClass::SaveFile(std::string filename)
{
	/* Open a file in writing */
  std::ofstream bfile(filename.c_str(), std::ios::out);
	
	if(bfile)
	{
		/* Write line per line from the data */
		for(unsigned int i = 0; i<m_data.size();i++)
		{
			for(unsigned int j = 0; j<m_data[i].size();j++)
			{
				if(j!=(m_data[i].size()-1))
					bfile << m_data[i][j] << "," ;
				else
					bfile << m_data[i][j];
			}
			bfile << std::endl;
		}
		
		bfile.close();
	}
	else
		std::cout<<"ERROR: Open the file for saving CSV!"<<std::endl;
}

/*********************************************************************************
 * Clear the vector
 ********************************************************************************/
void CSVClass::clearData()
{
	if(!m_data.empty())
	{
		for(unsigned int i = 0; i<m_data.size();i++)
			m_data[i].clear();
		m_data.clear();
	}
	else if(m_debug)
		std::cout<<"No data!"<<std::endl;
}


/*********************************************************************************
 * Add a value in a column. -1 = last column
 ********************************************************************************/
void CSVClass::AddData(std::string StringColumn, int row, int column)
{
	if(column==-1)
		m_data[row].push_back(StringColumn);
	else
		m_data[row].at(column)=StringColumn;
}


/********************************************************************************* 
 * Get the vector of data
 ********************************************************************************/
std::vector< vstring >* CSVClass::getData()
{
	return &m_data;
}

/********************************************************************************* 
 * Get the name of the csv file
 ********************************************************************************/
void CSVClass::SetFilename(std::string filename)
{
	m_csvfilename=filename;
}

/********************************************************************************* 
 * Get the name of the csv file
 ********************************************************************************/
std::string CSVClass::getFilename()
{
	return m_csvfilename;
}

/********************************************************************************* 
 * Get the number of row of the csv file
 ********************************************************************************/
unsigned int CSVClass::getRowSize()
{
	return m_data.size();
}

/********************************************************************************* 
 * Get the number of column of the csv file
 ********************************************************************************/
unsigned int CSVClass::getColSize(int row)
{
	return m_data[row].size();
}
