#ifndef PROCESSING
#define PROCESSING

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
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
#include <vtkVersion.h>
#include <limits>

typedef std::vector<std::string> vstring;
typedef std::vector<vstring> v2string;

class Processing{

	public:
		Processing();
		~Processing();
		
		bool IsFloat(std::string);
		int ReadDataFromCSV(std::string Filename);
		void ReadDataFromVTK(std::string Filename);
    void WritingDataInVTK(std::string output_vtk_file, double Min, double Max, double alpha , bool inverse_rescale_p_value) ;
		vstring StringToVector(std::string line);
		vstring GetColumn(v2string data, int column);
		double GetMaxFromColumn(int Column);
		double GetMinFromColumn(int Column);
        int GetRealIndex(int Index);
        int ArclengthToIndex(float Index);
		
	private:
		v2string m_DataTable;
		vtkSmartPointer<vtkPolyData> m_PolyData;
    bool m_Index;
    bool m_Arclength;
};

#endif
