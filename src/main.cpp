

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
      result = graph.SI_simulation(p.patient_zero, p.day_zero, p.use_static_network);
  } else if (p.simulation_type == "SIR")
  {
      result = graph.SIR_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
                                        p.use_static_network);
  } else if (p.simulation_type == "SIS")
  {
      result = graph.SIS_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
                                        p.use_static_network);
  } else if (p.simulation_type == "SIS_rewire")
  {
      result = graph.SIS_rewire_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
<<<<<<< HEAD
                                        p.detection_period,
                                        p.OUTPUT_FLAG);
=======
                                        p.detection_period);
  } else if (p.simulation_type == "SIR_rewire")
  {
      result = graph.SIR_rewire_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
                                        p.detection_period);
>>>>>>> c745e2e3c1297858514531d8f4fdb12d20f5db6d
  }
  
  return result;
}

std::array<double,2> GetOriginalAndRewiredInfectionCounts(Graph& graph, Parameters& p)
{
  std::array<double,2> counts;
  unsigned int original_count = 0;
  unsigned int rewired_count = 0;
  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {

    CallSimulation(graph,p);
    for (unsigned int i=0; i < graph.GROUP_NUMBER;++i)
    {
      original_count += graph.original_infections[i];
      rewired_count += graph.rewired_infections[i];
    }
    
  }
  counts[0]= original_count/(double)p.sample_size;
  counts[1]= rewired_count/(double)p.sample_size;
 
  return counts;
}

void printInfectedAndDetectedCounts(Graph& graph, Parameters& p)
{
  p.OUTPUT_FLAG = 1;
  printEndemicFractions(graph, p);
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
    std::cout << "First parameter is type = {'SI','SIR','SIS','SIS_rewire', 'SIR_rewire'}";
    return 1;
  }
  
  std::string name = argv[1];
  const unsigned int BASE_PARAMETERS = 6;
  
  if (name=="SI")
  {
    if (argc!=BASE_PARAMETERS)
    {
      std::cout << "Usage: ./run 'SI' use_groups transmission_probability RANDOM_FLAG";
      return 1;
    }
    p.simulation_type = "SI";
    p.infectious_period = 2;
    
  } else if (name=="SIR")
  {
    if (argc!=BASE_PARAMETERS+1)
    {
      std::cout << "Usage: 'SIR' use_groups transmission_probability RANDOM_FLAG STATIC infection_period";
      return 1;
    }
    p.simulation_type = "SIR";
    p.infectious_period = atoi(argv[BASE_PARAMETERS]);
  } else if (name=="SIS")
  {
    if (argc!=BASE_PARAMETERS+1)
    {
      std::cout << "Usage: 'SIS' use_groups transmission_probability RANDOM_FLAG STATIC infection_period";
      return 1;
    }
    p.simulation_type = "SIS";
    p.infectious_period = atoi(argv[BASE_PARAMETERS]);
  } else if (name=="SIS_rewire")
  {
    if (argc!=BASE_PARAMETERS+2)
    {
      std::cout << "Usage: 'SIS_rewire' use_groups transmission_probability RANDOM_FLAG STATIC infection_period detection_period";
      return 1;
    }
    
    p.simulation_type = "SIS_rewire";
    p.infectious_period = atoi(argv[BASE_PARAMETERS]);
    p.detection_period = atoi(argv[BASE_PARAMETERS+1]);
  } else if (name=="SIR_rewire")
  {
    if (argc!=BASE_PARAMETERS+2)
    {
      std::cout << "Usage: 'SIS_rewire' use_groups transmission_probability RANDOM_FLAG STATIC infection_period detection_period";
      return 1;
    }
    
    p.simulation_type = "SIR_rewire";
    p.infectious_period = atoi(argv[BASE_PARAMETERS]);
    p.detection_period = atoi(argv[BASE_PARAMETERS+1]);
  } else
  {
    std::cout << "First parameter is type = {'SI','SIR','SIS','SIS_rewire'}";
    return 1;
  }
  
  p.use_groups = atoi(argv[2]);
  p.transmission_probability = atof(argv[3]);
  p.RANDOM_FLAG = atoi(argv[4]);
