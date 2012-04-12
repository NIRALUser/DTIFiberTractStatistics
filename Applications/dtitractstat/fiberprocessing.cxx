#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
	
#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkFloatArray.h>

#include "fiberprocessing.h"

fiberprocessing::fiberprocessing()
{
  l_counter=0;
  plane_origin.Fill(0);plane_normal.Fill(0);
}

fiberprocessing::~fiberprocessing()
{}

void fiberprocessing::fiberprocessing_main(std::string& input_file, std::string& output_file, bool planeautoOn, std::string plane_str, bool worldspace, std::string auto_plane_origin, bool useNonCrossingFibers)
{
	GroupType::Pointer group = readFiberFile(input_file), Group=GroupType::New();
  itk::Vector<double,3> spacing = group->GetSpacing();
  itk::Vector<double,3> offset = group->GetObjectToParentTransform()->GetOffset();
  
  if (planeautoOn)
  {
    cout<<"Finding the plane origin and normal automatically\n\n";
	 find_plane(group , auto_plane_origin);
  }
  else 
  {
    bool plane_defined = read_plane_details(plane_str, spacing, offset, worldspace);
    if (!plane_defined)
    {
      cout<<"Could not read plane file::finding the plane origin and normal automatically\n\n";
		find_plane(group, auto_plane_origin);
    }
    cout<<"return from read_plane_file function :"<<plane_defined<<plane_origin<<plane_normal<<endl;
  }
  if(!useNonCrossingFibers)
  {
	  vtkSmartPointer<vtkPolyData> PolyData;
	  PolyData=RemoveNonCrossingFibers(input_file);
	  std::string extension=ExtensionofFile(input_file);
	  std::string plane_name=takeoffExtension(takeoffPath(plane_str));
	  input_file=takeoffExtension(output_file)+"_"+plane_name+"_Clean."+extension;
	  if (input_file.rfind(".vtk") != std::string::npos)
	  {
			vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
			fiberwriter->SetFileTypeToBinary();
			fiberwriter->SetFileName(input_file.c_str());
			fiberwriter->SetInput(PolyData);
			fiberwriter->Update();
	  }
	  else if(input_file.rfind(".vtp") != std::string::npos)
	  {
		  vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
		  fiberwriter->SetFileName(input_file.c_str());
		  fiberwriter->SetInput(PolyData);
		  fiberwriter->Update();
	  }
	  group=readFiberFile(input_file);
  }
  
  arc_length_parametrization(group,worldspace, spacing, offset);
}

std::string fiberprocessing::takeoffExtension(std::string filename)
{
	std::string inputfile;
	inputfile = filename.substr(0,filename.find_first_of("."));
	return inputfile;
}

std::string fiberprocessing::takeoffPath(std::string filename)
{
	std::string inputfile;
	inputfile = filename.substr(filename.find_last_of("/")+1,filename.size()-filename.find_last_of("/")+1);
	return inputfile;
}

std::string fiberprocessing::ExtensionofFile(std::string filename)
{
	std::string extension;
	extension = filename.substr(filename.find_last_of(".")+1,filename.size()-filename.find_last_of(".")+1);
	return extension;
}

vtkSmartPointer<vtkPolyData> fiberprocessing::RemoveNonCrossingFibers(std::string Filename)
{
	vtkSmartPointer<vtkPolyData> PolyData;
	if (Filename.rfind(".vtk") != std::string::npos)
	{
		vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
		reader->SetFileName(Filename.c_str());
		PolyData=reader->GetOutput();
		reader->Update();
	}
	else if (Filename.rfind(".vtp") != std::string::npos)
	{
		vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New();
		reader->SetFileName(Filename.c_str());
		PolyData=reader->GetOutput();
		reader->Update();
	}
	std::cout<<"Total Number of Fibers : "<<PolyData->GetNumberOfCells()<<std::endl;
	
	vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
	vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
	NewTensors->SetNumberOfComponents(9);
	vtkDataArray* Tensors=PolyData->GetPointData()->GetTensors();
	
	vtkPoints* Points=PolyData->GetPoints();
	vtkCellArray* Lines=PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	int NewId=0;
	Lines->InitTraversal();
	
	bool Cross;
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		Cross=false;
		vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
		NewLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
		for(int j=0; j<NumberOfPoints-1; j++)
		{
			double p1[3], p2[3];
			Points->GetPoint(Ids[j],p1);
			Points->GetPoint(Ids[j+1],p2);
			
			p1[0]=plane_origin[0]-p1[0];
			p1[1]=plane_origin[1]-p1[1];
			p1[2]=plane_origin[2]-p1[2];
			
			p2[0]=plane_origin[0]-p2[0];
			p2[1]=plane_origin[1]-p2[1];
			p2[2]=plane_origin[2]-p2[2];
			
			double ScalarProduct1, ScalarProduct2;
			ScalarProduct1=p1[0]*plane_normal[0]+p1[1]*plane_normal[1]+p1[2]*plane_normal[2];
			ScalarProduct2=p2[0]*plane_normal[0]+p2[1]*plane_normal[1]+p2[2]*plane_normal[2];
			if(ScalarProduct1*ScalarProduct2<0)
			{
				Cross=true;
				break;
			}
		}
		if(Cross)
		{
			for(int j=0; j<NumberOfPoints; j++)
			{
				NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
				NewLine->GetPointIds()->SetId(j,NewId);
				NewId++;
				double tensorValue[9];
				for(int k=0; k<9; k++)
					tensorValue[k]=Tensors->GetComponent(Ids[j],k);
				NewTensors->InsertNextTuple(tensorValue);
			}
			NewLines->InsertNextCell(NewLine);
		}
	}
	FinalPolyData->SetPoints(NewPoints);
	FinalPolyData->GetPointData()->SetTensors(NewTensors);
	FinalPolyData->SetLines(NewLines);
	std::cout<<"Number of Fibers without non crossing fibers : "<<FinalPolyData->GetNumberOfCells()<<std::endl;
	return FinalPolyData;
}

