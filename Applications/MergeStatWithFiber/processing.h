#ifndef PROCESSING
#define PROCESSING

#include <iostream>
#include <fstream>
#include <vector>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkObject.h>
#include <vtkCellArray.h>
#include <vtkFieldData.h>


typedef std::vector<std::string> vstring;
typedef std::vector<vstring> v2string;

class Processing{

	public:
		Processing();
		~Processing();
		
		void ReadDataFromCSV(std::string Filename);
		void ReadDataFromVTK(std::string Filename);
		void WritingDataInVTK(std::string output_vtk_file, double Min, double Max);
		vstring StringToVector(std::string line);
		vstring GetColumn(v2string data, int column);
		double GetMaxFromColumn(int Column);
		double GetMinFromColumn(int Column);
		int GetRealIndex(int Index);
		
	private:
		v2string m_DataTable;
		vtkSmartPointer<vtkPolyData> m_PolyData;
		bool m_Index;
};

#endif
