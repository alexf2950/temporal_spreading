

#include "main.h"

#include <fstream>
#include <iostream>
#include <stdlib.h> 
#include <string>


#include "graph.h"
#include "custom_io.h"





double getMean(std::vector<unsigned int>& vector){
  double mean = 0;
  for(unsigned int i = 0;i<vector.size();++i){
    mean+=vector[i];
  }
  return mean/vector.size();
}

double GetFraction(std::vector<unsigned int>& vector,bool (*condition)(unsigned int))
{
  double count = 0;
  for(unsigned int i = 0;i<vector.size();++i){
    if(condition(vector[i]))
    {
      ++count;
    }
  }
  return count/(double)vector.size();
}

Graph::GroupResult CallSimulation(Graph& graph, Parameters& p)
{
  Graph::GroupResult result;
  if (p.simulation_type == "SI")
  { 
      result = graph.SI_simulation(p.patient_zero, p.day_zero);
  } else if (p.simulation_type == "SIR")
  {
      result = graph.SIR_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period);
  } else if (p.simulation_type == "SIS")
  {
      result = graph.SIS_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period);
  } else if (p.simulation_type == "SIS_rewire")
  {
      result = graph.SIS_rewire_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
                                        p.detection_period);
  }
  
  return result;
}


std::vector<unsigned int> GetFinalInfectionSizes(Graph& graph, Parameters& p)
{
  std::vector<unsigned int> final_endemic_fraction(p.sample_size);
  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {
    p.patient_zero = n;
    auto infected_recovered_farms = CallSimulation(graph,p);
    for (auto it= infected_recovered_farms[infected_recovered_farms.size()-1].begin();
              it!=infected_recovered_farms[infected_recovered_farms.size()-1].end();++it)
    {
      final_endemic_fraction[n]+=(*it);
    }
    
  }
  
  return final_endemic_fraction;
}

double GetEndemicFraction(Graph& graph, Parameters& p)
{
  auto final_sizes = GetFinalInfectionSizes(graph, p);
  return 1-GetFraction(final_sizes,[](unsigned int size){return size==0;});
}

double GetMeanFinalInfectionSize(Graph& graph, Parameters& p)
{  
  auto final_sizes = GetFinalInfectionSizes(graph, p);
  return getMean(final_sizes);
}


void printEndemicFractions(Graph& graph, Parameters& p)
{
  
  std::ofstream file;
  file.open (p.out_file_name);

  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {
    p.patient_zero = n;
    auto infected_recovered_farms = CallSimulation(graph,p);
    for (int j = 0; j < infected_recovered_farms[0].size(); ++j)
    {
      for (int i = 1; i < infected_recovered_farms.size(); ++i)
      {
        file << infected_recovered_farms[i][j] << " ";
      }
      
    }
    file << "\n";
  }


  file.close();
}


int parseInput(Parameters& p, int argc, char** argv)
{
  if (argc<2)
  {
    std::cout << "First parameter is type = {'SI','SIR','SIS','SIS_rewire'}";
    return 1;
  }
  
  std::string name = argv[1];
  
  if (name=="SI")
  {
    if (argc!=2)
    {
      std::cout << "SI doesnt take additional parameters.";
      return 1;
    }
    p.simulation_type = "SI";
    p.infectious_period = 2;
  } else if (name=="SIR")
  {
    if (argc!=3)
    {
      std::cout << "Usage: 'SIR' infection_period";
      return 1;
    }
    p.simulation_type = "SIR";
    p.infectious_period = atoi(argv[2]);
  } else if (name=="SIS")
  {
    if (argc!=3)
    {
      std::cout << "Usage: 'SIS' infection_period";
      return 1;
    }
    p.simulation_type = "SIS";
    p.infectious_period = atoi(argv[2]);
  } else if (name=="SIS_rewire")
  {
    if (argc!=4)
    {
      std::cout << "Usage: 'SIS_rewire' infection_period detection_period";
      return 1;
    }
    p.simulation_type = "SIS_rewire";
    p.infectious_period = atoi(argv[2]);
    p.detection_period = atoi(argv[3]);
  } else
  {
    std::cout << "First parameter is type = {'SI','SIR','SIS','SIS_rewire'}";
    return 1;
  }
  
  return 0;
} 


int main(int argc, char** argv)
{
  Parameters parameters;
  
  if(parseInput(parameters, argc, argv)==1)
  {
    return 1;
  }
  
  
  
  
  
  const unsigned int NODE_NUMBER = 97980;
  
  std::string OUTPUT_FILENAME = "/home/afengler/Dokumente/traversal/SIS/"+ parameters.simulation_type +
                                  "_infection_period="+ std::to_string(parameters.infectious_period) +
                                  "_detection_period="+ std::to_string(parameters.detection_period) +".txt";
  
  
  
  parameters.day_zero = 0;
  parameters.sample_size = 100;
  parameters.out_file_name = OUTPUT_FILENAME;

  
  
  
  Graph::DayEdges edges(1461);
  
  ReadEdgesC(std::string("/home/afengler/Dokumente/traversal/edges_c.dat"), edges);
  //WriteEdgesToFile(edges,std::string("edges_c.dat"));
  
  
  Graph::NodeProperty groups;
  unsigned int group_number;
  group_number = ReadGroups(std::string("/home/afengler/Dokumente/traversal/groups.txt"),
      groups);
  
  //Graph graph = Graph(edges, NODE_NUMBER, &groups, group_number);
  Graph graph = Graph(edges, NODE_NUMBER);
  
  
  /*std::ofstream file;
  file.open (parameters.out_file_name);

  
  
  for(unsigned int i = 1;i<parameters.infectious_period && i<150;++i){
    parameters.detection_period=i;
    auto mean_final_fraction = GetEndemicFraction(graph,parameters);
    file << mean_final_fraction << " ";
  }
  
  file.close();*/
  
  printEndemicFractions(graph, parameters);
  
  return 0;
}
