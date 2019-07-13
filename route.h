// route.h - figures out how to calculate fastest route using graph
//             which was built with real time traffic information

#ifndef ROUTE_H
#define ROUTE_H

#include "graph.h"


class Route
{
	public:
		Route();
		~Route();

		int	 GetStartID() const;	//cabinet index in graph array
		void SetStartID(int ID);

		int  GetEndID() const;
		void SetEndID(int ID);

		bool IsHOV() const;
		void SetHOV(bool HOV);

		bool calculateDijkstra(Graph & g);	//calculate shortest path from start

		bool GetPathDirections(char * directionsBuffer, int bufSize) const;

		//get points of path first is start,, ......  ,,end
		//returns total travel time in minutes
		double GetPathCoordinates(Graph & g, double * latitude, double * longitude, int maxArraySize, int & retSize);

	private:
		//shortest path stuff
		double nodeCost[MAX_NODES];	//shortest  cost to get to node from start
		int  sourceNode[MAX_NODES]; //number of node we came from
		int  sensorTaken[MAX_NODES];//number of the prev sensor took to get here
		bool pathCalculated;

		int  startID;	//start cabinet ID
		int  endID;		//end cabinet ID
		bool isHOV;	//true = car is 2 or more people
};

#endif //ROUTE_H