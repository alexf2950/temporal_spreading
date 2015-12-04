
#include "graph.h"
#include "property_tools.h"
#include "random_tools.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <array>


Graph::Graph(DayEdges& _edges, unsigned int _NODE_NUMBER):
	edges(_edges),
  NODE_NUMBER(_NODE_NUMBER)
{
	DAYS = edges.size();
  // initialize with zeros
  //infectious = NodeProperty(NODE_NUMBER,0);
  //recovered = NodeProperty(NODE_NUMBER,0);
}

void Graph::initializeInfection(int inode,int iday, unsigned int infection_period)
{
    infectious.fill(0);
    infectious.at(inode) = 1;
    
    infectedCount=1;
    
    recovered.fill(0);
    recoveredCount=0;

    
    recovery.clear();
    recovery[iday +infection_period].push_back(inode);
    
    
    return;
}

void Graph::initializeInfection(int inode,int iday, unsigned int infection_period, unsigned int detection_period)
{
    initializeInfection(inode,iday,infection_period);
    
    detection.clear();
    detection[iday + detection_period].push_back(inode);
    
    detectedCount=0;
    
    return;
}

void Graph::infectionSweep(unsigned int day, unsigned int infection_period,
                           unsigned int detection_period, 
                           bool (Graph::*rewire)(int,int,unsigned int))
{
	// infection
	if (infectedCount>0)
	{
		NodeSet new_infectious = infect(day,rewire);
		infectedCount +=Unite(infectious, new_infectious);
    
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
      detectedCount +=Detect(infectious, (*it).second);
      
    }
}    

void Graph::removalPrototype(RdayMap& map,unsigned int day,const int KEEP_RECOVERED)
{
  
    RdayIter it = map.find(day);
    if (it != map.end())
    {
      int recovers = Recover(infectious,recovered, (*it).second,KEEP_RECOVERED);
      infectedCount -=recovers;
      detectedCount -=recovers;
      recoveredCount += recovers;
    }
}
  
//return newly infected nodes for 'day'
Graph::NodeSet Graph::infect(unsigned int day,bool (Graph::*rewire)(int,int,unsigned int))
{
  NodeSet new_infectious;
  
  EdgeList day_list = edges[day];
  if (day_list.size()>0)
  {
    for (auto b = day_list.begin(); b != day_list.end(); ++b)
    {
      int u = (*b)[0]; //source
      int v = (*b)[1]; //target
      
      // infections happening here
      // add target to infectious if source is infected and target is neither infected and nor recovered
      if (infectious.at(u) && !infectious.at(v))       // This is decides whether or not reinfection is possible
      {
        if (!recovered.at(v)){
          bool rewired_to_infected = ((this)->*rewire)(u,v,day);
          if(rewired_to_infected)
            new_infectious.push_back(v);
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
std::vector<unsigned int> Graph::SI_simulation(int inode, int iday)
{
  auto infected_recovered_farms = SIR_simulation(inode,iday,DAYS);
  auto infectedFarms = std::vector<unsigned int>(infected_recovered_farms.begin(),infected_recovered_farms.begin()+DAYS);
  
  return infectedFarms;
}

/********************************************************************
*  SIS Simulation
*  Cuts of the recovered part of SIX output
********************************************************************/
std::vector<unsigned int> Graph::SIS_simulation(int inode, int iday, unsigned int infection_period)
{
  auto infected_recovered_farms = SIX_simulation(inode,iday,infection_period,0);
 
  // discard the recovered part
  auto infectedFarms = std::vector<unsigned int>(infected_recovered_farms.begin(),infected_recovered_farms.begin()+DAYS);
  
  return infectedFarms;
}



/********************************************************************
*  SIR Simulation
*
********************************************************************/
std::vector<unsigned int> Graph::SIR_simulation(int inode, int iday, unsigned int infection_period)
{
  return SIX_simulation(inode,iday,infection_period,1);
}


/********************************************************************
*  SIX Simulation
*  Template for SIR and SIS simulations   
*
********************************************************************/
std::vector<unsigned int> Graph::SIX_simulation(int inode, int iday, unsigned int infection_period,const int KEEP_RECOVERED)
{
  auto infected_recovered_farms = std::vector<unsigned int>(2*DAYS);
  
  initializeInfection(inode,iday,infection_period);

  for (unsigned int day = iday; day < DAYS; day++)
  {     
    
    infectionSweep(day, infection_period);
    if(infection_period<DAYS){
      recoverySweep(day,KEEP_RECOVERED);
    }
    infected_recovered_farms[day-iday] = infectedCount;
    infected_recovered_farms[DAYS+(day-iday)] = recoveredCount;
  }

  return infected_recovered_farms;
}



std::vector<unsigned int> Graph::SIS_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period
													 )
{ 
  bool (Graph::*rewire)(int,int,unsigned int) = &Graph::_rewire;
 
  auto infected_farms = std::vector<unsigned int>(DAYS);
  
  initializeInfection(inode,iday,infection_period,detection_period);
  
  for (unsigned int day = iday; day < DAYS; day++)
  {     
    
    infectionSweep(day, infection_period,detection_period, rewire);
    detectionSweep(day);
    recoverySweep(day,0);
    
    infected_farms[day-iday] = infectedCount;
  }
  
  return infected_farms;
  
}

// source is infectious
bool Graph::_rewire(int source, int target, unsigned int day)
{
  int infection_status = infectious.at(source);
  if(infection_status==2){
    unsigned int latent_infected = infectedCount-detectedCount;
    unsigned int seemingly_uninfected = NODE_NUMBER-detectedCount;
    double infection_probability = latent_infected/(double)seemingly_uninfected;    
    if(SampleProbability(infection_probability)==0){
      return false;
    }
  }

  return true;
}