void fiberprocessing::find_distance_from_plane(itk::Point<double, 3> pos, int index)
{
  float plane_norm = (sqrt((plane_normal[0]*plane_normal[0])+(plane_normal[1]*plane_normal[1])+(plane_normal[2]*plane_normal[2])));
  if (plane_norm != 1)
  {
    plane_normal[0] /= plane_norm;
    plane_normal[1] /= plane_norm;
    plane_normal[2] /= plane_norm;	
  }
  
  //using plane eq Ax + By + Cz +D =0; D= -Ax0 -By0 -Cz0;  d= abs (Ax1 + By1 + Cz1)/ plane_norm
  double D= - ((plane_normal[0] * plane_origin[0]) + (plane_normal[1] * plane_origin[1]) + (plane_normal[2] * plane_origin[2]));
  double d = (fabs ((plane_normal[0] * pos[0]) + (plane_normal[1] * pos[1]) + (plane_normal[2] * pos[2]) + D)) / plane_norm;
  if (d < closest_d)
  {
    closest_d = d;
    closest_point = pos;
  }
}

bool fiberprocessing::Siequals(std::string a, std::string b)
{
    unsigned int sz = a.size();
    //std::cout<<"a size is "<<a.size()<<"b size is "<<b.size()<<std::endl;
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

std::vector< std::vector<double> > fiberprocessing::get_arc_length_parametrized_fiber(std::string param_name)
{
  const char* parameter_std_list[] = {"fa", "md", "ad", "l2" , "l3", "fro" , "rd" ,"ga"};
  int parameter_index = 0; //which parameter was called
  if (param_name == "all")
    {
      cout<<"Total Number of sample points in the fiber bundle: "<<all.size()<<"...getting all parameters..."<<endl;
      return(all);
      
    }
  else
    {
      for (int a =0;a<8;a++)
	{
	  std::string tmp_parameter(parameter_std_list[a]);
	  //std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<tmp_parameter<<" a is "<<a<<" param_name is "<<param_name<<std::endl;
	  if (Siequals(tmp_parameter, param_name))
	    {
	      //std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<parameter_std_list[a]<<"a is"<<a<<std::endl;
	      parameter_index = a;
	      break;
	    }
	}
    }
  std::vector< std::vector<double> > length_temp;
  for (int i =0; i<l_counter; i++)
    {
      length_temp.push_back(std::vector<double>());
      length_temp[i].push_back(all[i][0]);
      length_temp[i].push_back(all[i][parameter_index+1]);
    }	
  cout<<"Total Number of sample points in the fiber bundle: "<<length_temp.size()<<"...getting parameter "<<parameter_std_list[parameter_index]<<endl;
  return(length_temp);

}

void fiberprocessing::arc_length_parametrization(GroupType::Pointer group, bool worldspace, itk::Vector<double,3> spacing, itk::Vector<double,3> offset)
{
  ChildrenListType* children = group->GetChildren(0);
  ChildrenListType::iterator it;
 //**********************************************************************************************************
  // For each fiber
  int count_opposite = 0;
  int fiber_counter = 0;
  for(it = (children->begin()); it != children->end() ; it++)
  {
    closest_d = 1000.0;		
    closest_point.Fill(0);      
    parametrized_position_dist.push_back(std::vector<double>());
    parametrized_position_x.push_back(std::vector<double>());
    parametrized_position_y.push_back(std::vector<double>());
    parametrized_position_z.push_back(std::vector<double>());
    
    int flag_orientation = 0, counter=0;
		
    DTIPointListType pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
    DTIPointListType::iterator pit,pit_temp,pit_tmp;
    //check fiber orientation
    pit_tmp = pointlist.begin();
    itk::Point<double, 3> position_first = (*pit_tmp).GetPosition();
    if (worldspace)
    {
      position_first[0] = (position_first[0] * spacing[0]) + offset[0];
      position_first[1] = (position_first[1] * spacing[1]) + offset[1];
      position_first[2] = (position_first[2] * spacing[2]) + offset[2];
    }
    pit_tmp = pointlist.end();
    pit_tmp--;
    itk::Point<double, 3> position_last = (*pit_tmp).GetPosition();
    if (worldspace)
    {
      position_last[0] = (position_last[0] * spacing[0]) + offset[0];
      position_last[1] = (position_last[1] * spacing[1]) + offset[1];
      position_last[2] = (position_last[2] * spacing[2]) + offset[2];
    }

    itk::Vector<double, 3> orient_vec = position_first-position_last;
    
    double dot_prod = (plane_normal[0]*orient_vec[0] + plane_normal[1]*orient_vec[1] + plane_normal[2]*orient_vec[2] );
    if (dot_prod<0)
    {
      //found fiber orientation as opposite
      flag_orientation =1;
      count_opposite +=1;
    }
    
    if (flag_orientation == 0)
    {
      
      int count=1;
      // For each point along the fiber
      for(pit = pointlist.begin(); pit != pointlist.end(); ++pit)
      {
	typedef DTIPointType::PointType PointType;
	itk::Point<double, 3> position = (*pit).GetPosition();
	if (worldspace)
	{
	  position[0] = (position[0] * spacing[0]) + offset[0];
	  position[1] = (position[1] * spacing[1]) + offset[1];
	  position[2] = (position[2] * spacing[2]) + offset[2];
	}
	
	find_distance_from_plane(position, counter);	
	counter++;
      }
      //getting an iterator at the intersection point
      for (pit = pointlist.begin(); pit != pointlist.end(); ++pit)
      {
	itk::Point<double, 3> position_o =(*pit).GetPosition();
	if (worldspace)
	{
	  position_o[0] = (position_o[0] * spacing[0]) + offset[0];
	  position_o[1] = (position_o[1] * spacing[1]) + offset[1];
	  position_o[2] = (position_o[2] * spacing[2]) + offset[2];
	}
	
	if (position_o == closest_point)
	{
	  pit_temp = pit;
	  break;
	}
	
      }

      //adding sample points AT the intersection point to avoid gap at origin
      all.push_back(std::vector<double>());
      all[l_counter].push_back(0.0);
      //add x,y,z information to vectors all by YUNDI SHI
      itk::Point<double, 3> p1;
      p1=(*pit).GetPosition();
      if (worldspace)
	{
	  p1[0] = (p1[0] * spacing[0]) + offset[0];
	  p1[1] = (p1[1] * spacing[1]) + offset[1];
	  p1[2] = (p1[2] * spacing[2]) + offset[2];
	}
      parametrized_position_dist[fiber_counter].push_back(0.0);
      parametrized_position_x[fiber_counter].push_back(p1[0]);
      parametrized_position_y[fiber_counter].push_back(p1[1]);
      parametrized_position_z[fiber_counter].push_back(p1[2]);

      all[l_counter].push_back((*pit).GetField(DTIPointType::FA));				
      all[l_counter].push_back((*pit).GetField("MD"));
      all[l_counter].push_back((*pit).GetField("AD"));
      all[l_counter].push_back((*pit).GetField("l2"));
      all[l_counter].push_back((*pit).GetField("l3"));
      all[l_counter].push_back((*pit).GetField("FRO"));				
      all[l_counter].push_back((*pit).GetField("RD"));	//RD
      all[l_counter].push_back((*pit).GetField("GA"));
     
      
      l_counter++;

      //to find the total distance from origin to the current sample
      //FROM BEGINNING OF FIBER TILL PLANE
      double cumulative_distance_1 = 0.0;
      for (pit = pit_temp; pit != pointlist.begin(); --pit)
      {
	//gives the distance between current and previous sample point
	double current_length=0.0;
			
	itk::Point<double, 3> p1;
	itk::Point<double, 3> p2;
	p1=(*pit).GetPosition();
	if (worldspace)
	{
	  p1[0] = (p1[0] * spacing[0]) + offset[0];
	  p1[1] = (p1[1] * spacing[1]) + offset[1];
	  p1[2] = (p1[2] * spacing[2]) + offset[2];
	}
	
	p2=(*(pit+1)).GetPosition();
	if (worldspace)
	{
	  p2[0] = (p2[0] * spacing[0]) + offset[0];
	  p2[1] = (p2[1] * spacing[1]) + offset[1];
	  p2[2] = (p2[2] * spacing[2]) + offset[2];
	}
	current_length = sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]));
	cumulative_distance_1 += current_length;
	
	//multiplying by -1 to make arc length on 1 side of plane as negative
	
	all.push_back(std::vector<double>());
	
	all[l_counter].push_back(-1 * cumulative_distance_1);
	all[l_counter].push_back((*pit).GetField(DTIPointType::FA));			
	all[l_counter].push_back((*pit).GetField("MD"));
	all[l_counter].push_back((*pit).GetField("AD"));
	all[l_counter].push_back((*pit).GetField("l2"));
	all[l_counter].push_back((*pit).GetField("l3"));
	all[l_counter].push_back((*pit).GetField("FRO"));//FRO
	all[l_counter].push_back((*pit).GetField("RD")); //RD
	all[l_counter].push_back((*pit).GetField("GA"));

	//add x,y,z information to vectors all by YUNDI SHI 
	parametrized_position_dist[fiber_counter].push_back(-1 * cumulative_distance_1);
	parametrized_position_x[fiber_counter].push_back(p1[0]);
	parametrized_position_y[fiber_counter].push_back(p1[1]);
	parametrized_position_z[fiber_counter].push_back(p1[2]);

	l_counter++;
	count++;
      }
      
      //to find the total distance from origin to the current sample
      double cumulative_distance_2 = 0.0;
      for (pit = pit_temp; pit < (pointlist.end()-1); pit++)
      {
	  //gives the distance between current and previous sample point
	double current_length=0.0;
	
	itk::Point<double, 3> p1;
	itk::Point<double, 3> p2;
	p1=(*pit).GetPosition();
	if (worldspace)
	{
	  p1[0] = (p1[0] * spacing[0]) + offset[0];
	  p1[1] = (p1[1] * spacing[1]) + offset[1];
	  p1[2] = (p1[2] * spacing[2]) + offset[2];
	}
	
	p2=(*(pit+1)).GetPosition();
	if (worldspace)
	{
	  p2[0] = (p2[0] * spacing[0]) + offset[0];
	  p2[1] = (p2[1] * spacing[1]) + offset[1];
	  p2[2] = (p2[2] * spacing[2]) + offset[2];
	}
	
	current_length = sqrt(((p1[0]-p2[0])*(p1[0]-p2[0]))+((p1[1]-p2[1])*(p1[1]-p2[1]))+((p1[2]-p2[2])*(p1[2]-p2[2])));
	
	cumulative_distance_2 += current_length;
	all.push_back(std::vector<double>());
	
	all[l_counter].push_back(cumulative_distance_2);

	all[l_counter].push_back((*pit).GetField(DTIPointType::FA));				
	all[l_counter].push_back((*pit).GetField("MD"));
	all[l_counter].push_back((*pit).GetField("AD"));
	all[l_counter].push_back((*pit).GetField("l2"));
	all[l_counter].push_back((*pit).GetField("l3"));
	all[l_counter].push_back((*pit).GetField("FRO"));				//AD
	all[l_counter].push_back((*pit).GetField("RD"));	//RD
	all[l_counter].push_back((*pit).GetField("GA"));
	
	//add x,y,z information to vectors all by YUNDI SHI 
	parametrized_position_dist[fiber_counter].push_back(cumulative_distance_2);
	parametrized_position_x[fiber_counter].push_back(p1[0]);
	parametrized_position_y[fiber_counter].push_back(p1[1]);
	parametrized_position_z[fiber_counter].push_back(p1[2]);

	l_counter++;
	count++;
      }
      
    }
    
    //dealing with opposite oriented fibers
    else 
    {
      int count=1;
      // For each point along the fiber
      for(pit = pointlist.end()-1; pit != pointlist.begin(); --pit)
      {
	typedef DTIPointType::PointType PointType;
	itk::Point<double, 3> position = (*pit).GetPosition();

	if (worldspace)
	{
	  position[0] = (position[0] * spacing[0]) + offset[0];
	  position[1] = (position[1] * spacing[1]) + offset[1];
	  position[2] = (position[2] * spacing[2]) + offset[2];
	}
	
	find_distance_from_plane(position, counter);		
	counter++;
      }
      
      //getting an iterator at the intersection point
      for (pit = pointlist.end()-1; pit != pointlist.begin(); --pit)
      {
	itk::Point<double, 3> position_o = (*pit).GetPosition();
	if (worldspace)
	{
	  position_o[0] = (position_o[0] * spacing[0]) + offset[0];
	  position_o[1] = (position_o[1] * spacing[1]) + offset[1];
	  position_o[2] = (position_o[2] * spacing[2]) + offset[2];
	}
	
	if (position_o == closest_point)
	{
	  pit_temp = pit;
	  break;
	}
	
      }
      
      //adding sample points AT the intersection point to avoid gap at origin
      all.push_back(std::vector<double>());
      
      all[l_counter].push_back(0.0);

      itk::Point<double, 3> p_inter;
      p_inter=(*pit).GetPosition();

      if (worldspace)
	{
	  p_inter[0] = (p_inter[0] * spacing[0]) + offset[0];
	  p_inter[1] = (p_inter[1] * spacing[1]) + offset[1];
	  p_inter[2] = (p_inter[2] * spacing[2]) + offset[2];
	}

      
      all[l_counter].push_back((*pit).GetField(DTIPointType::FA));			
      all[l_counter].push_back((*pit).GetField("MD"));
      all[l_counter].push_back((*pit).GetField("AD"));
      all[l_counter].push_back((*pit).GetField("l2"));
      all[l_counter].push_back((*pit).GetField("l3"));
      all[l_counter].push_back((*pit).GetField("FRO"));
      all[l_counter].push_back((*pit).GetField("RD"));	//RD
      all[l_counter].push_back((*pit).GetField("GA"));
      
      //add x,y,z information to vectors all by YUNDI SHI 
      parametrized_position_dist[fiber_counter].push_back(0.0);
      parametrized_position_x[fiber_counter].push_back(p_inter[0]);
      parametrized_position_y[fiber_counter].push_back(p_inter[1]);
      parametrized_position_z[fiber_counter].push_back(p_inter[2]);

     	
      l_counter++;
      
      
      //to find the total distance from origin to the current sample
      //FROM BEGINNING OF FIBER TILL PLANE
      double cumulative_distance_1 = 0.0;
      for (pit = pit_temp; pit < pointlist.end()-1;)
      {
	//gives the distance between current and previous sample point
	double current_length=0.0;
	
	itk::Point<double, 3> p1;
	itk::Point<double, 3> p2;
	p1=(*pit).GetPosition();
	if (worldspace)
	{
	  p1[0] = (p1[0] * spacing[0]) + offset[0];
	  p1[1] = (p1[1] * spacing[1]) + offset[1];
	  p1[2] = (p1[2] * spacing[2]) + offset[2];
	}
	
	p2=(*(pit+1)).GetPosition();
	if (worldspace)
	{
	  p2[0] = (p2[0] * spacing[0]) + offset[0];
	  p2[1] = (p2[1] * spacing[1]) + offset[1];
	  p2[2] = (p2[2] * spacing[2]) + offset[2];
	}
	current_length = sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]));
	cumulative_distance_1 += current_length;
	//multiplying by -1 to make arc length on 1 side of plane as negative
	all.push_back(std::vector<double>());
	
	all[l_counter].push_back(-1 * cumulative_distance_1);
	
	all[l_counter].push_back((*pit).GetField(DTIPointType::FA));			
	all[l_counter].push_back((*pit).GetField("MD"));
	all[l_counter].push_back((*pit).GetField("AD"));
	all[l_counter].push_back((*pit).GetField("l2"));
	all[l_counter].push_back((*pit).GetField("l3"));
	all[l_counter].push_back((*pit).GetField("FRO"));
	all[l_counter].push_back((*pit).GetField("RD"));	//RD
	all[l_counter].push_back((*pit).GetField("GA"));
      
	//add x,y,z information to vectors all by YUNDI SHI
	//add x,y,z information to vectors all by YUNDI SHI 
	parametrized_position_dist[fiber_counter].push_back(-1 * cumulative_distance_1);
	parametrized_position_x[fiber_counter].push_back(p1[0]);
	parametrized_position_y[fiber_counter].push_back(p1[1]);
	parametrized_position_z[fiber_counter].push_back(p1[2]);
	
	pit++;
	l_counter++;
	count++;
      }
      
      //to find the total distance from origin to the current sample
      double cumulative_distance_2 = 0.0;
      
      for (pit = pit_temp-1; pit > pointlist.begin(); )
      {
	//gives the distance between current and previous sample point
	double current_length=0.0;
	
	itk::Point<double, 3> p1;
	itk::Point<double, 3> p2;
	p1=(*pit).GetPosition();
	if (worldspace)
	{
	  p1[0] = (p1[0] * spacing[0]) + offset[0];
	  p1[1] = (p1[1] * spacing[1]) + offset[1];
	  p1[2] = (p1[2] * spacing[2]) + offset[2];
	}
	
	
	p2=(*(pit+1)).GetPosition();
	if (worldspace)
	{
	  p2[0] = (p2[0] * spacing[0]) + offset[0];
	  p2[1] = (p2[1] * spacing[1]) + offset[1];
	  p2[2] = (p2[2] * spacing[2]) + offset[2];
	}
	
	current_length = sqrt(((p1[0]-p2[0])*(p1[0]-p2[0]))+((p1[1]-p2[1])*(p1[1]-p2[1]))+((p1[2]-p2[2])*(p1[2]-p2[2])));
	cumulative_distance_2 += current_length;
	

	all.push_back(std::vector<double>());
	
	all[l_counter].push_back(cumulative_distance_2);

	all[l_counter].push_back((*pit).GetField(DTIPointType::FA));			
	all[l_counter].push_back((*pit).GetField("MD"));
	all[l_counter].push_back((*pit).GetField("AD"));
	all[l_counter].push_back((*pit).GetField("l2"));
	all[l_counter].push_back((*pit).GetField("l3"));
	all[l_counter].push_back((*pit).GetField("FRO"));
	all[l_counter].push_back((*pit).GetField("RD"));	//RD
	all[l_counter].push_back((*pit).GetField("GA"));
	
	//add x,y,z information to vectors all by YUNDI SHI 
	parametrized_position_dist[fiber_counter].push_back(cumulative_distance_2);
	parametrized_position_x[fiber_counter].push_back(p1[0]);
	parametrized_position_y[fiber_counter].push_back(p1[1]);
	parametrized_position_z[fiber_counter].push_back(p1[2]);

	pit--;
	l_counter++;
	count++;
      }
      
    }
    //std::cout<<"fiber_counter is"<<fiber_counter<<std::endl;
    // cout<<"size is "<<parametrized_position_z[fiber_counter].size()<<endl;
    fiber_counter++;
  }  
  
  cout<<"Total # of opposite oriented fibers:"<<count_opposite<<endl;
}

