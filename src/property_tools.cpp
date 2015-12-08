
#include <algorithm>
#include <vector>
#include "property_tools.h"
 

int Recover(Graph* graph,
				const Graph::NodeSet& set, const int KEEP_RECOVERED)
{
  Graph::NodeProperty& inf = graph->infectious;
  Graph::NodeProperty& rec = graph->recovered;
  
  Graph::NodeProperty& groups = graph->groups;
  
  std::vector<unsigned int>& infected_count = graph->infected_count;
  std::vector<unsigned int>& detected_count = graph->detected_count;
  std::vector<unsigned int>& recovered_count = graph->recovered_count;
  
  unsigned int counter =0;
  unsigned int group =0;
  for (auto it = set.begin();it!=set.end();++it)
  {
    if (inf[*it]>0){
      group = groups[*it];
      --infected_count[group];
      --detected_count[group];
      ++counter;
      inf[*it] = 0;
      if(KEEP_RECOVERED){
        rec[*it] = 1;
        ++recovered_count[group];
      }
        
      
    } 
    
  }
  return counter;
}

int Unite(Graph* graph, const Graph::NodeSet& set)
{
  Graph::NodeProperty& v = graph->infectious;
  Graph::NodeProperty& groups = graph->groups;
  std::vector<unsigned int>& infected_count = graph->infected_count;
  
  bool (*infectNode)(unsigned int&) = &InfectNode;
  return UpdateVector(v, groups, set, infected_count, infectNode);
}

int Detect(Graph* graph, const Graph::NodeSet& set)
{
  Graph::NodeProperty& v = graph->infectious;
  Graph::NodeProperty& groups = graph->groups;
  std::vector<unsigned int>& detected_count = graph->detected_count;
  
  bool (*detectNode)(unsigned int&) = &DetectNode;
  return UpdateVector(v, groups, set, detected_count, detectNode);
}

int UpdateVector(Graph::NodeProperty& v, const Graph::NodeProperty& groups,
					 const Graph::NodeSet& set, std::vector<unsigned int>& countVector,  bool (*changeOperation)(unsigned int&))
{
  unsigned int changeCounter=0;
  for (auto it = set.begin();it!=set.end();++it)
  {
    if(changeOperation(v[*it])){
      ++countVector[groups[*it]];
      ++changeCounter;
    }
      
    
  }
  return changeCounter;
}

bool InfectNode(unsigned int& node_status)
{
  return ChangeStatus(node_status,0,1);
}

bool DetectNode(unsigned int& node_status)
{
  return ChangeStatus(node_status,1,2);
}

bool RecoverNode(unsigned int& node_status)
{
  if(node_status>0){
    node_status=0;
    return true;
  } 
  return false;
}

bool ChangeStatus(unsigned int& node_status, const int& from, const int& to)
{
  if(node_status==from){
    node_status=to;
    return true;
  } 
  return false;
}

unsigned int countInfected(std::vector<unsigned int>& v,int n){
    return std::count (v.begin(), v.end(), n);
  }