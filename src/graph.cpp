
#include "graph.h"
#include "property_tools.h"
#include "random_tools.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <array>


Graph::Graph(DayEdges& _edges, unsigned int _NODE_NUMBER, NodeProperty& _groups,
                  unsigned int _group_number):
	edges(_edges),
  NODE_NUMBER(_NODE_NUMBER)
{
	DAYS = 1460;
  // initialize with zeros
  if(_group_number<=1){
    GROUP_NUMBER = 1;
    groups.fill(0);
    countGroups();
  } else
  {
    groups = _groups;
    GROUP_NUMBER = _group_number;
    countGroups();
  } 
  
  //infectious = NodeProperty(NODE_NUMBER,0);
  //recovered = NodeProperty(NODE_NUMBER,0);
}

void Graph::initializeInfection(int inode,int iday, unsigned int infection_period)
{
    infectious.fill(0);
    infectious.at(inode) = 1;
    
    //infected_by.fill(0);
    //infected_by.at(inode) = inode;
    
    //infection_day.fill(0);
    
    summed_infected_count = 1;
    infected_count.assign(GROUP_NUMBER,0);
    infected_count[groups[inode]]=1;
    
    recovered.fill(0);
    recovered_count.assign(GROUP_NUMBER,0);

    
    recovery.clear();
    recovery[iday +infection_period].push_back(inode);
    
    original_infections.assign(GROUP_NUMBER,0);
    
    return;
}

void Graph::initializeInfection(int inode,int iday, unsigned int infection_period, unsigned int detection_period)
{
    initializeInfection(inode,iday,infection_period);
    
    detection.clear();
    detection[iday + detection_period].push_back(inode);
    
    detected_count.assign(GROUP_NUMBER,0);
    
    rewired_infections.assign(GROUP_NUMBER,0);
    
    
    
    return;
}

void Graph::countGroups()
{
  group_counts.resize(GROUP_NUMBER);
  for(unsigned int i = 0;i<groups.size();++i)
  {
    ++group_counts[groups[i]];
  }
  
  return;
}

void Graph::infectionSweep(unsigned int day, unsigned int infection_period,
                           unsigned int detection_period, 
                           unsigned int STATIC,
                           bool (Graph::*rewire)(int,int,unsigned int))
{
	// infection
	if (summed_infected_count>0)
	{
		NodeSet new_infectious;
    if(STATIC)
    {
      new_infectious= staticInfect();
    } else
    {
      new_infectious= infect(day,rewire);
    }

		summed_infected_count +=Unite(this, new_infectious);
    
    // Node gets marked for recovery after infection period
    if(infection_period<DAYS){
      recovery[day+infection_period].insert(recovery[day+infection_period].end(),new_infectious.begin(), new_infectious.end());
      if(detection_period<infection_period){
         detection[day+detection_period].insert( detection[day+detection_period].end(),new_infectious.begin(), new_infectious.end());
      }
     
    }
    
	}
}
	
void Graph::recoverySweep(unsigned int day,const int KEEP_RECOVERED)
{
    // recovery
    removalPrototype(recovery, day,KEEP_RECOVERED);
}

void Graph::detectionSweep(unsigned int day)
{
    // detection
    //removalPrototype(recovery, openly_infectious, day,0);
    
    RdayIter it = detection.find(day);
    if (it != detection.end())
    {
      Detect(this, (*it).second);
      
    }
}    

void Graph::removalPrototype(RdayMap& map,unsigned int day,const int KEEP_RECOVERED)
{
  
    RdayIter it = map.find(day);
    if (it != map.end())
    {
      int recovers = Recover(this, (*it).second,KEEP_RECOVERED);
      summed_infected_count -=recovers;

    }
}

Graph::NodeSet Graph::staticInfect()
{
  NodeSet new_infectious;
  
  DayEdges out_list = edges;
  if (infectious.size()>0)
  {
    for (unsigned int s = 0; s != infectious.size(); ++s)
    {
      if(infectious[s] && out_list[s].size()>0)
      {
        int u = s; //source
        for(auto t = out_list[s].begin();t!=out_list[s].end();++t)
        {
          int v = (*t)[1]; //target
          // infections happening here
          // add target to infectious if source is infected and target is neither infected and nor recovered
          if (!infectious.at(v))       // This is decides whether or not reinfection is possible
          {
            if (!recovered.at(v)){
              
                //std::cout << transmission_probability << "\n";  
                if (transmission_probability==1 || SampleProbability(transmission_probability)==1)
                {   
                    
                  new_infectious.push_back(v);
                  
                }
              
            }
              
          }
        }
      }
        
        
      
      
    }
  }
  return new_infectious;
}
  
