/** MergeStatWithFiber 

Command line : ./MergeStatWithFiber --vtk_input filename.vtk --csv_input filename.csv --output_vtk_file filename.vtk --interpolation_min 0 --interpolation_max 100

**/

#include <iostream>
#include <fstream>
#include <vector>

#include "MergeStatWithFiberCLP.h"
#include "processing.h"

int main(int argc, char* argv[])
{
	PARSE_ARGS;
	
	//Checking syntax and files.
	if(vtk_input=="")
	{
		std::cout<<"Please provide a .vtk input file."<<std::endl;
		return -1;
	}
	else
	{
		if(vtk_input.rfind(".vtk") != std::string::npos)
		{
			std::ifstream filecheck;
			filecheck.open(vtk_input.c_str(), std::ios::in);
			if(!filecheck.good())
			{
				std::cout<<"Unable to open .vtk input file. Exit without results."<<std::endl;
				return -1;
			}
			filecheck.close();
		}
		else
			std::cout<<"Vtk input file is not a .vtk file"<<std::endl;
	}
	
	if(csv_input=="")
	{
		std::cout<<"Please provide a .csv input file."<<std::endl;
		return -1;
	}
	else
	{
		if(csv_input.rfind(".csv") != std::string::npos)
		{
			std::ifstream filecheck;
			filecheck.open(csv_input.c_str(), std::ios::in);
			if(!filecheck.good())
			{
				std::cout<<"Unable to open .csv input file. Exit without results."<<std::endl;
				return -1;
			}
			filecheck.close();
		}
		else
			std::cout<<"Csv input file is not a .csv file"<<std::endl;
	}
	
	if(output_vtk_file=="")
	{
		std::cout<<"Please provide the output file."<<std::endl;
		return -1;
	}
	else
	{
		if(output_vtk_file.rfind(".vtk") != std::string::npos)
		{
			std::ofstream filecheck;
			filecheck.open(output_vtk_file.c_str(), std::ios::out);
			if(!filecheck.good())
			{
				std::cout<<"Unable to open the output file. Exit without results."<<std::endl;
				return -1;
			}
			filecheck.close();
		}
		else
			std::cout<<"Output file is not a .vtk file."<<std::endl;
	}
	
	//Processing function
	Processing* process=new Processing;
	process->ReadDataFromCSV(csv_input);
	process->ReadDataFromVTK(vtk_input);
	process->WritingDataInVTK(output_vtk_file, interpolation_min, interpolation_max, p_value_alpha);
	
	
}
