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
  unsigned int use_groups;
  double transmission_probability;
  unsigned int RANDOM_FLAG; // 0 means original network, 1 temporal randomized
  unsigned int use_static_network;
  
  std::string out_file_name;
  std::string simulation_type;
};

int 						  parseInput(Parameters& p, int argc, char** argv);
double 						getMean(std::vector<unsigned int>& vector);
double            GetFraction(std::vector<unsigned int>& vector,bool (*condition)(unsigned int));

Graph::GroupResult 	  CallSimulation(Graph& graph, Parameters& p);
std::vector<unsigned int> 	  GetFinalInfectionSizes(Graph& graph, Parameters& p);
double 						    GetMeanFinalInfectionSize(Graph& graph, Parameters& p);
double                GetEndemicFraction(Graph& graph, Parameters& p);
void 						      printEndemicFractions(Graph& graph, Parameters& p);


// Do p.sample_size runs with fixed initial node p.patient_zero and return the average number of infections via orignal
// and recovered edges. Make sure p.simulation_type is "SIS_rewire" and to set patien_zero to work properly
std::array<double,2> GetOriginalAndRewiredInfectionCounts(Graph& graph, Parameters& p);

#endif