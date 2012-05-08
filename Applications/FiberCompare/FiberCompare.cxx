#include <iostream>
#include <fstream>

#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>

#include <FiberCompareCLP.h>


std::vector<double> FillDistanceTable(vtkSmartPointer<vtkPolyData> Source, vtkSmartPointer<vtkPolyData> Target)
{
	std::vector<double> DistanceTable;
	vtkPoints* SourcePoints = Source->GetPoints();
	vtkPoints* TargetPoints = Target->GetPoints();
	
	for(int i=0; i<SourcePoints->GetNumberOfPoints(); i++)
	{
		double SourcePoint[3], DistanceMin = 10000;
		SourcePoints->GetPoint(i,SourcePoint);
		for(int j=0; j<TargetPoints->GetNumberOfPoints(); j++)
		{
			double TargetPoint[3], XYZ[3], Distance;
			TargetPoints->GetPoint(j,TargetPoint);
			for(int k=0; k<3; k++)
				XYZ[k] = TargetPoint[k]-SourcePoint[k];
			Distance = sqrt(XYZ[0]*XYZ[0]+XYZ[1]*XYZ[1]+XYZ[2]*XYZ[2]);
			if(Distance<DistanceMin)
				DistanceMin = Distance;
		}
		DistanceTable.push_back(DistanceMin);
	}
	return DistanceTable;
}

void GetBounds(std::vector<std::vector<double> > DistanceTable, double Bounds[2])
{
	double Min = 100000, Max = -1;
	for(unsigned int i=0; i<DistanceTable.size(); i++)
	{
		for(unsigned int j=0; j<DistanceTable[i].size(); j++)
		{
			if(DistanceTable[i][j]<Min)
				Min = DistanceTable[i][j];
			if(DistanceTable[i][j]>Max)
				Max = DistanceTable[i][j];
		}
	}
	Bounds[0] = Min;
	Bounds[1] = Max;
}

int GetFrequency(std::vector<std::vector<double> > DistanceTable, double IntervalMin, double IntervalMax)
{
	int Frequency=0;
	for(unsigned int i=0; i<DistanceTable.size(); i++)
	{
		for(unsigned int j=0; j<DistanceTable[i].size(); j++)
		{
			if(DistanceTable[i][j]<IntervalMax && DistanceTable[i][j]>=IntervalMin)
				Frequency++;
		}
	}
	return Frequency;
}

double GetMeanDistance(std::vector<double> Distance, std::vector<int> Frequency)
{
	double MeanDistance=0, TotalFrequency=0;
	for(unsigned int i=0; i<Distance.size(); i++)
	{
		MeanDistance+=Distance[i]*Frequency[i];
		TotalFrequency+=Frequency[i];
	}
	MeanDistance/=TotalFrequency;
	
	return MeanDistance;
}

