#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
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
	std::cout<<"Reading CSV data ... "<< Filename << std::endl;
	//Reading data
	getline(File,Buffer,'\n');
	while(!File.eof())
	{
        //std::cout<< Buffer << std::endl;
		Line=StringToVector(Buffer);
		TempTable.push_back(Line);
		getline(File, Buffer,'\n');
	}
	std::cout<<"Reading done: "<< TempTable.size() << "x" << TempTable[0].size() << std::endl;
	//std::cout<<"more:" <<  TempTable[0][0] << "," << TempTable[0][1] << std::endl;
	
	
	
	//Check if the table is written by column or by row.
	if(!IsFloat(TempTable[0][1].c_str())) {
		TransposeCol=true;
        std::cout << "transposing needed" << std::endl;
	} else if(!IsFloat(TempTable[1][0].c_str())) {
		TransposeCol=false;
	} else {
		std::cout<<"Error finding to transpose csv table or not."<<std::endl;
    }
	
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
    
    
	// m_DataTable[0][0] => arclength title,  m_DataTable[0][1] << first arclength value etc
    std::string columnName = m_DataTable[0][0] ;
    std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
    if( columnName == "index" || columnName == "id" )
		m_Index=true;
	else
		m_Index=false;
    if( columnName == "arclength" )
		m_Arclength=true;
	else
		m_Arclength=false;
    
	std::cout<<"CSV File read successfuly. Index? "<< m_Index << ", Arclength? " << m_Arclength << std::endl;
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
	double LocalMin=10000;
	for(unsigned int i=1; i<m_DataTable[Column].size(); i++)
	{
		if(atof(m_DataTable[Column][i].c_str())<LocalMin)
			LocalMin=atof(m_DataTable[Column][i].c_str());
	}
	return LocalMin;
}

double Processing::GetMaxFromColumn(int Column)
{
	double LocalMax=-10000;
	for(unsigned int i=0; i<m_DataTable[Column].size(); i++)
	{
		if(atof(m_DataTable[Column][i].c_str())>LocalMax)
			LocalMax=atof(m_DataTable[Column][i].c_str());
	}
	return LocalMax;
}

/**********************************************************************************
*WritingDataInVTK: Complete the VTK file with data from CSV file
* Does NOT handle cropping of fibers at the moment, simply looks up fiber Index in vtkFiber
*   and 
***********************************************************************************/

void Processing::WritingDataInVTK(std::string output_vtk_file, double Min, double Max, double alpha , inverse_rescale_p_value )
{
	int NbParameters=m_DataTable.size();
	
	vtkCellArray* Lines=m_PolyData->GetLines();
	vtkIdType NumberOfPoints;
	vtkIdType* Ids;
	vtkDataArray* Scalars;
    if (m_Arclength)
        Scalars=m_PolyData->GetPointData()->GetScalars("SamplingDistance2Origin");
    else    
        Scalars=m_PolyData->GetPointData()->GetScalars("FiberLocationIndex");
    
    if (Scalars == NULL) 
    {
        std::cout << "Scalars with Name FiberLocationIndex/SamplingDistance2Origin not found, using first found scalar in vtk file" << std::endl;
        Scalars=m_PolyData->GetPointData()->GetScalars();
    } else {
        std::cout << "Scalars with Name FiberLocationIndex/SamplingDistance2Origin found" << std::endl;
    }

  double* Bounds=new double[2];
    if(Min==-1)
        Min=0;
    if(Max==-1)
        Max=100;
    
  for(int i=0; i<NbParameters; i++)
  {
    Bounds[0]=GetMinFromColumn(i);
    Bounds[1]=GetMaxFromColumn(i);
		vtkSmartPointer<vtkFloatArray> Parameter(vtkFloatArray::New());
		vtkSmartPointer<vtkFloatArray> ParameterSlicer(vtkFloatArray::New());
    // This line changes the dimension along which the data is written
    // Without it, it writes along dimension 2 (eg: 1 70397)
    // With it, it writes along dimension 1 (eg: 70397 1)
    //Parameter->SetNumberOfComponents(m_PolyData->GetNumberOfPoints());
    Lines->InitTraversal();
    for(int FiberId=0; Lines->GetNextCell(NumberOfPoints,Ids); FiberId++)
    {
      for(int PointId=0; PointId<NumberOfPoints; PointId++)
      {
        float FiberIndex_f = Scalars->GetComponent(0,Ids[PointId]);
        double Value;
        int FiberIndex ;
        if(m_Index)
        {
          FiberIndex=GetRealIndex((int)FiberIndex_f); // look up index
          Value=atof(m_DataTable[i][FiberIndex].c_str());
        }
        else if (m_Arclength)
        {
          FiberIndex=ArclengthToIndex(FiberIndex_f); // look up index
          Value=atof(m_DataTable[i][FiberIndex].c_str());
        }
        else
          Value=atof(m_DataTable[i][(int)FiberIndex_f+2].c_str());

        Parameter->InsertComponent(0,Ids[PointId],Value);

        if (alpha == -1 || i == 0)
        {
          // linearly rescales from Min to Max
          Value=(int)(((Value-Bounds[0])/(Bounds[1]-Bounds[0]))*(Max-Min)+Min) ;
        }
        else
        {
          if( inverse_rescale_p_value )
          {
            // linearly rescales 0..alpha from Max to Min
            if (Value > alpha)
            {
              Value = Min;
            }
            else
            {
              Value=(int)(Value/alpha*(Min-Max)+Max);
            }
          }
          {
            // linearly rescales 0..alpha from Min to Max
            if (Value > alpha)
            {
              Value = Max;
            }
            else
            {
              Value=(int)(Value/alpha*(Max-1-Min)+Min);//why is there a "-1"?
            }
          }
        }
        ParameterSlicer->InsertComponent(0,Ids[PointId],Value);
      }
    }
    Parameter->SetName(m_DataTable[i][0].c_str());
    m_PolyData->GetPointData()->AddArray(Parameter);
    ParameterSlicer->SetName((m_DataTable[i][0]+"_Slicer").c_str());
    m_PolyData->GetPointData()->AddArray(ParameterSlicer);
  }
  vtkSmartPointer<vtkPolyDataWriter> writer(vtkPolyDataWriter::New());
  writer->SetFileName(output_vtk_file.c_str());
  writer->SetInput(m_PolyData);
  writer->Update();

  delete Bounds;
}

//Should probably replace by a dichotomic search or anything more efficient
//than looking through the whole list that is sorted
int Processing::ArclengthToIndex(float Index)
{
  float min_dist = std::numeric_limits<float>::max() ;
  size_t pos = 0 ;
  float previous_value =-100000 ;
  for(size_t i = 1 ; i < m_DataTable[ 0 ].size() ; i++ )
  {
    float csvValue = atof(m_DataTable[0][i].c_str()) ;
    float dist = (csvValue -Index)*(csvValue -Index) ;
    if( min_dist > dist )
    {
      min_dist = dist ;
      pos = i ;
    }
    previous_value = csvValue ;
  }
  return pos ;
}

int Processing::GetRealIndex(int Index)
{
  for(unsigned int i=1; i<m_DataTable[0].size(); i++)
  {
    if(atoi(m_DataTable[0][i].c_str())==Index)
      return i;
  }
  return -1;
}