itk::Vector<double, 3> fiberprocessing::get_plane_origin()
{
  return(plane_origin);
}
itk::Vector<double, 3> fiberprocessing::get_plane_normal()
{
  return(plane_normal);
}


void fiberprocessing::find_plane(GroupType::Pointer group, std::string auto_plane_origin)
{
  ChildrenListType* pchildren = group->GetChildren(0);
  ChildrenListType::iterator it, it_closest;
  DTIPointListType pointlist;
  DTIPointListType::iterator pit, pit_closest;
  double sum_x=0,sum_y=0,sum_z=0;
  int num_points=0;
  
  if(auto_plane_origin=="cog")
  {
		//Finding Plane origin as the average x,y,z coordinate of the whole fiber bundle
	  for(it = (pchildren->begin()); it != pchildren->end() ; it++)
	  {
		  pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
		  for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
		  {
			  itk::Point<double, 3> position = (*pit).GetPosition();
			  num_points++;
			  sum_x=sum_x+position[0]; sum_y=sum_y+position[1]; sum_z=sum_z+position[2];
		  }
	  }
	  plane_origin[0]=sum_x/num_points;
	  plane_origin[1]=sum_y/num_points;
	  plane_origin[2]=sum_z/num_points;
  }
  else if(auto_plane_origin=="median")
  {
	  itk::Vector<double, 3> median_point;
	  //Finding Plane origin as the average x,y,z of middle points for each fibers
	  int median=0;
	  double distance_min=10000.0;
	  for(it=pchildren->begin(); it!=pchildren->end(); it++)
	  {
		  pointlist=dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
		  median=ceil((double)pointlist.size()/2.0);
		  pit=pointlist.begin();
		  pit+=median;
		  itk::Point<double,3> position=(*pit).GetPosition();
		  num_points++;
		  sum_x+=position[0]; sum_y+=position[1]; sum_z+=position[2];
	  }
	  median_point[0]=sum_x/num_points;
	  median_point[1]=sum_y/num_points;
	  median_point[2]=sum_z/num_points;
	  for(it=pchildren->begin(); it!=pchildren->end(); it++)
	  {
		  pointlist=dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
		  median=ceil((double)pointlist.size()/2.0);
		  pit=pointlist.begin();
		  pit+=median;
		  itk::Point<double,3> position=(*pit).GetPosition();
		  double distance=sqrt(pow(position[0]-median_point[0],2)+pow(position[1]-median_point[1],2)+pow(position[2]-median_point[2],2));
		  if(distance<distance_min)
		  {
			  distance_min=distance;
			  plane_origin[0]=position[0];
			  plane_origin[1]=position[1];
			  plane_origin[2]=position[2];
		  }
	  }
  }
  cout<<"\nCalculated Plane Origin (avg x,y,z): "<<plane_origin[0]<<","<<plane_origin[1]<<","<<plane_origin[2]<<endl;
		
  //Leaving a percent of the bundle end points, find the closest point on bundle to the plane origin (to avoid curved bundles getting ends as the closest point)

  int closest_d = 1000.0;
  itk::Point<double, 3> closest_point_coor;
  closest_point_coor.Fill(0.0);
  for(it = (pchildren->begin()); it != pchildren->end() ; it++)
    {
      pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();

      int percent_count = 1;
      for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
	{
	  //ignoring first 30% and last 30% of points along the fiber(Fiber-ends)
	  if (percent_count>floor(pointlist.size() * .3) && percent_count<floor(pointlist.size() * .7))	
	    {
	      itk::Point<double, 3> position = (*pit).GetPosition();
	      double dist =  sqrt((position[0]-plane_origin[0])*(position[0]-plane_origin[0])+(position[1]-plane_origin[1])*(position[1]-plane_origin[1])+(position[2]-plane_origin[2])*(position[2]-plane_origin[2]));
	      if (dist <= closest_d)
		{
		  closest_d = dist;
		  pit_closest = pit;
		  closest_point_coor[0] = (*pit).GetPosition()[0];
		  closest_point_coor[1] = (*pit).GetPosition()[1];
		  closest_point_coor[2] = (*pit).GetPosition()[2];
		  it_closest  = it;
		}
	    }
	  percent_count++;
	}
    }

  //Using 3 points to left and right of this closest point, find the plane normal
  itk::Point<double, 3> point_before, point_after;
  point_before.Fill(0);
  point_after.Fill(0);
  for(it = (pchildren->begin()); it != pchildren->end() ; it++)
    {
      if (it==it_closest)
	{
	  pointlist = dynamic_cast<DTITubeType*>((*it).GetPointer())->GetPoints();
	  for(pit = pointlist.begin(); pit != pointlist.end(); pit++)
	    {
	      if (pit==pit_closest)
		{
		  point_before = (*(--(--(--pit)))).GetPosition();
		  ++pit;++pit;++pit;
		  point_after = (*(++(++(++pit)))).GetPosition();
		}
	    }
	}
    }
  plane_normal=point_after-point_before;
  double plane_norm = (sqrt((plane_normal[0]*plane_normal[0])+(plane_normal[1]*plane_normal[1])+(plane_normal[2]*plane_normal[2])));
  if (plane_norm != 1)
    {
      plane_normal[0] /= plane_norm;
      plane_normal[1] /= plane_norm;
      plane_normal[2] /= plane_norm;	
    }
  cout<<"Plane normal:"<<plane_normal[0]<<","<<plane_normal[1]<<","<<plane_normal[2]<<endl;
}


