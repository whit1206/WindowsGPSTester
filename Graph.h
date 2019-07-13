//Graph.h - class to hold all sensors and how they interconnect
//          used later for finding shortest route by other class

#ifndef GRAPH_H
#define GRAPH_H

#include <windows.h>
#include <stdio.h>
#include "sensor.h"
#include "list.h"

const int MAX_NODES = 1050;

class GraphNode;	//prototype for graphnode (cabinet) class

class Graph			//holds graph data structure of cabinets with sensors and how connect
{
	public:
		Graph();		//Constructor - inits all vars to empty/not used
		~Graph();		//Deconstructor - cleans up memory

		bool BuildGraph();	//download files, input data

		GraphNode * AddNode(Sensor * s); //add a node and return its pointer

		GraphNode * GetNodeAt(int pos);	//gets node pointer from position in array

		void Clear();		//clear out all nodes

		int getNodeCount() const;


	private:
		int numNodes;					//number of nodes currently in node
		GraphNode * gnArray[MAX_NODES];	//array of graphnode pointers, set by builder fn

		long GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName, LPCSTR lpOutFileName);

};



class GraphNode		//a node in the graph (same as a cabinet)
{
	public:
		GraphNode();		//Constructor - inits all vars to empty/not used
		~GraphNode();		//Deconstructor - cleans up memory

		int graphArrayPosition;

		bool addSensor(Sensor * s);

//	private:
		List sensorList;	//our list of sensors
		

};

#endif //GRAPH_H
