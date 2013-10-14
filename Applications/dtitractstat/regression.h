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
#include <algorithm>

class regression
{

public:
  regression();
  ~regression();

  //Main function
  void regression_main(std::string output_file,std::string param_str, std::vector< std::vector<double> > length, double step_size, double bandwidth, int stat, int noise_model, double q_perc, int all_flag, bool windowOn, int window, std::string output_viz_file, bool worldspace);

  //Regression functions
  void Regression_Beta_Gaussian(std::string output_file, std::vector< std::vector<double> > length, double step_size, double bandwidth, int noise_model, int stat, int all_flag, bool windowOn, int window, std::string output_viz_file);
  void Regression_Quantile(std::string output_file, std::vector< std::vector<double> > length, double step_size, double bandwidth, double q_perc, int all_flag);

  //Helper functions
  double find_min(std::vector< std::vector<double> > length, int l_counter);
  double find_max(std::vector< std::vector<double> > length, int l_counter);
  std::vector< std::vector<double> > get_all_results();
  
private:
  static bool sortFunction(std::vector< double > i , std::vector< double > j );
  std::vector< std::vector<double> > all_results;

};


#endif