bool fiberprocessing::read_plane_details(std::string plane_str, itk::Vector<double,3> spacing, itk::Vector<double,3> offset, bool worldspace)
{
  char extra[30];
  fstream plane;
  plane.open(plane_str.c_str(),fstream::in);
  if (!plane.is_open())
    return 0;

  if (worldspace)		//no need to adjust for spacing and offset
  {
    spacing.Fill(1);offset.Fill(0);
  }
  cout<<"Plane File successfully opened: "<<plane_str<<endl;
  while (plane.good())
  {
    //reading the plane origin
    for (int i=0;i<3;i++)
    {	
      //to discard the words: "Cut Plane Origin:"
      plane.getline(extra, 30, ' ');
    }
    for (int i=0;i<2;i++)
    {
      char value[30];
      plane.getline(value, 30, ' ');
      plane_origin[i] = (atof(value) - offset[i])/ spacing[i];
      if (value[0]=='\0' || value[0]==' ' || value[0]=='\t' || value[0]=='\n')
	i--;
    }
    char value[30];
    plane.getline(value, 30, '\n');
    plane_origin[2] = (atof(value) - offset[2])/ spacing[2];
    
    //reading the plane normal
    extra[0]='\0';
    for (int i=0;i<3;i++)
    {	
      //to discard the words: "Cut Plane normal:"
      plane.getline(extra, 30, ' ');
    }
    for (int i=0;i<2;i++)
    {
      char value[30];
      plane.getline(value, 30, ' ');
      plane_normal[i] = (atof(value) - offset[i])/ spacing[i];
      if (value[0]=='\0' || value[0]==' ' || value[0]=='\t' || value[0]=='\n')
	i--;
    }
    value[0]='\0';
    plane.getline(value, 30, '\n');
    plane_normal[2] = (atof(value) - offset[2])/ spacing[2];
    break;
  }
  plane.close();
  return 1;
  
}


