#ifndef GUARD_custom_io_h
#define GUARD_custom_io_h

#include <string>
#include "graph.h"

void WriteEdgesToFile(Graph::DayEdges& edges,std::string filename);
void ReadEdgesC(std::string filename, Graph::DayEdges& edges );

unsigned int ReadGroups(std::string filename, Graph::NodeProperty& groups );

#endif