<<<<<<< HEAD
  p.OUTPUT_FLAG = 0;
=======
  p.use_static_network = atoi(argv[5]);
>>>>>>> c745e2e3c1297858514531d8f4fdb12d20f5db6d
  
  return 0;
} 


int main(int argc, char** argv)
{
  Parameters parameters;
  
  parameters.detection_period = 0;
  
  if(parseInput(parameters, argc, argv)==1)
  {
    return 1;
  }
  
  
  
  
  
  const unsigned int NODE_NUMBER = 97980;
  const unsigned int EDGE_NUMBER = 6359697;
  //const unsigned int EDGE_NUMBER = 747746;
  
  std::string OUTPUT_FILENAME = "output/SIS/"+ parameters.simulation_type +
                                  "_infection_period="+ std::to_string(parameters.infectious_period) +
                                  "_detection_period="+ std::to_string(parameters.detection_period) +".txt";
  
  const unsigned int DAY_NUMBER = 1460;
  
  parameters.day_zero = 0;
  parameters.sample_size = 100;
  parameters.out_file_name = OUTPUT_FILENAME;

  
  
  
  Graph::DayEdges edges(DAY_NUMBER);
  
  unsigned int RANDOM_FLAG = parameters.RANDOM_FLAG; // 0 means original network, 1 temporal randomized
  
<<<<<<< HEAD
  ReadEdgesC(std::string("/home/afengler/Dokumente/traversal/edges_c.dat"), edges, EDGE_NUMBER, RANDOM_FLAG);
  //ReadEdgesTxt(std::string("/home/afengler/Dokumente/data/LuisRocha_sexnet_pnas.txt"), edges);
=======

  //ReadEdgesTxt(std::string("/home/afengler/Dokumente/traversal/edges_subset.txt"), edges);
>>>>>>> c745e2e3c1297858514531d8f4fdb12d20f5db6d
  //WriteEdgesToFile(edges,std::string("edges_c.dat"));

  
  Graph::NodeProperty groups;
  unsigned int group_number;

  
  if (parameters.use_groups)
  {
<<<<<<< HEAD
      group_number = ReadGroups(std::string("/home/afengler/Dokumente/traversal/community_groups_0.txt"),
=======
      group_number = ReadGroups(//std::string("/home/afengler/Dokumente/traversal/groups.txt"),
                                std::string("/home/alex/Documents/data/groups.txt"),
>>>>>>> c745e2e3c1297858514531d8f4fdb12d20f5db6d
                                groups);
  }
  
    
  if (parameters.use_static_network==1)
  {
    //ReadEdgesC(std::string("data/edges_c.dat"), edges, EDGE_NUMBER, RANDOM_FLAG);
    //std::string static_network_file = "data/edges_static.dat";
    //aggregateNetwork(edges,groups,group_number,static_network_file);
    edges.resize(97980);
    ReadEdgesC(std::string("data/edges_static.dat"), edges, 315267, RANDOM_FLAG);
    //return 1;
  } else
  {
    ReadEdgesC(std::string("data/edges_c.dat"), edges, EDGE_NUMBER, RANDOM_FLAG);
  }
  
  Graph graph = Graph(edges, NODE_NUMBER, groups, group_number);
  graph.transmission_probability = parameters.transmission_probability;
  
  
  /*std::ofstream file;
  file.open (parameters.out_file_name);

  parameters.patient_zero = 3;
  
  for(unsigned int i = 1;i<16;++i){
    parameters.detection_period+=1;
    auto orig_and_rewired_counts = GetOriginalAndRewiredInfectionCounts(graph,parameters);
    file << orig_and_rewired_counts[0] << " " << orig_and_rewired_counts[1] << "\n";
  }
  
  file.close();*/
  
  printEndemicFractions(graph,parameters);
  //printInfectedAndDetectedCounts(graph, parameters);
  
  return 0;
}