void fiberprocessing::writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup)
{
  // Make sure origins are updated
  fibergroup->ComputeObjectToWorldTransform();

  // ITK Spatial Object
  if(filename.rfind(".fib") != std::string::npos)
  {
    typedef itk::SpatialObjectWriter<3> WriterType;
    WriterType::Pointer writer  = WriterType::New();
    writer->SetInput(fibergroup);
    writer->SetFileName(filename);
    writer->Update();
  }
  // VTK Poly Data
  else if (filename.rfind(".vt") != std::string::npos)
  {
    // Build VTK data structure
    vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
    vtkSmartPointer<vtkFloatArray> tensorsdata(vtkFloatArray::New());
    vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
    vtkSmartPointer<vtkPoints> pts(vtkPoints::New());

    tensorsdata->SetNumberOfComponents(9);
    polydata->SetPoints (pts);

    ids->SetNumberOfIds(0);
    pts->SetNumberOfPoints(0);
    polydata->Allocate();

    std::auto_ptr<ChildrenListType> children(fibergroup->GetChildren(0));
    typedef ChildrenListType::const_iterator IteratorType;

    for (IteratorType it = children->begin(); it != children->end(); it++)
    {
      itk::SpatialObject<3>* tmp = (*it).GetPointer();
      itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);
      unsigned int nPointsOnFiber = tube->GetNumberOfPoints();
      vtkIdType currentId = ids->GetNumberOfIds();

      for (unsigned int k = 0; k < nPointsOnFiber; k++)
      {
        itk::Point<double, 3> v(tube->GetPoint(k)->GetPosition());
        itk::Vector<double, 3> spacing(tube->GetSpacing());
        itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());

        // convert origin from LPS -> RAS
        origin[0] = -origin[0];
        origin[1] = -origin[1];

        vtkIdType id;
        // Need to multiply v by spacing and origin
        // Also negate the first to convert from LPS -> RAS
        // for slicer 3
        id = pts->InsertNextPoint(-v[0] * spacing[0] + origin[0],
                                  -v[1] * spacing[1] + origin[1],
                                  v[2] * spacing[2] + origin[2]);

        ids->InsertNextId(id);

        itk::DTITubeSpatialObjectPoint<3>* sopt = dynamic_cast<itk::DTITubeSpatialObjectPoint<3>* >(tube->GetPoint(k));
        float vtktensor[9];
        vtktensor[0] = sopt->GetTensorMatrix()[0];
        vtktensor[1] = sopt->GetTensorMatrix()[1];
        vtktensor[2] = sopt->GetTensorMatrix()[2];
        vtktensor[3] = sopt->GetTensorMatrix()[1];
        vtktensor[4] = sopt->GetTensorMatrix()[3];
        vtktensor[5] = sopt->GetTensorMatrix()[4];
        vtktensor[6] = sopt->GetTensorMatrix()[2];
        vtktensor[7] = sopt->GetTensorMatrix()[4];
        vtktensor[8] = sopt->GetTensorMatrix()[5];

        tensorsdata->InsertNextTupleValue(vtktensor);

      }
      polydata->InsertNextCell(VTK_POLY_LINE, nPointsOnFiber, ids->GetPointer(currentId));
    }

    polydata->GetPointData()->SetTensors(tensorsdata);

    // Legacy
    if (filename.rfind(".vtk") != std::string::npos)
    {
      vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
//       fiberwriter->SetFileTypeToBinary();
      fiberwriter->SetFileName(filename.c_str());
      fiberwriter->SetInput(polydata);
      fiberwriter->Update();
    }
    // XML
    else if (filename.rfind(".vtp") != std::string::npos)
    {
      vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
      fiberwriter->SetFileName(filename.c_str());
      fiberwriter->SetInput(polydata);
      fiberwriter->Update();
    }
    else
    {
      throw itk::ExceptionObject("Unknown file format for fibers");
    }
  }
  else
  {
    throw itk::ExceptionObject("Unknown file format for fibers");
  }
}

