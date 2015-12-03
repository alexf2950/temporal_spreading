#ifndef GUARD_main_h
#define GUARD_main_h

#include <string>
#include <vector>
#include "graph.h"

struct Parameters{
  unsigned int patient_zero;
  unsigned int day_zero;
  unsigned int sample_size;
  unsigned int infectious_period;
  unsigned int detection_period;
  
  std::string out_file_name;
  std::string simulation_type;
};

int 						parseInput(Parameters& p, int argc, char** argv);
double 						getMean(std::vector<unsigned int>& vector);

std::vector<unsigned int> 	CallSimulation(Graph& graph, Parameters& p);
std::vector<unsigned int> 	GetFinalInfectionSizes(Graph& graph, Parameters& p);
double 						GetMeanFinalInfectionSize(Graph& graph, Parameters& p);
void 						printEndemicFractions(Graph& graph, Parameters& p);

#endif