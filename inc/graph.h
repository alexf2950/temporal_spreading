#ifndef GUARD_graph_h
#define GUARD_graph_h

#include <vector>
#include <map>
#include <set>
#include <list>



class Graph{
	public:
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

	
	
	
	Graph();

	Graph(DayEdges& _edges, unsigned int _NODE_NUMBER);
	~Graph(){};
	
	
	// Network representation
	DayEdges& edges;
	
	unsigned int NODE_NUMBER;
	// Number of days
	unsigned int DAYS;
	
	unsigned int infectedCount, recoveredCount, detectedCount;
	
	
	// Vectors representing the state of the infection
	NodeProperty infectious,recovered;
	
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
	
	std::vector<unsigned int> SI_simulation(int inode, int iday);
	
	std::vector<unsigned int> SIX_simulation(int inode, int iday, unsigned int infection_period, int KEEP_RECOVERED);
	std::vector<unsigned int> SIS_simulation(int inode, int iday, unsigned int infection_period);
	std::vector<unsigned int> SIR_simulation(int inode, int iday, unsigned int infection_period);
	
	std::vector<unsigned int> SIS_rewire_simulation(	int inode,
														int iday,
														unsigned int infection_period,
														unsigned int detection_period
													 );
	
	void initializeInfection(int inode,int iday, unsigned int infection_period, unsigned int detection_period);
	void initializeInfection(int inode,int iday, unsigned int infection_period);
	
	bool _rewire(int source, int target, unsigned int day);
	
	
};

#endif