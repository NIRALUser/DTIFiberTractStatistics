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
	
#include "dtitypes.h"
#include "argio.h"
#include "fiberprocessing.h"
#include "regression.h"
#include "dtitractstatCLP.h"


int main(int argc, char* argv[])
{
  PARSE_ARGS;
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  if(input_fiber_file=="")
    {
      std::cout<<"Please provide the input fiber file name (.vtk or .fib format)"<<std::endl;
      return -1;}
  else
    {
      fstream filecheck;
      filecheck.open(input_fiber_file.c_str(),fstream::in);
      if (!filecheck.good())
	{
	  std::cout<<"Unable to open the input fiber file...exit without results!\n"<<std::endl;
	  return -1;
	}
      filecheck.close();
    }

  if(output_stats_file=="")
    {
      std::cout<<"Please provide the output file name (.fvp format) "<<std::endl;
      return -1;
    }
  else
    {
      fstream filecheck;
      filecheck.open(output_stats_file.c_str(),fstream::out);
      if (!filecheck.good())
	{
	  std::cout<<"Unable to open the output file...exit without results!\n"<<std::endl;
	  return -1;
	}
      filecheck.close();
    }

  //Cut Plane definition
  bool planeautoOn = false;
  if(plane_file == "")
    {
      std::cout<<"No plane file is specified. A plane will be selected automatically"<<std::endl;
      planeautoOn=true;
    }
  

  //Step Size for arc length parametrization
  bool stepsizeOn   = false; 
  if(step_size!=1)
  {
    stepsizeOn = true;
  }

  //Standard deviation used for a kernel window
  bool bandwidthOn   = false;
  if(bandwidth!=1)
    {
      bandwidthOn = true;
    }
  //Check for step size and bandwidth > 0
  if (step_size<0.1)
    {
      cout<<"Step size is too small...making step size = 0.1\n";
      step_size=0.1;
    }
  if (bandwidth<0.1)
    {
      cout<<"Standard deviation (bandwidth) is too small...making bandwidth = 0.5\n";
      bandwidth=0.1;
    }

  //Image versus world space for the fiber bundle being considered
  bool worldspace = true;
  if(image_space == true)
    {
      worldspace = false;
    }
  //Tensor-derived scalar diffusion invariant- the diffusion parameter can be either of FA, MD, FRO, Lambda 1, Lambda 2, Lambda 3, AD, RD, GA
  //if param = 0 we want all the values
  int param = parameter_list.size();
  //std::cout<<"# of scalar parameters to be analyzed are "<<param<<std::endl;
  //Variables needed for regression
  int stat=2;		//Mean-default
  int int_noise_model=2; 	//Gaussian-default

  //Type of noise distribution assumed in a kernel window- Beta or Gaussian
  bool noisemodelOn = true;
  //if incorrect noise model in -n option, we set Gaussian mean
  if(noise_model == "beta")
    {
      int_noise_model = 1; 
    }
  else if(noise_model == "gaussian")
    {
      int_noise_model = 2; 
    }
  else
    {
      std::cout<<"noise model needs to be either gaussian or beta"<<std::endl;
      return -1;
    }

  //Type of Maximum likelihood estimate (MLE) assumed in  a kernel window- Mean, Mode or Quantiles
  bool statOn = 1;
  std::string stat_str;	
  if(stat_type == "mean")
    {
      stat = 2;
    } 
  else if(stat_type == "mode")
    {
      stat = 3; 
    }
  else if(stat_type == "quantile")
    {
      stat = 1; 
    }
  else
    {
      std::cout<<"stat type needs to be one of the following mean, mode or quantile"<<std::endl;
      return -1;
    }

  //Percentage quantile - (if Quantiles chosen as MLE)
  bool qpercOn   = true; 
  if(q_perc < 0 || q_perc>100)
    {
      std::cout<<"Quantile percentage needs to be between 0 to 100"<<std::endl;
      return -1;
    }

  //Visualization option for a kernel window's distribution histogram
  bool windowOn   = false; 
  if(window!=-1)
    {
      windowOn = true;
    }

  bool windowfileOn = true;
  if(window_file=="")
    {
      windowfileOn = false;
    }
  else
    {
      fstream filecheck;
      filecheck.open(window_file.c_str(),fstream::out);
      if (!filecheck.good() || !filecheck.is_open() || window_file.length()==1)
	{
	  cout<<"Unable to open the output visualization file provided as command line argument for writing...!\n";
	  windowfileOn = false;
	}
      filecheck.close();
    }

  if (!windowfileOn && windowOn)
    {
      int dot_loc = output_stats_file.find_last_of(".");
      window_file = output_stats_file;
      window_file.replace(dot_loc,4,"_viz.csv");
      cout<<"Creating the output visualization file in the main output file location as :\n\t"<< window_file<<endl;
    }
  


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  fiberprocessing* FP;
  FP = new fiberprocessing;
  itk::Vector<double, 3> origin;
  itk::Vector<double, 3> normal;
  
  //DEBUG
  //std::cout<<"param is "<<param<<std::endl;
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  regression* REG;
  REG = new regression;
  int all_flag = -1;
  std::cout<<"there are "<<param<<" parameters"<<std::endl;

  FP->fiberprocessing_main(input_fiber_file, planeautoOn, plane_file, worldspace, auto_plane_origin);

  if (param == 8){
    all_flag=1; 
    std::vector< std::vector<double> > all_main = FP->get_arc_length_parametrized_fiber("all");
    origin = FP->get_plane_origin();
    normal = FP->get_plane_normal();
    for (int a=0; a<=param-1; a++)	
      {
	all_flag = -1;
	std::string tmp_parameter(parameter_list[a]);
	std::cout<<"paramter list[a] is"<<tmp_parameter<<std::endl;
	std::vector< std::vector<double> > length_temp = FP->get_arc_length_parametrized_fiber(tmp_parameter);	
	REG->regression_main(output_stats_file,param,"All parameters", length_temp, origin, normal, stepsizeOn, step_size, bandwidthOn, bandwidth, statOn, 2, noisemodelOn, 2, qpercOn, q_perc, all_flag, windowOn, window, window_file, worldspace);

    }

    
    std::vector< std::vector<double> > all_results_main = REG->get_all_results();
    int reg_counter = all_results_main.size();
    std::cout<<"start to write parametrized fiber file"<<std::endl;
    //Write parametrized fiber by Yundi Shi
    FP->Write_parametrized_fiber(input_fiber_file,output_parametrized_fiber_file,step_size);
    //Writing results to the output file	
    ofstream fp_output_stats_file;
    fp_output_stats_file.open(output_stats_file.c_str(),ios::app);
    fp_output_stats_file<<"Number of samples along the bundle: "<<reg_counter<<"\n";
    fp_output_stats_file<<"Arc_Length , #_fiber_points ,";
    
    for (int a=0; a<=param-1; a++)
      {
	fp_output_stats_file<<parameter_list[a]<<" , ";
      }
    fp_output_stats_file<<"\n";
    
    //DEBUG
    //
    for (int i=0;i<reg_counter;i++)
      {
	fp_output_stats_file<<all_results_main[i][0];
	for (int j=1;j<10;j++)
	  {
	    fp_output_stats_file<<" , "<<all_results_main[i][j];
	  }
	fp_output_stats_file<<endl;
      }
    fp_output_stats_file.close();
    
  
    std::cout<<"\n***********  Finished General Evaluating of All parameters "<<std::endl;
  }
  else{

    all_flag = -1;
    for (int a=0; a<=param-1; a++){
      
      std::vector< std::vector<double> > length_main = FP->get_arc_length_parametrized_fiber(parameter_list[a]);
      if (a == 0){
	origin = FP->get_plane_origin();
	normal = FP->get_plane_normal();
	std::cout<<"origin is "<<origin<<" and normal is "<<normal<<std::endl;
      }
      
      REG->regression_main(output_stats_file,param,parameter_list[a], length_main, origin, normal, stepsizeOn, step_size, bandwidthOn, bandwidth, statOn, stat, noisemodelOn, int_noise_model, qpercOn, q_perc, -1, windowOn, window, window_file,worldspace);
      
      std::cout<<"\n***********  Finished Parameter "<<parameter_list[a]<<"  **************\n"<<std::endl;
    }
  }
  return 0;
}
 

