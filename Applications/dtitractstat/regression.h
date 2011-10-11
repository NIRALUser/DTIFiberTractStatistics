#ifndef REGRESSION_H
#define REGRESSION_H

#include <string>
#include <cmath>
#include <memory>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include "dtitypes.h"

class regression
{

public:
  regression();
  ~regression();

  //Main function
  void regression_main(std::string output_file,int param,std::string param_str, std::vector< std::vector<double> > length, itk::Vector<double, 3> origin, itk::Vector<double, 3> normal, bool stepsizeOn, double step_size, bool bandwidthOn, double bandwidth, bool statOn, int stat, bool noisemodelOn, int noise_model, bool qpercOn, double q_perc, int all_flag, bool windowOn, int window, std::string output_viz_file, bool worldspace);

  //Regression functions
  void Regression_Beta_Gaussian(std::string output_file, std::vector< std::vector<double> > length, double step_size, double bandwidth, int noise_model, int stat, int all_flag, bool windowOn, int window, std::string output_viz_file);
  void Regression_Quantile(std::string output_file, std::vector< std::vector<double> > length, double step_size, double bandwidth, double q_perc, int all_flag);

  //Helper functions
  std::vector< std::vector<double> >  sort_length(std::vector< std::vector<double> >  length,  int l_counter);
  std::vector< std::vector<double> >  sort_position(std::vector< std::vector<double> >  length, std::vector< itk::Vector<double, 4> > parametrized_position, int l_counter);
  double find_min(std::vector< std::vector<double> > length, int l_counter);
  double find_max(std::vector< std::vector<double> > length, int l_counter);
  std::vector< std::vector<double> > get_all_results();
  
  //write parametrized fiber
  void Write_paramatrized_fiber(std::string output_parametrized_fiber_file,bool stepsizeOn, double step_size);
private:
  std::vector< std::vector<double> > all_results;

};


#endif
