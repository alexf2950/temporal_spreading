#include <iostream>
#include <string> 
#include <cstdio>
#include "graph.h"
#include "custom_io.h"

const unsigned int EDGE_NUMBER = 6359697;
const unsigned int MAX_DAYS = 1460;

void WriteEdgesToFile(Graph::DayEdges& edges,std::string filename){
	FILE* file= fopen(filename.c_str(),"wb");
	unsigned int edgeCounter =0;
	for(unsigned int day = 0;day<MAX_DAYS;++day){
		auto day_list = edges[day];
		for (auto edge = day_list.begin();edge!=day_list.end();++edge){
			fwrite(&day,sizeof(int),1,file);
			fwrite(&(*edge)[0],sizeof(int),1,file);
			fwrite(&(*edge)[1],sizeof(int),1,file);
			++edgeCounter;
		}
	}
	fclose(file);
	std::cout << edgeCounter << "\n";
}

void ReadEdgesC(std::string filename, Graph::DayEdges& edges ){
	std::vector<unsigned int> data(EDGE_NUMBER*3);
	
	FILE* file = fopen(filename.c_str(),"rb");
	fread(data.data(),sizeof(int),EDGE_NUMBER*3,file);
	fclose(file);
	
	
	
	for(unsigned int i=0;i<EDGE_NUMBER;++i){
		unsigned int day = data[3*i];
		unsigned int source = data[3*i+1];
		unsigned int target = data[3*i+2];
		
		
		edges[day].push_back(Graph::EDGE({source,target}));
		
	}
	
	return;
}