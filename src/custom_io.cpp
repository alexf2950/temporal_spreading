#include <iostream>
#include <string> 
#include <cstdio>
#include <fstream>
#include <sstream>
#include <random>
#include <map>
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

void ReadEdgesC(std::string filename, Graph::DayEdges& edges, const int EDGE_NUMBER, const int RANDOM_FLAG, 
						 const unsigned int REVERSED ){
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
		
		if(REVERSED)
		{
			edges[day].push_back(Graph::EDGE({target,source}));
		}
		else
		{		
			edges[day].push_back(Graph::EDGE({source,target}));
		}

		
	}
	
	return;
}

void ReadEdgesTxt(std::string filename, Graph::DayEdges& edges ){
	std::ifstream file;
	file.open (filename, std::ifstream::in); 
	
	std::string line, number;
	char seperator = '\t';
	
	unsigned int index=0;
	unsigned int max_day=0;
	unsigned int max_index=0;
	unsigned int day,source,target;

	while(std::getline(file,line,'\n'))
    {	
		std::istringstream line_iss(line);
		std::getline(line_iss,number, seperator);
		source = stoi(number);
		std::getline(line_iss,number, seperator);
		target = stoi(number);
		std::getline(line_iss,number, seperator);
		day = stoi(number);
		
		if(day>max_day)
			max_day = day;
		if(source>max_index)
			max_index = source;
		if(target>max_index)
			max_index = target;		
		
		edges[day].push_back(Graph::EDGE({source,target}));
    }
	
	std::cout << "DAYS = " << max_day << " NODE_NUMBER = " << max_index << "\n";
	
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

void aggregateNetwork(Graph::DayEdges& edges, Graph::NodeProperty& groups,
 unsigned int group_number, std::string out_filename)
{
	std::map<Graph::EDGE,unsigned int> edge_count_map;
	std::vector<std::vector<unsigned int>> group_coupling(group_number);
	
	for (unsigned int i  = 0;i<group_number;++i)
	{
		group_coupling[i].assign(group_number,0);
	}
	
	for(unsigned int day = 0;day<edges.size();++day){
		auto day_list = edges[day];
		
		for (auto edge = day_list.begin();edge!=day_list.end();++edge){
			edge_count_map[*edge]++;
			
			//coupling matrix
			unsigned int source = (*edge)[0];
			unsigned int target = (*edge)[1];
			group_coupling[groups[source]][groups[target]]+=1;
		}
		
		edges[day].clear();
	}
	
	std::ofstream file;
	file.open("data/coupling_matrix_.txt",std::ofstream::out | std::ofstream::trunc);
	
	for (unsigned int i  = 0;i<group_number;++i)
	{
		for (unsigned int j  = 0;j<group_number;++j)
		{
			file << group_coupling[i][j] << " ";
		}
		file << "\n";
	}
	file.close();
	
	edges.resize(97980);
	for (auto edge = edge_count_map.begin();edge!=edge_count_map.end();++edge){
			edges[edge->first[0]].push_back(edge->first);
		}
	WriteEdgesToFile(edges,out_filename);
	return;
}