
#include <algorithm>
#include <vector>
#include "property_tools.h"
 

int Recover(Graph::NodeProperty& inf,
				Graph::NodeProperty& rec,
				const std::set<int>& set, const int KEEP_RECOVERED)
{
  int counter =0;
  for (auto it = set.begin();it!=set.end();++it)
  {
    if (inf[*it]>0){
      ++counter;
      inf[*it] = 0;
      if(KEEP_RECOVERED)
        rec[*it] = 1;
      
    } 
    
  }
  return counter;
}

int Unite(Graph::NodeProperty& v, const std::set<int>& set)
{
  bool (*infectNode)(unsigned int&) = &InfectNode;
  return UpdateVector(v,set, infectNode);
}

int Detect(Graph::NodeProperty& v, const std::set<int>& set)
{
  bool (*detectNode)(unsigned int&) = &DetectNode;
  return UpdateVector(v,set, detectNode);
}

int UpdateVector(Graph::NodeProperty& v, const std::set<int>& set, bool (*changeOperation)(unsigned int&))
{
  int changeCounter =0;
  for (auto it = set.begin();it!=set.end();++it)
  {
    if(changeOperation(v[*it]))
      ++changeCounter;
    
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