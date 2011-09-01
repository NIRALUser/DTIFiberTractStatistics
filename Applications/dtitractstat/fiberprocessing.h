#ifndef FIBERPROCESSING_H
#define FIBERPROCESSING_H

#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#include <itkSpatialObjectReader.h>
#include <itkSpatialObjectWriter.h>

#include "dtitypes.h"

class fiberprocessing{

 public:
  fiberprocessing();
  ~fiberprocessing();

  //Main functions
  void fiberprocessing_main(std::string input_file,bool planeautoOn, std::string plane_file, bool worldspace);
  void arc_length_parametrization(GroupType::Pointer group, bool worldspace, itk::Vector<double,3> spacing, itk::Vector<double,3> offset);

  //IO functions
  bool read_plane_details(std::string plane_str, itk::Vector<double,3> spacing, itk::Vector<double,3> offset, bool worldspace);
  void writeFiberFile(const std::string & filename, GroupType::Pointer fibergroup);
  GroupType::Pointer readFiberFile(std::string filename);

  //Helper functions
  void find_distance_from_plane(itk::Point<double, 3> pos, int index);
  void find_plane(GroupType::Pointer group);
  bool Siequals(std::string a, std::string b);
  itk::Vector<double, 3> get_plane_origin();
  itk::Vector<double, 3> get_plane_normal();
  std::vector< std::vector<double> > get_arc_length_parametrized_fiber(std::string param_name);

 private:
  double SQ2(double x) {return x*x;};

  //Variables
  itk::Vector<double, 3> plane_origin, plane_normal;
  itk::Point<double, 3> closest_point;
  //all --> contain all the dti info in the order of FA, MD FRO, l2, l3, AD, RD, GA
  std::vector< std::vector<double> >  all;
  std::vector< itk::Vector<double, 3> > parametrized_position;
  double closest_d;
  int l_counter;

};


#endif
