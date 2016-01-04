#ifndef GUARD_graph_h
#define GUARD_graph_h

#include <vector>
#include <map>
#include <set>
#include <list>



class Graph{
	public:
	unsigned int GROUP_NUMBER = 0;
	
	
	
	// edges
	typedef std::array<unsigned int,2> EDGE;
	typedef std::vector<EDGE> EdgeList;
	typedef std::vector<EdgeList> DayEdges;
	typedef std::array<unsigned int,97980> NodeProperty;
	
	// nodes
	typedef std::list<int> NodeSet;
	
	typedef std::map<int, NodeSet> RdayMap;
	//iterators
	typedef RdayMap::iterator RdayIter;

	typedef std::vector<std::vector<unsigned int>> GroupResult; 
	
	
	
	Graph();

	Graph(DayEdges& _edges, unsigned int _NODE_NUMBER, NodeProperty& _groups,
						 unsigned int _group_number=1);
	~Graph(){};
	
	
	// Network representation
	DayEdges& edges;
	
	unsigned int NODE_NUMBER;
	// Number of days
	unsigned int DAYS;
	
	double transmission_probability = 1; 
	
	
	std::vector<unsigned int> infected_count, recovered_count, detected_count,
								original_infections, rewired_infections;
	unsigned int summed_infected_count;
	
	std::vector<unsigned int> group_counts;
	
	// Vectors representing the state of the infection
	NodeProperty infectious,recovered;
	
	
	NodeProperty groups;
	
	
	// Map for marking the recovery day of an infected node
	RdayMap recovery,detection;
	
	void infectionSweep(unsigned int day, unsigned int infection_period,
						unsigned int detection_period=1460,
						bool (Graph::*rewire)(int,int,unsigned int)=&Graph::defaultFunc);
	
	// KEEP_RECOVERED = 1 -> SIR, =0 -> SIS
	void recoverySweep(unsigned int day,const int KEEP_RECOVERED);
	
	// move nodes to openly_infectious
	void detectionSweep(unsigned int day);
	
	bool defaultFunc(int,int,unsigned int){return true;};
	
	// prototype for recovery and detection sweeps
	void removalPrototype(RdayMap& map, unsigned int day,const int KEEP_RECOVERED);
	
	NodeSet infect(unsigned int day,bool (Graph::*rewire)(int,int,unsigned int));
	
	GroupResult 				SI_simulation(int inode, int iday);
	
	GroupResult 				SIX_simulation(int inode, int iday, unsigned int infection_period, int KEEP_RECOVERED);
	GroupResult 				SIS_simulation(int inode, int iday, unsigned int infection_period);
	GroupResult 				SIR_simulation(int inode, int iday, unsigned int infection_period);
	
	GroupResult 				SIS_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period
													 );
	
	void initializeInfection(int inode,int iday, unsigned int infection_period, unsigned int detection_period);
	void initializeInfection(int inode,int iday, unsigned int infection_period);
	
	void countGroups();
	
	
	void InitializeResultVector(unsigned int group_number, Graph::GroupResult& result);
	
	bool _rewire(int source, int target, unsigned int day);
	
	
};

#endif