GroupType::Pointer fiberprocessing::readFiberFile(std::string filename)
{

  // ITK Spatial Object
  if(filename.rfind(".fib") != std::string::npos)
  {
    typedef itk::SpatialObjectReader<3, unsigned char> SpatialObjectReaderType;

    // Reading spatial object
    SpatialObjectReaderType::Pointer soreader = SpatialObjectReaderType::New();

    soreader->SetFileName(filename);
    soreader->Update();

    return soreader->GetGroup();	
  }
  // VTK Poly Data
  else if (filename.rfind(".vt") != std::string::npos)
  {
    // Build up the principal data structure for fiber tracts
    GroupType::Pointer fibergroup = GroupType::New();
 
    vtkSmartPointer<vtkPolyData> fibdata(NULL);

    // Legacy
    if (filename.rfind(".vtk") != std::string::npos)
    {
      vtkSmartPointer<vtkPolyDataReader> reader(vtkPolyDataReader::New());
      reader->SetFileName(filename.c_str());
      reader->Update();
      fibdata = reader->GetOutput();

    }
    else if (filename.rfind(".vtp") != std::string::npos)
    {
      vtkSmartPointer<vtkXMLPolyDataReader> reader(vtkXMLPolyDataReader::New());
      reader->SetFileName(filename.c_str());
      reader->Update();
      fibdata = reader->GetOutput();
    }
    else
    {
      throw itk::ExceptionObject("Unknown file format for fibers");
    }

    typedef  itk::SymmetricSecondRankTensor<double,3> ITKTensorType;
    typedef  ITKTensorType::EigenValuesArrayType LambdaArrayType;

    // Iterate over VTK data
    const int nfib = fibdata->GetNumberOfCells();
    int pindex = -1;
    for(int i = 0; i < nfib; ++i)
    {
      itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
      vtkSmartPointer<vtkCell> fib = fibdata->GetCell(i);

      vtkSmartPointer<vtkPoints> points = fib->GetPoints();

      typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;
      std::vector<DTIPointType> pointsToAdd;

      vtkSmartPointer<vtkDataArray> fibtensordata = fibdata->GetPointData()->GetTensors();
      for(int j = 0; j < points->GetNumberOfPoints(); ++j)
      {
        ++pindex;

        vtkFloatingPointType* coordinates = points->GetPoint(j);
        DTIPointType pt;
        // Convert from RAS to LPS for vtk
        pt.SetPosition(coordinates[0], coordinates[1], coordinates[2]);
        pt.SetRadius(0.5);
        pt.SetColor(0.0, 1.0, 0.0);

        vtkFloatingPointType* vtktensor = fibtensordata->GetTuple9(pindex);
        float floattensor[6];
        ITKTensorType itktensor;

        floattensor[0] = itktensor[0] = vtktensor[0];
        floattensor[1] = itktensor[1] = vtktensor[1];
        floattensor[2] = itktensor[2] = vtktensor[2];
        floattensor[3] = itktensor[3] = vtktensor[4];
        floattensor[4] = itktensor[4] = vtktensor[5];
        floattensor[5] = itktensor[5] = vtktensor[8];

        pt.SetTensorMatrix(floattensor);

        LambdaArrayType lambdas;

        // Need to do do eigenanalysis of the tensor
        itktensor.ComputeEigenValues(lambdas);

        float md = (lambdas[0] + lambdas[1] + lambdas[2])/3;
        float fa = sqrt(1.5) * sqrt((lambdas[0] - md)*(lambdas[0] - md) +
                                    (lambdas[1] - md)*(lambdas[1] - md) +
                                    (lambdas[2] - md)*(lambdas[2] - md))
          / sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);

        float logavg = (log(lambdas[0])+log(lambdas[1])+log(lambdas[2]))/3;

        float ga =  sqrt( SQ2(log(lambdas[0])-logavg) \
                          + SQ2(log(lambdas[1])-logavg) \
                          + SQ2(log(lambdas[2])-logavg) );

	float fro = sqrt(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);
	float ad = lambdas[2];
	float rd = (lambdas[0] + lambdas[1])/2;

        pt.AddField("FA",fa);
	pt.AddField("MD",md);
	pt.AddField("FRO",fro);
        pt.AddField("l2",lambdas[1]);
        pt.AddField("l3",lambdas[0]);
	pt.AddField("l1",ad);
	pt.AddField("RD",rd);
	pt.AddField("GA",ga);
	
        pointsToAdd.push_back(pt);
      }

      dtiTube->SetPoints(pointsToAdd);
      fibergroup->AddSpatialObject(dtiTube);
    }
    return fibergroup;
  } // end process .vtk .vtp
  else
  {
    throw itk::ExceptionObject("Unknown fiber file");
  }
}

