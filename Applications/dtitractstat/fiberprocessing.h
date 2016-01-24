#ifndef FIBERPROCESSING_H
#define FIBERPROCESSING_H

#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <numeric>
#include <algorithm>

#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkVersion.h>
#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

#include "dtitypes.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType double
#endif

struct parametrized_distance_struct
{
  double dist ;
  double x ;
  double y ;
  double z ;
};

class fiberprocessing{

 public:
  fiberprocessing();
  ~fiberprocessing();
  std::string takeoffExtension(std::string filename);
  std::string takeoffPath(std::string filename);
  std::string ExtensionofFile(std::string filename);
  //Main functions
  void fiberprocessing_main(std::string& input_file,std::string& output_file,bool planeautoOn, std::string plane_file, bool worldspace, std::string auto_plane_origin, bool useNonCrossingFibers , double bandwidth , bool removeCleanFibers, bool removeNanFibers, const char* TargetScalarName="") ;
  std::vector< std::vector<double> > get_arc_length_parametrized_fiber(std::string param_name);
  void Write_parametrized_fiber(std::string output_parametrized_fiber_file);//Writes output parametrized fiber file without resampling the fibers
  void Write_parametrized_fiber_avg_position_and_arclength(std::string input_file, std::string output_parametrized_fiber_file, double step_size) ;

 private:
  void arc_length_parametrization(GroupType::Pointer group);
  //IO functions
  bool read_plane_details(std::string plane_str);
  void writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup);
  GroupType::Pointer readFiberFile(std::string filename);
  //Helper functions
  void find_plane(GroupType::Pointer group, std::string auto_plane_origin);
  bool Siequals(std::string a, std::string b);
  double find_min_dist();
  double find_max_dist();
  void sort_parameter();
  itk::Vector<double, 3> get_plane_origin();
  itk::Vector<double, 3> get_plane_normal();
  vtkSmartPointer<vtkPolyData> RemoveNonCrossingFibers(std::string Filename);
  vtkSmartPointer<vtkPolyData> RemoveNanFibers(std::string Filename);
  double SQ2(double x) {return x*x;};
  static bool sortFunction(parametrized_distance_struct i , parametrized_distance_struct j );
  double DistanceToPlane(itk::Point<double , 3> point) ;
  int CheckFiberOrientation( DTIPointListType &pointlist , int &count_opposite ) ;
  void ParametrizesEntireFiber(DTIPointListType &pointlist , int flag_orientation ) ;
  int ParametrizesHalfFiber(DTIPointListType &pointlist , DTIPointListType::iterator &endit, int increment, int displacement ) ;
  void ComputeArcLength(DTIPointListType::iterator &beginit , DTIPointListType::iterator &endit , itk::Point<double,3> p2 , int increment , int displacement , double min_distance ) ;
  void AddValueParametrization( DTIPointListType::iterator &pit , itk::Point<double,3> p1 , double distance ) ;
  double Find_First_Point(DTIPointListType &pointlist , int displacement , DTIPointListType::iterator &pit_first ) ;
  itk::Point< double , 3> SpatialPosition(itk::Point<double, 3> position ) ;

  //Variables
  itk::Vector<double, 3> plane_origin, plane_normal;
  itk::Point<double, 3> closest_point;
  //all --> contain all the dti info in the order of FA, MD FRO, l2, l3, AD(l1), RD, GA
  std::vector< std::vector<double> >  all;
  std::vector< std::vector<parametrized_distance_struct> > parametrized_position;
  double closest_d;
  itk::Vector<double,3> m_Spacing ;
  itk::Vector<double,3> m_Offset ;
  double m_Bandwidth ;
  bool m_WorldSpace ;
  const char* scalarName;
};


#endif
