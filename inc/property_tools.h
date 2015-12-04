#ifndef GUARD_property_tools_h
#define GUARD_property_tools_h

#include <set>
#include <vector>
#include "graph.h"



int Recover(Graph::NodeProperty& inf,
				Graph::NodeProperty& rec,
				const Graph::NodeSet& set, const int KEEP_RECOVERED);
int Unite(Graph::NodeProperty& v, const Graph::NodeSet& set);


int Detect(Graph::NodeProperty& v, const Graph::NodeSet& set);

int UpdateVector(Graph::NodeProperty& v, const Graph::NodeSet& set, bool (*changeOperation)(unsigned int&));

bool InfectNode(unsigned int& node_status);
bool DetectNode(unsigned int& node_status);
bool RecoverNode(unsigned int& node_status);

bool ChangeStatus(unsigned int& node_status, const int& from, const int& to);


unsigned int countInfected(Graph::NodeProperty& v,int n=1);

#endif