void fiberprocessing::Write_parametrized_fiber(std::string input_file, std::string output_parametrized_fiber_file, double step_size, bool worldspace)
{

  GroupType::Pointer group = readFiberFile(input_file);
  ChildrenListType* children = group->GetChildren(0);
  //sort the parameter positions based on the cumulative distance
  sort_parameter(group);

  ChildrenListType::iterator it;
    
  double min_length = find_min_dist(); 
  double max_length = find_max_dist();	//since length[][] is sorted 
  int reg_length = ceil((max_length - min_length +1)/step_size);	//note: min_length is negative
  std::cout<<"reg_length is  "<<reg_length<<std::endl;
 
  // writing the fibers
  vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
  vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
  vtkSmartPointer<vtkPoints> pts(vtkPoints::New());
  vtkSmartPointer<vtkIntArray> fiberindex(vtkIntArray::New());
  vtkSmartPointer<vtkFloatArray> arclength(vtkFloatArray::New());
  polydata->SetPoints (pts);

  fiberindex->SetName("FiberLocationIndex");
  arclength->SetName("SamplingDistance2Origin");
  ids->SetNumberOfIds(0);
  pts->SetNumberOfPoints(0);

  polydata->Allocate();
  
  //loop through all the fibers
  int fiber_counter = 0; //counter of the fiber
  //int debugcounter = 0;
  double avglocation[4];
  int noptinwindow = 0;
  
  
  //initialize
  for (int pt_index = 0;pt_index<4;++pt_index)
    {
      avglocation[pt_index] = 0;
    }
  
  for(it = (children->begin()); it != children->end() ; it++)
  {
    //std::cout<<"debugcounter is "<<debugcounter<<std::endl;
    //debugcounter++;
    int real_no_pits_on_fiber = 0; 
    vtkIdType currentId = ids->GetNumberOfIds();
    int fiber_length = parametrized_position_dist[fiber_counter].size();
    int sampling_start=0;
    double range_min = min_length;
    double range_max =  min_length+step_size;
    //where to start sampling the data
	
    while (parametrized_position_dist[fiber_counter][0] < range_min || parametrized_position_dist[fiber_counter][0] > range_max){
      sampling_start++;
      
      range_min = min_length + sampling_start * step_size;
      range_max = min_length + (sampling_start + 1) * step_size;
    }
   
   
    //    std::cout<<"sampling starts at "<<sampling_start<<std::endl;
    int pos_counter = 0;
    for (int sampling_loc = sampling_start;sampling_loc<reg_length;sampling_loc++)
      {
	range_min = min_length + sampling_loc * step_size;
	range_max = min_length + (sampling_loc + 1) * step_size;
	//std::cout<<"three numbers are "<<range_min<<" "<<parametrized_position_dist[fiber_counter][pos_counter]<<" "<<range_max<<std::endl;
	for (int pt_index = 0;pt_index<4;++pt_index)
	  {
	    avglocation[pt_index] = 0;
	  }
	noptinwindow = 0;	    
	//stay in the window to average the postions
	while (pos_counter < fiber_length && parametrized_position_dist[fiber_counter][pos_counter] <= range_max && parametrized_position_dist[fiber_counter][pos_counter] >= range_min)
	  { 
	    //std::cout<<"Including data "<<pos_counter<<" for sampling location #"<<sampling_loc<<std::endl;
	    avglocation[3] = parametrized_position_dist[fiber_counter][pos_counter] + avglocation[3];
	    //getting location info in this sampling window for average
	    avglocation[0] = parametrized_position_x[fiber_counter][pos_counter] + avglocation[0];
	    avglocation[1] = parametrized_position_y[fiber_counter][pos_counter] + avglocation[1];
	    avglocation[2] = parametrized_position_z[fiber_counter][pos_counter] + avglocation[2];
	    
	    noptinwindow++; //number of points in the window
	    pos_counter++;
	  }
	
	//std::cout<<"pos_counter is "<<pos_counter<<" v.s. size is "<<fiber_length<<std::endl;
	if(noptinwindow > 0)
	  {
	    //	    std::cout<<"there are #"<<noptinwindow<<" pts for sampling location #"<<sampling_loc<<std::endl;
	    vtkIdType id;
	    real_no_pits_on_fiber ++;
	    fiberindex->InsertNextTuple1(sampling_loc);
	    arclength->InsertNextTuple1(avglocation[3]/noptinwindow);
	    id = pts->InsertNextPoint(avglocation[0]/noptinwindow,
				      avglocation[1]/noptinwindow,
				      avglocation[2]/noptinwindow);
	    //std::cout<<avglocation[0]/noptinwindow<<" "<<avglocation[1]/noptinwindow<<" "<<avglocation[2]/noptinwindow;
	    ids->InsertNextId(id);
	    //std::cout<<"three numbers are "<<range_min<<" "<<parametrized_position_dist[fiber_counter][pos_counter]<<" "<<range_max<<std::endl;	
	  }
	
	if (pos_counter == fiber_length-1)
	  {
	    //std::cout<<"pos_counter is "<<pos_counter<<endl;
	    break;
	  }
    }
    //    std::cout<<"real_no_pits_on_fiber is "<<real_no_pits_on_fiber<<std::endl;
    polydata->InsertNextCell(VTK_POLY_LINE,real_no_pits_on_fiber,ids->GetPointer(currentId));
    //    std::cout<<"goes to fiber #"<<fiber_counter<<std::endl;
    fiber_counter++;
  }

  std::cout<<"writing fiber"<<output_parametrized_fiber_file.c_str()<<std::endl;

  polydata->GetPointData()->SetActiveScalars("SamplingDistance2Origin");
  polydata->GetPointData()->SetScalars(arclength);
  polydata->GetPointData()->SetActiveScalars("FiberLocationIndex");
  polydata->GetPointData()->SetScalars(fiberindex);

  vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
  fiberwriter->SetFileTypeToASCII();
  fiberwriter->SetFileName(output_parametrized_fiber_file.c_str());
  fiberwriter->SetInput(polydata);
  fiberwriter->Update();
  
}

