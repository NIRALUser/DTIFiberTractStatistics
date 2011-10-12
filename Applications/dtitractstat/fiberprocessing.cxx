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

void fiberprocessing::fiberprocessing_main(std::string input_file, bool planeautoOn, std::string plane_str, bool worldspace, std::string auto_plane_origin)
{
  GroupType::Pointer group = readFiberFile(input_file);
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
  arc_length_parametrization(group,worldspace, spacing, offset);
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
    std::cout<<"a size is "<<a.size()<<"b size is "<<b.size()<<std::endl;
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
	  std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<tmp_parameter<<" a is "<<a<<" param_name is "<<param_name<<std::endl;
	  if (Siequals(tmp_parameter, param_name))
	    {
	      std::cout<<"param_name is"<<param_name<<"parameter_std_list[a] is "<<parameter_std_list[a]<<"a is"<<a<<std::endl;
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
      parametrized_position.push_back(itk::Vector<double, 4>());
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
      //add x,y,z information to vectors all by YUNDI SHI
      for (int pt_index = 0;pt_index<3;++pt_index)
	{
	  parametrized_position[l_counter][pt_index] = p1[pt_index];
	}
      parametrized_position[l_counter][4] = fiber_counter;
      
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
	parametrized_position.push_back(itk::Vector<double, 4>());
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
	for (int pt_index = 0;pt_index<3;++pt_index)
	  {
	    parametrized_position[l_counter][pt_index] = p1[pt_index];
	  }
	parametrized_position[l_counter][4] = fiber_counter;
	l_counter++;
	count++;
      }
      
      //to find the total distance from origin to the current sample
      double cumulative_distance_2 = 0.0;
      for (pit = pit_temp; pit < (pointlist.end()-1); ++pit)
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
	parametrized_position.push_back(itk::Vector<double, 4>());
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
	for (int pt_index = 0;pt_index<3;++pt_index)
	  {
	    parametrized_position[l_counter][pt_index] = p1[pt_index];
	  }
	parametrized_position[l_counter][4] = fiber_counter;
	l_counter++;
	count++;
      }
      
    }
    
    //dealing with opposite oriented fibers
    else 
    {
      int count=1;
      
      // For each point along the fiber
      for(pit = pointlist.end(); pit != pointlist.begin(); --pit)
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
      for (pit = pointlist.end(); pit != pointlist.begin(); --pit)
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
      parametrized_position.push_back(itk::Vector<double, 4>());
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
      for (int pt_index = 0;pt_index<3;++pt_index)
	{
	  parametrized_position[l_counter][pt_index] = p_inter[pt_index];
	}
      parametrized_position[l_counter][4] = fiber_counter;
	
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
	parametrized_position.push_back(itk::Vector<double, 4>());
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
	for (int pt_index = 0;pt_index<3;++pt_index)
	  {
	    parametrized_position[l_counter][pt_index] = p1[pt_index];
	  }
	parametrized_position[l_counter][4] = fiber_counter;
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
	parametrized_position.push_back(itk::Vector<double, 4>());
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
	for (int pt_index = 0;pt_index<3;++pt_index)
	  {
	    parametrized_position[l_counter][pt_index] = p1[pt_index];
	  }
	parametrized_position[l_counter][4] = fiber_counter;
	pit--;
	l_counter++;
	count++;
      }
      
    }
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
  }
  else if(auto_plane_origin=="median")
  {
	  //Finding Plane origin as the average x,y,z of middle points for each fibers
	  int median=0;
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
  }
  plane_origin[0]=sum_x/num_points;
  plane_origin[1]=sum_y/num_points;
  plane_origin[2]=sum_z/num_points;
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
      fiberwriter->SetFileTypeToBinary();
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
	pt.AddField("AD",ad);
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

void fiberprocessing::Write_parametrized_fiber(std::string input_file, std::string output_parametrized_fiber_file, double step_size)
{

  std::cout<<"Reading Fiber"<<std::endl;
  GroupType::Pointer group = readFiberFile(input_file);
  ChildrenListType* children = group->GetChildren(0);
  ChildrenListType::iterator it;

  
  int l_length = all.size();
  std::cout<<"size of length is "<<l_length;
  
  double min_length = find_min_dist(); 
  double max_length=find_max_dist();	//since length[][] is sorted 
  int reg_counter = ceil((max_length - min_length +1)/step_size);	//note: min_length is negative

  //Variables
  double fiber_distance[reg_counter]; //distance along the fiber
  //initializing 
  for (int k=0;k<reg_counter;k++)
  {
    fiber_distance[k] = min_length+k*step_size;
  }
 
  // writing the fibers
  vtkSmartPointer<vtkPolyData> polydata(vtkPolyData::New());
  vtkSmartPointer<vtkIdList> ids(vtkIdList::New());
  vtkSmartPointer<vtkPoints> pts(vtkPoints::New());
  vtkSmartPointer<vtkIntArray> fiberindex(vtkIntArray::New());
  
  fiberindex->SetName("fiberindex");
  fiberindex->SetNumberOfTuples(0);
  ids->SetNumberOfIds(0);
  pts->SetNumberOfPoints(0);
  polydata->Allocate();
  
  //loop through all the fibers
  int fiber_counter = 0; //counter of the fiber
  int l_counter = 0;
  
  for(it = (children->begin()); it != children->end() ; it++)
  {
    int fiber_loc_index = 0; //index of fiber location 
    vtkIdType currentId = ids->GetNumberOfIds();
    while (parametrized_position[l_counter][4] == fiber_counter)
    {
      for (int i=0; i<reg_counter; i++)
	{
	  vtkIdType id;
	  double range_min = min_length + i * step_size;
	  double range_max = min_length + (i + 1) * step_size;
	  if (fiber_distance[i] <= range_max && fiber_distance[i] >= range_min)
	    {
	      fiberindex->InsertNextTuple1(fiber_loc_index);
	      id = pts->InsertNextPoint(parametrized_position[l_counter][0],
					parametrized_position[l_counter][1],
					parametrized_position[l_counter][2]);
	  
	      ids->InsertNextId(id);
	      l_counter++;
	    }
	}
      fiber_loc_index ++ ;
      }
    polydata->InsertNextCell(VTK_POLY_LINE, reg_counter, ids->GetPointer(currentId));
      
  }
  polydata->GetPointData()->SetScalars(fiberindex);
  vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
  fiberwriter->SetFileTypeToASCII();
  fiberwriter->SetFileName(output_parametrized_fiber_file.c_str());
  fiberwriter->SetInput(polydata);
  fiberwriter->Update();
}

double fiberprocessing::find_min_dist()
{
  l_counter = all.size();
  double min=100000;
  for (int i=0; i<l_counter; i++)
    {
      if (all[i][0]<min)
	min=all[i][0];
    }

  return(min);
}

double fiberprocessing::find_max_dist()
{
  l_counter = all.size();
  double max=-100000;
  for (int i=0; i<l_counter; i++)
    {
      if (all[i][0]>max)
	max=all[i][0];
    }
  return(max);
}