//return newly infected nodes for 'day'
Graph::NodeSet Graph::infect(unsigned int day,bool (Graph::*rewire)(int,int,unsigned int))
{
  NodeSet new_infectious;
  
  EdgeList day_list = edges[day];
  if (day_list.size()>0)
  {
    for (unsigned int i = 0;i<day_list.size();++i)
    {
      EDGE& b = day_list[i];
      int u = (b)[0]; //source
      int v = (b)[1]; //target
      
      // infections happening here
      // add target to infectious if source is infected and target is neither infected and nor recovered
      if (infectious.at(u) && !infectious.at(v))       // This is decides whether or not reinfection is possible
      {
        if (!recovered.at(v)){
          bool rewired_to_infected = ((this)->*rewire)(u,v,day);
          if(rewired_to_infected)
          {
            //std::cout << transmission_probability << "\n";  
            if (transmission_probability==1 || SampleProbability(transmission_probability)==1)
            {   
                
              new_infectious.push_back(v);
              //infected_by.at(v) = u;
              //infection_day.at(v) = day;
            }
          }
        }
          
      }
    }
  }
  return new_infectious;
}

/********************************************************************
*  SI Simulation
*   Cuts of the recovered part of SIR output
********************************************************************/
Graph::GroupResult Graph::SI_simulation(int inode, int iday, unsigned int STATIC)
{
  auto infected_recovered_farms = SIR_simulation(inode,iday,DAYS, STATIC);
  auto infectedFarms = Graph::GroupResult (infected_recovered_farms.begin(),infected_recovered_farms.begin()+DAYS);
  
  return infectedFarms;
}

/********************************************************************
*  SIS Simulation
*  Cuts of the recovered part of SIX output
********************************************************************/
Graph::GroupResult Graph::SIS_simulation(int inode, int iday,
 unsigned int infection_period, unsigned int STATIC)
{
  auto infected_recovered_farms = SIX_simulation(inode,iday,infection_period,0, STATIC);
 
  // discard the recovered part
  auto infectedFarms = Graph::GroupResult(infected_recovered_farms.begin(),infected_recovered_farms.begin()+DAYS);
  
  return infectedFarms;
}



/********************************************************************
*  SIR Simulation
*
********************************************************************/
Graph::GroupResult Graph::SIR_simulation(int inode, int iday,
 unsigned int infection_period, unsigned int STATIC)
{
  return SIX_simulation(inode,iday,infection_period,1, STATIC);
}


/********************************************************************
*  SIX Simulation
*  Template for SIR and SIS simulations   
*
********************************************************************/
Graph::GroupResult Graph::SIX_simulation(int inode, int iday, unsigned int infection_period,
  const int KEEP_RECOVERED,unsigned int STATIC)
{
  auto infected_recovered_farms = Graph::GroupResult(DAYS);
  
  InitializeResultVector(GROUP_NUMBER, infected_recovered_farms);
  
  
  initializeInfection(inode,iday,infection_period,0);

  for (unsigned int day = iday; day < DAYS; day++)
  {     
    infectionSweep(day, infection_period,1460,STATIC);
    //std::cout << day << "\n";
    
    if(infection_period<DAYS){
      recoverySweep(day,KEEP_RECOVERED);
    }
    
    for(unsigned int i=0;i<GROUP_NUMBER;++i){
      infected_recovered_farms[day-iday][i] = infected_count[i];
      //infected_recovered_farms[DAYS+(day-iday)][i] = recovered_count[i];
    }
    
  }

  return infected_recovered_farms;
}



Graph::GroupResult Graph::SIS_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period
													 )
{ 
  return SIX_rewire_simulation(inode, iday, infection_period, detection_period,0);
  
}

Graph::GroupResult Graph::SIR_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period
													 )
{ 
  return SIX_rewire_simulation(inode, iday, infection_period, detection_period,1);
  
}
Graph::GroupResult Graph::SIX_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period,
                            const unsigned int KEEP_RECOVERED
													 )
{ 
  bool (Graph::*rewire)(int,int,unsigned int) = &Graph::_rewire;
 
  auto infected_farms = Graph::GroupResult(DAYS);
  InitializeResultVector(GROUP_NUMBER, infected_farms);
  
  
  initializeInfection(inode,iday,infection_period,detection_period);
  
  for (unsigned int day = iday; day < DAYS; day++)
  {     
    
    infectionSweep(day, infection_period,detection_period,0, rewire);
    detectionSweep(day);
    recoverySweep(day,KEEP_RECOVERED);
    
    for(unsigned int i=0;i<GROUP_NUMBER;++i){
      infected_farms[day-iday][i] = infected_count[i];
    }
  }
  
  return infected_farms;
  
}

// source is infectious
bool Graph::_rewire(int source, int target, unsigned int day)
{
  int infection_status = infectious.at(source);
  
  if(infection_status==2){
    
    unsigned int latent_infected = infected_count[groups[source]]-detected_count[groups[source]];
    unsigned int seemingly_uninfected = group_counts[groups[source]]-detected_count[groups[source]];
    
    double infection_probability = latent_infected/(double)seemingly_uninfected;    
    //double infection_probability = infected_count[groups[source]]/(double)seemingly_uninfected;    
    
    if(SampleProbability(infection_probability)==0){
      return false;
      
    }
    
    rewired_infections[groups[target]]+=1;
  } else
  {
    original_infections[groups[target]]+=1;
  }
  
  return true;
}

void Graph::InitializeResultVector(unsigned int group_number, Graph::GroupResult& result)
{
  
  for(unsigned int i=0;i<result.size();++i)
  {
      result[i].resize(group_number);
  }
}