int main(int argc, char* argv[])
{
	PARSE_ARGS;
	
	std::vector<vtkSmartPointer<vtkPolyData> > FiberTracts;
	std::vector<std::string> Filenames;
	
	std::cout<<"Reading VTK data..."<<std::endl;
	
	Filenames.push_back(vtk_input1);
	Filenames.push_back(vtk_input2);
	for(int i=0; i<2; i++)
	{
		if(Filenames[i].rfind(".vtk") != std::string::npos)
		{
			vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
			reader->SetFileName(Filenames[i].c_str());
			FiberTracts.push_back(reader->GetOutput());
			reader->Update();
		}
		else if(Filenames[i].rfind(".vtp") != std::string::npos)
		{
			vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
			reader->SetFileName(Filenames[i].c_str());
			FiberTracts.push_back(reader->GetOutput());
			reader->Update();
		}
		else
		{
			std::cout<<"Wrong filename : "<<Filenames[i]<<" Check file format and location."<<std::endl;
			return 0;
		}
	}
	std::cout<<Filenames[0]<<" "<<FiberTracts[0]->GetNumberOfCells()<<" fibers, "<<FiberTracts[0]->GetPoints()->GetNumberOfPoints()<<" points."<<std::endl;
	std::cout<<Filenames[1]<<" "<<FiberTracts[1]->GetNumberOfCells()<<" fibers, "<<FiberTracts[1]->GetPoints()->GetNumberOfPoints()<<" points."<<std::endl;
	std::cout<<"VTK Files read successfuly."<<std::endl<<std::endl;
	
	std::cout<<"Calculating..."<<std::endl;
			
	std::vector<std::vector<double> > DistanceTable;
	DistanceTable.push_back(FillDistanceTable(FiberTracts[0],FiberTracts[1]));
	DistanceTable.push_back(FillDistanceTable(FiberTracts[1],FiberTracts[0]));
			
	double Bounds[2];
	GetBounds(DistanceTable,Bounds);
	int TotalNumberOfFibers=FiberTracts[0]->GetPoints()->GetNumberOfPoints()+FiberTracts[1]->GetPoints()->GetNumberOfPoints();
			
	std::ofstream StatFile(output_stat_file.c_str(), std::ios::out);
	if(StatFile)
	{
		double IntervalMin = Bounds[0], IntervalMax = Bounds[0] + step;
		std::vector<int> FrequencyVector;
		std::vector<double> DistanceVector;
		
		while(IntervalMin<=Bounds[1])
		{
			int Frequency = GetFrequency(DistanceTable, IntervalMin, IntervalMax);
			DistanceVector.push_back(IntervalMin);
			FrequencyVector.push_back(Frequency);
			IntervalMin += step;
			IntervalMax += step;
		}
		
		double Dist25=-1, Dist50=-1, Dist75=-1, Dist90=-1, Dist95=-1;
		double CumulatedFrequency=0;
		
		StatFile<<"Filename,Number of fibers,Number of points"<<std::endl;
		StatFile<<Filenames[0]<<","<<FiberTracts[0]->GetNumberOfCells()<<","<<FiberTracts[0]->GetPoints()->GetNumberOfPoints()<<std::endl;
		StatFile<<Filenames[1]<<","<<FiberTracts[1]->GetNumberOfCells()<<","<<FiberTracts[1]->GetPoints()->GetNumberOfPoints()<<std::endl<<std::endl;
		
		StatFile<<"Distance,Frequency"<<std::endl;
		for(unsigned int i=0; i<DistanceVector.size(); i++)
		{
			StatFile<<DistanceVector[i]<<","<<FrequencyVector[i]<<std::endl;
			CumulatedFrequency+=FrequencyVector[i];
			if(CumulatedFrequency>=TotalNumberOfFibers*0.25 && Dist25==-1)
				Dist25=DistanceVector[i];
			if(CumulatedFrequency>=TotalNumberOfFibers*0.5 && Dist50==-1)
				Dist50=DistanceVector[i];
			if(CumulatedFrequency>=TotalNumberOfFibers*0.75 && Dist75==-1)
				Dist75=DistanceVector[i];
			if(CumulatedFrequency>=TotalNumberOfFibers*0.9 && Dist90==-1)
				Dist90=DistanceVector[i];
			if(CumulatedFrequency>=TotalNumberOfFibers*0.95 && Dist95==-1)
				Dist95=DistanceVector[i];
		}
		
		StatFile<<std::endl;
		StatFile<<"Frequency Percentage,Distance"<<std::endl;
		StatFile<<"25%,"<<Dist25<<std::endl;
		StatFile<<"50%,"<<Dist50<<std::endl;
		StatFile<<"75%,"<<Dist75<<std::endl;
		StatFile<<"90%,"<<Dist90<<std::endl;
		StatFile<<"95%,"<<Dist95<<std::endl;
		StatFile<<std::endl;
		
		for(unsigned int i=0; i<methods.size(); i++)
		{
			if(methods[i] == "Hausdorff")
			{
				std::cout<<"	Hausdorff..."<<std::endl;
				
				StatFile<<"Hausdorff,"<<Bounds[1]<<std::endl;
				
				std::cout<<"	End of Hausdorff."<<std::endl;
			}
			else if(methods[i] == "Mean")
			{
				std::cout<<"	Mean..."<<std::endl;
				
				StatFile<<"Mean,"<<GetMeanDistance(DistanceVector,FrequencyVector)<<std::endl;
				
				std::cout<<"	End of Mean."<<std::endl;
			}
			else
			{
				std::cout<<"Wrong method or syntax for argument : "<<methods[i]<<std::endl;
				std::cout<<"Ignoring argument."<<std::endl;
			}
		}
	StatFile.close();
	}
	else
		std::cout<<"ERROR: Unable to save output stat file."<<std::endl;
	
	std::cout<<"Calculation complete."<<std::endl;
	
	return 0;
}

