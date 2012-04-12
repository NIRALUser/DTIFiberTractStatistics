#ifndef FIBERPROCESSING_H
#define FIBERPROCESSING_H

#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#include <vtkCellArray.h>
#include <vtkPolyLine.h>

#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

#include "dtitypes.h"

class fiberprocessing{

 public:
  fiberprocessing();
  ~fiberprocessing();

  //Main functions
  void fiberprocessing_main(std::string& input_file,std::string& output_file,bool planeautoOn, std::string plane_file, bool worldspace, std::string auto_plane_origin, bool useNonCrossingFibers);
  void arc_length_parametrization(GroupType::Pointer group, bool worldspace, itk::Vector<double,3> spacing, itk::Vector<double,3> offset);

  //IO functions
  bool read_plane_details(std::string plane_str, itk::Vector<double,3> spacing, itk::Vector<double,3> offset, bool worldspace);
  void writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup);
  GroupType::Pointer readFiberFile(std::string filename);
  void Write_parametrized_fiber(std::string input_file, std::string output_parametrized_fiber_file,double step_size,bool worldspace);

  //Helper functions
  void find_distance_from_plane(itk::Point<double, 3> pos, int index);
  void find_plane(GroupType::Pointer group, std::string auto_plane_origin);
  bool Siequals(std::string a, std::string b);
  double find_min_dist();
  double find_max_dist();
  void sort_parameter(GroupType::Pointer fibergroup);
  itk::Vector<double, 3> get_plane_origin();
  itk::Vector<double, 3> get_plane_normal();
  std::vector< std::vector<double> > get_arc_length_parametrized_fiber(std::string param_name);
  vtkSmartPointer<vtkPolyData> RemoveNonCrossingFibers(std::string Filename);
  std::string takeoffExtension(std::string filename);
  std::string takeoffPath(std::string filename);
  std::string ExtensionofFile(std::string filename);
  
 private:
  double SQ2(double x) {return x*x;};

  //Variables
  itk::Vector<double, 3> plane_origin, plane_normal;
  itk::Point<double, 3> closest_point;
  //all --> contain all the dti info in the order of FA, MD FRO, l2, l3, AD, RD, GA
  std::vector< std::vector<double> >  all;
  std::vector< std::vector<double> > parametrized_position_dist;
  std::vector< std::vector<double> > parametrized_position_x;
  std::vector< std::vector<double> > parametrized_position_y;
  std::vector< std::vector<double> > parametrized_position_z;
  double closest_d;
  int l_counter;
};


#endif
