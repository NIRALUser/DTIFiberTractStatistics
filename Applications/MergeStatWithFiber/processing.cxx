#include <iostream>
#include <fstream>
#include <vector>

#include "processing.h"


Processing::Processing()
{
}

Processing::~Processing()
{
}

bool Processing::IsFloat(std::string Str)
{
	std::istringstream iss( Str );
	float temp;
	return (iss >> temp) && (iss.eof());
}

/**********************************************************************************
*ReadDataFromCSV: Look on the CSV file, read the data and put it in m_DataTable
***********************************************************************************/

void Processing::ReadDataFromCSV(std::string Filename)
{
	bool TransposeCol;
	std::ifstream File(Filename.c_str(), std::ios::in);
	v2string TempTable;
	vstring Line;
	std::string Buffer;
	std::cout<<"Reading CSV data..."<<std::endl;
	//Reading data
	getline(File, Buffer);
	while(!File.eof())
	{
		Line=StringToVector(Buffer);
		TempTable.push_back(Line);
		getline(File, Buffer);
	}
	
	
	
	//Check if the table is written by column or by row.
	if(!IsFloat(TempTable[0][1].c_str()))
		TransposeCol=true;
	else if(!IsFloat(TempTable[1][0].c_str()))
		TransposeCol=false;
	else
		std::cout<<"Error finding to transpose csv table or not."<<std::endl;
	
	//Checking if data were read correctly
	if(TempTable.size()==0)
	{
		std::cout<<"Error reading the CSV file. No data were obtained."<<std::endl;
		return;
	}
	else if(TempTable[0].size()==0)
	{
		std::cout<<"Error reading the CSV file. No data were obtained."<<std::endl;
		return;
	}
	
	m_DataTable.clear();
	
	//Saving data on m_DataTable
	if(TransposeCol)
	{
		//Transposing tables.
		for(unsigned int i=0; i<TempTable[0].size(); i++)
		{
			Line=GetColumn(TempTable, i);
			m_DataTable.push_back(Line);
		}		
	}
	else
		m_DataTable=TempTable;
	
	if(m_DataTable[0][0]=="Index" || m_DataTable[0][0]=="Id")
		m_Index=true;
	else
		m_Index=false;
	
	std::cout<<"CSV File read successfuly."<<std::endl;
}

/**********************************************************************************
*StringToVector: Convert a string to an std::vector<std::string>
***********************************************************************************/

vstring Processing::StringToVector(std::string line)
{
	vstring  column;
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
	return column;
}

/**********************************************************************************
*GetColumn: Extract a column from a data table.
***********************************************************************************/

vstring Processing::GetColumn(v2string data, int column)
{
	vstring line;
	for(unsigned int i=0; i<data.size(); i++)
		line.push_back(data[i][column]);
	return line;
}

/**********************************************************************************
*ReadDataFromVTK: Look on the VTK file, read the data and put it in m_PolyData
***********************************************************************************/

void Processing::ReadDataFromVTK(std::string Filename)
{
	std::cout<<"Reading VTK data..."<<std::endl;
	vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
	reader->SetFileName(Filename.c_str());
	if(reader->IsFilePolyData())
	{
		reader->Update();
		m_PolyData=reader->GetOutput();
		std::cout<<"VTK File read successfuly."<<std::endl;
	}
	else
		std::cout<<"Error reading VTK File. Check VTK Format."<<std::endl;
}

double Processing::GetMinFromColumn(int Column)
{
	double Min=10000;
	for(unsigned int i=1; i<m_DataTable[Column].size(); i++)
	{
		if(atof(m_DataTable[Column][i].c_str())<Min)
			Min=atof(m_DataTable[Column][i].c_str());
	}
	return Min;
}

double Processing::GetMaxFromColumn(int Column)
{
	double Max=-10000;
	for(unsigned int i=0; i<m_DataTable[Column].size(); i++)
	{
		if(atof(m_DataTable[Column][i].c_str())>Max)
			Max=atof(m_DataTable[Column][i].c_str());
	}
	return Max;
}

/**********************************************************************************
*WritingDataInVTK: Complete the VTK file with data from CSV file
***********************************************************************************/

void Processing::WritingDataInVTK(std::string output_vtk_file, double Min, double Max)
{
	int NbParameters=m_DataTable.size()-1;
	
	vtkCellArray* Lines=m_PolyData->GetLines();
	vtkIdType NumberOfPoints;
	vtkIdType* Ids;
	vtkDataArray* Scalars=m_PolyData->GetPointData()->GetScalars();
	for(int i=0; i<NbParameters; i++)
	{
		if(Min==-1)
			Min=GetMinFromColumn(i+1);
		if(Max==-1)
			Max=GetMaxFromColumn(i+1);
		vtkSmartPointer<vtkFloatArray> Parameter(vtkFloatArray::New());
		vtkSmartPointer<vtkFloatArray> ParameterSlicer(vtkFloatArray::New());
		Parameter->SetNumberOfComponents(m_PolyData->GetNumberOfPoints());
		Lines->InitTraversal();
		for(int FiberId=0; Lines->GetNextCell(NumberOfPoints,Ids); FiberId++)
		{
			for(int PointId=0; PointId<NumberOfPoints; PointId++)
			{
				int FiberIndex=(int)Scalars->GetComponent(0,Ids[PointId]);
				double Value;
				if(m_Index)
				{
					FiberIndex=GetRealIndex(FiberIndex);
					Value=atof(m_DataTable[i+1][FiberIndex].c_str());
				}
				else
					Value=atof(m_DataTable[i][FiberIndex+1].c_str());
				Parameter->InsertComponent(0,Ids[PointId],Value);
				Value=((Value-Min)/(Max-Min))*100;
				ParameterSlicer->InsertComponent(0,Ids[PointId],Value);
			}
		}
		Parameter->SetName(m_DataTable[i+1][0].c_str());
		m_PolyData->GetPointData()->AddArray(Parameter);
		ParameterSlicer->SetName((m_DataTable[i+1][0]+"_Slicer").c_str());
		m_PolyData->GetPointData()->AddArray(ParameterSlicer);
	}
	vtkSmartPointer<vtkPolyDataWriter> writer(vtkPolyDataWriter::New());
	writer->SetFileName(output_vtk_file.c_str());
	writer->SetInput(m_PolyData);
	writer->Update();
	
	
}

int Processing::GetRealIndex(int Index)
{
	for(unsigned int i=0; i<m_DataTable[0].size(); i++)
	{
		if(atoi(m_DataTable[0][i].c_str())==Index)
			return i;
	}
	return -1;
}
