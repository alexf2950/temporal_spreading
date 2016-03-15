

#include "main.h"

#include <fstream>
#include <iostream>
#include <stdlib.h> 
#include <string>
#include <cstring>
#include <json.hpp>

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
                                        p.detection_period);
   } else if (p.simulation_type == "SIR_rewire")
   {
      result = graph.SIR_rewire_simulation(p.patient_zero,
                                        p.day_zero,
                                        p.infectious_period,
                                        p.detection_period);
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

std::vector<unsigned int> GetFinalInfectionSizes(Graph& graph, Parameters& p)
{
  const unsigned int SAMPLES = 5;
  std::vector<unsigned int> final_endemic_fraction(p.sample_size);
  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {
    p.patient_zero = n;
    auto infected_recovered_farms = CallSimulation(graph,p);
    for(unsigned int k=0;k<SAMPLES;++k)
    {
       for (auto it= infected_recovered_farms[infected_recovered_farms.size()-1-k].begin();
                  it!=infected_recovered_farms[infected_recovered_farms.size()-1-k].end();++it)
        {
          final_endemic_fraction[n]+=(*it);
        }
    }
    final_endemic_fraction[n]/=SAMPLES;
       
    
  }
  
  return final_endemic_fraction;
}


std::array<double,2> GetMeanPeaks(Graph& graph, Parameters& p)
{
  std::vector<unsigned int> peaks(p.sample_size);
  std::vector<unsigned int> peak_times(p.sample_size);
  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {
    p.patient_zero = n;
    auto infected_recovered_farms = CallSimulation(graph,p);
    std::vector<double> summed(infected_recovered_farms.size());
    summed.assign(infected_recovered_farms.size(),0);
    
    for(unsigned int k=0;k<infected_recovered_farms.size();++k)
    {
       for (auto it= infected_recovered_farms[k].begin();
                  it!=infected_recovered_farms[k].end();++it)
        {
          summed[k]+=(*it);
        }  
    }
    
    unsigned int max = 0;
    unsigned int argmax =0;
    
    for(unsigned int k=0;k<summed.size();++k)
    {
      if(summed[k]>max)
      {
        max = summed[k];
        argmax = k;
      }
    }
    peak_times[n] = argmax;
    peaks[n] = max;
              
  }
  
  std::array<double,2> peak_values;
  peak_values[0] = getMean(peak_times);
  peak_values[1] = getMean(peaks);
  
  return peak_values;
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

void printMeanFinalSizes(Graph& graph, Parameters& p)
{
  p.out_file_name = p.output_path + p.simulation_type +
                                  "_infection_period="+ std::to_string(p.infectious_period) +
                                  "_detection_period="+ std::to_string(p.detection_period) +".txt";
  if(p.use_groups ==1)
  {
    p.out_file_name.replace(p.out_file_name.length()-4,11,"_groups.txt");
  }
  std::ofstream file;
  file.open (p.out_file_name);
  
  if(p.simulation_type == "SIS")
  {
        const unsigned int MAX = p.infectious_period;
        for(unsigned int i = 1;i<MAX+1;++i){
          p.infectious_period =i;
          double finalSize = GetMeanFinalInfectionSize(graph,p);
          file << finalSize << " ";
        }
  } else if(p.simulation_type == "SIS_rewire")
  {
        const unsigned int MAX = p.infectious_period;
        for(unsigned int i = 1;i<MAX;++i){
          p.detection_period =i;
          double finalSize = GetMeanFinalInfectionSize(graph,p);
          file << finalSize << " ";
        }
  } else if(p.simulation_type == "SIR")
  {
        const unsigned int MAX = p.infectious_period;
        for(unsigned int i = 1;i<MAX;++i){
          p.infectious_period =i;
          std::array<double,2> peak = GetMeanPeaks(graph,p);
          file << peak[0] << " " << peak[1] << "\n";
        }
  } else if(p.simulation_type == "SIR_rewire")
  {
        const unsigned int MAX = p.infectious_period;
        for(unsigned int i = 1;i<MAX;++i){
          p.detection_period =i;
          std::array<double,2> peak = GetMeanPeaks(graph,p);
          file << peak[0] << " " << peak[1] << "\n";
        }
  }
  
  
  file.close();
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

void printShortestPaths(Graph& graph, Parameters& p)
{
  p.out_file_name.replace(p.out_file_name.length()-4,9,"_days.txt");
  
  std::ofstream file;
  file.open (p.out_file_name);

  graph.DAYS = 1460;
  
  for (unsigned int n=0;n<p.sample_size; ++n)
  {
    p.patient_zero = n;
    CallSimulation(graph,p);
    auto infection_day = graph.infection_day;
    auto infected_by = graph.infected_by;
    for (int d = 0; d < infection_day.size(); ++d)
    {
      /*if (infection_day[d]>170)
      {
        unsigned int target_node = d;
        unsigned int source_node = infected_by[d];
        while(target_node!=source_node)
        {
          file << target_node << " ";
          target_node = source_node;
          source_node = infected_by[source_node];
        }
        file << target_node << "\n";
        
        
      }*/
      file << infection_day[d] << " ";     
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
  
  std::string name(argv[1]);
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
    p.infectious_period  = atoi(argv[BASE_PARAMETERS]);
    p.detection_period = atoi(argv[BASE_PARAMETERS+1]);
  } else
  {
    std::cout << "First parameter is of type = {'SI','SIR','SIS','SIS_rewire'}";
    return 1;
  }
  
  p.use_groups = atoi(argv[2]);
  p.transmission_probability = atof(argv[3]);
  p.RANDOM_FLAG = atoi(argv[4]);
  p.use_static_network = atoi(argv[5]);
  
  p.day_zero=0;
  
  return 0;
} 

int parseSettings(nlohmann::json& settings, Parameters& p){
  
  std::vector<std::string> params = {"simulation_type",
                                      "use_groups",
                                      "transmission_probability",
                                      "RANDOM_FLAG",
                                      "use_static_network",
                                      "infectious_period",
                                      "detection_period",
                                      "input_file",
                                      "output_path"
                                      };
                             
  for(auto it=params.begin();it!=params.end();++it)
  {
    if(settings[*it].is_null()){
     std::cout << "Warning: " << *it << " missing in settings file";
    }
  }


  p.simulation_type = settings["simulation_type"];
  p.use_groups = settings["use_groups"];
  p.transmission_probability = settings["transmission_probability"];
  p.RANDOM_FLAG = settings["RANDOM_FLAG"];
  p.use_static_network = settings["use_static_network"];
  p.infectious_period = settings["infectious_period"];
  p.detection_period = settings["detection_period"];
  p.output_path = settings["output_path"];
  p.input_file = settings["input_file"];
  

  return 1;
}

nlohmann::json readSettings(){
  std::ifstream json_file("settings.json");
  nlohmann::json settings(json_file);
  settings << json_file;
 
  
  json_file.close();
  
  return settings;
}

bool binaryExists(const std::string& name){
  std::string binary_path = name;
  binary_path.replace(binary_path.length()-4,9,"_c.dat");
  
  std::ifstream f(binary_path.c_str());
  if (f.good()) {
      f.close();
      return true;
  } else {
      f.close();
      return false;
  } 
    
}


int main(int argc, char** argv)
{
  Parameters parameters;
  parameters.detection_period = 0;
  
  if(parseInput(parameters, argc, argv)==1)
  {
    return 1;
  }
  
  
  nlohmann::json settings = readSettings();
 
  // Try settings file, if empty fall back to command line input
  if (settings.size()>0) parseSettings(settings,parameters);
    else {
      if(parseInput(parameters, argc, argv)==1)
      {
        return 1;
      }
    }
 
  
  
  
  const unsigned int NODE_NUMBER = 97980;
  const unsigned int EDGE_NUMBER = 6359697;
  //const unsigned int EDGE_NUMBER = 747746;
  
  std::string OUTPUT_FILENAME = parameters.output_path + parameters.simulation_type +
                                  "_infection_period="+ std::to_string(parameters.infectious_period) +
                                  "_detection_period="+ std::to_string(parameters.detection_period) +".txt";
  
  const unsigned int DAY_NUMBER = 1460;
  
  parameters.day_zero = 0;
  parameters.sample_size = 100;
  parameters.out_file_name = OUTPUT_FILENAME;

  
  
  
  Graph::DayEdges edges(DAY_NUMBER);
  
  unsigned int RANDOM_FLAG = parameters.RANDOM_FLAG; // 0 means original network, 1 temporal randomized
  
  if(!binaryExists(parameters.input_file)){
    ReadEdgesTxt(parameters.input_file, edges);
    WriteEdgesToFile(edges,std::string("edges_c.dat"));
  } else {
    const unsigned int REVERSE=0;
    std::string binary_path = parameters.input_file;
    binary_path.replace(binary_path.length()-4,9,"_c.dat");
    ReadEdgesC(binary_path, edges, EDGE_NUMBER, RANDOM_FLAG, REVERSE);
  }
  
  
  
  Graph::NodeProperty groups;
  unsigned int group_number;

  

  
  if (parameters.use_groups)
  {
      group_number = ReadGroups(std::string("/home/afengler/Dokumente/traversal/groups_old.txt"),
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
  
  //printMeanFinalSizes(graph,parameters);
  printEndemicFractions(graph,parameters);
  //printInfectedAndDetectedCounts(graph, parameters);
  //printShortestPaths(graph,parameters);
  
  return 0;
}
