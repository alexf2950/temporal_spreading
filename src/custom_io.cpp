#include <iostream>
#include <string> 
#include <cstdio>
#include <fstream>
#include <sstream>
#include <random>
#include "graph.h"
#include "custom_io.h"

std::default_random_engine def_generator;



void WriteEdgesToFile(Graph::DayEdges& edges,std::string filename){
	FILE* file= fopen(filename.c_str(),"wb");
	unsigned int edgeCounter =0;
	for(unsigned int day = 0;day<edges.size();++day){
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

void ReadEdgesC(std::string filename, Graph::DayEdges& edges, const int EDGE_NUMBER, const int RANDOM_FLAG){
	std::vector<unsigned int> data(EDGE_NUMBER*3);
	
	FILE* file = fopen(filename.c_str(),"rb");
	fread(data.data(),sizeof(int),EDGE_NUMBER*3,file);
	fclose(file);
	
	
	
	for(unsigned int i=0;i<EDGE_NUMBER;++i){
		unsigned int day = data[3*i];
		unsigned int source = data[3*i+1];
		unsigned int target = data[3*i+2];
		
		if (RANDOM_FLAG == 1)
		{
			
			std::uniform_int_distribution<int> distribution(0,edges.size()-1);
			day = distribution(def_generator);
		}
		
		edges[day].push_back(Graph::EDGE({source,target}));
		
	}
	
	return;
}

void ReadEdgesTxt(std::string filename, Graph::DayEdges& edges ){
	std::ifstream file;
	file.open (filename, std::ifstream::in); 
	
	std::string line, number;
	
	unsigned int index=0;
	unsigned int day,source,target;

	while(std::getline(file,line,'\n'))
    {	
		std::istringstream line_iss(line);
		std::getline(line_iss,number, '\t');
		source = stoi(number);
		std::getline(line_iss,number, '\t');
		target = stoi(number);
		std::getline(line_iss,number, '\t');
		day = stoi(number);
		
		edges[day].push_back(Graph::EDGE({source,target}));
    }
	
	
	file.close();
	
	return;
}

unsigned int ReadGroups(std::string filename, Graph::NodeProperty& groups ){

	
	std::ifstream file;
	file.open (filename, std::ifstream::in); 
	
	std::string cell;
	unsigned int max_group = 0;
	
	unsigned int index=0;

	while(std::getline(file,cell,','))
    {
		unsigned int group = stoi(cell);
        groups[index] = group;
		if(group>max_group)
			max_group=group;
		++index;
    }
	
	
	file.close();
	
	
	return max_group+1;
}