double fiberprocessing::find_min_dist()
{
  double min=100000;
  for (int i=0; i<all.size(); i++)
    {
      if (all[i][0]<min)
	min=all[i][0];
    }
  std::cout<<"min is "<<min<<endl;
  return(min);
}

double fiberprocessing::find_max_dist()
{
  double max=-100000;
  for (int i=0; i<all.size(); i++)
    {
      if (all[i][0]>max)
	max=all[i][0];
    }
  std::cout<<"max is "<<max<<endl;
  return(max);
}

void fiberprocessing::sort_parameter(GroupType::Pointer fibergroup)
{
  //sort parameter_x,y,z,dist based on dist
  ChildrenListType* children = fibergroup->GetChildren(0);
  ChildrenListType::iterator it;
  int fiber_counter = 0;

  for(it = (children->begin()); it != children->end() ; it++)
  {
    int fiber_length = parametrized_position_dist[fiber_counter].size();
    //    std::cout<<"fiber # "<<fiber_counter<<" has the length of "<<fiber_length<<std::endl;

    for (int i=0; i<fiber_length-1; i++) 
      {
	for (int j=0; j<fiber_length-1-i; j++)
	  {
	    if (parametrized_position_dist[fiber_counter][j+1] < parametrized_position_dist[fiber_counter][j]) 
	      {  /* compare the two neighbors */
		double tmp_dist = parametrized_position_dist[fiber_counter][j];         /* swap a[j] and a[j+1]      */
		double tmp_x = parametrized_position_x[fiber_counter][j];
		double tmp_y = parametrized_position_y[fiber_counter][j];
		double tmp_z = parametrized_position_z[fiber_counter][j];
		
		parametrized_position_dist[fiber_counter][j] = parametrized_position_dist[fiber_counter][j+1];
		parametrized_position_x[fiber_counter][j] = parametrized_position_x[fiber_counter][j+1];
		parametrized_position_y[fiber_counter][j] = parametrized_position_y[fiber_counter][j+1];
		parametrized_position_z[fiber_counter][j] = parametrized_position_z[fiber_counter][j+1];
		
		parametrized_position_dist[fiber_counter][j+1] = tmp_dist;
		parametrized_position_x[fiber_counter][j+1] = tmp_x;
		parametrized_position_y[fiber_counter][j+1] = tmp_y;
		parametrized_position_z[fiber_counter][j+1] = tmp_z;
	      }
	  }
      }
    fiber_counter++;
  }
}
