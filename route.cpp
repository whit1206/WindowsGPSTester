#include "route.h"

const double INFINITY = 0x0FFFFFF;

Route::Route() : startID(-1), endID(-1), isHOV(false), pathCalculated(false)
{
	for(int i= 0; i < MAX_NODES; i++) //default to empty graph
	{
		nodeCost[i] = INFINITY;	//shortest  cost to get to node from start
		sourceNode[i] = -1; //number of node we came from
		sensorTaken[i] = -1;
	}
}


Route::~Route()
{
}

int	Route::GetStartID() const
{
	return startID;
}

void Route::SetStartID(int ID)
{
	if(ID >= 0 && ID < MAX_NODES)
		startID = ID;
}

int Route::GetEndID() const
{
	return endID;
}

void Route::SetEndID(int ID)
{
	if(ID >= 0 && ID < MAX_NODES)
		endID = ID;
}

bool Route::IsHOV() const
{
	return isHOV;
}

void Route::SetHOV(bool HOV)
{
	isHOV = HOV;
}



//calculates the edgevalue the specified sensor will have
//edgevalue = (distance to next sensor) / (sensor avg speed) * 60 = # minutes to go thru edge
double calculateEdgeCost(Sensor * s)
{
	return (s->getDistToNext() / s->getSpeed()) * (double) 60;
}

//assumes that start and endID's are correct, also assumes gnArray has no empty 
//spaces between 0-numNodes
bool Route::calculateDijkstra(Graph & g)	//calculate shortest path from start
{
	bool nodeKnown[MAX_NODES];	//array of known nodes for dijkstras algorithm

	pathCalculated = false;

	for(int i= 0; i < MAX_NODES; i++) //default to empty path table
	{
		nodeKnown[i] = false;	//array of known nodes for dijkstras algorithm
		nodeCost[i] = INFINITY;	//shortest  cost to get to node from start
		sourceNode[i] = -1; //number of node we came from
		sensorTaken[i] = -1;
	}

	//select the starting node and go form there
	nodeKnown[startID] = true;
	nodeCost[startID] = 0;	//0 distance to itself, dont set source since none
	sourceNode[startID] = -1;	//say this node comes from nowhere else (= start)

	double newEdgeCost = INFINITY;

	int curNodeNum = startID;	//start table off at starting node
	int targetNum;
	Sensor * pSensor;
	GraphNode * curNode;
	ListNode * sensorListNode;
	int sensorNum;

	//fill table of costs to nodes it connects to
	while(curNodeNum != -1)
	{
		curNode = g.GetNodeAt(curNodeNum);
		
		if(curNode == NULL)	//check to make sure node valid
			return false;

		sensorListNode = curNode->sensorList.GetFirstNode();

		sensorNum = 0;

		//go through cabinet's sensor-connection list
		while(sensorListNode != NULL)	
		{
			pSensor = (Sensor *) sensorListNode->GetObj();	//get the sensor
			
			if(pSensor != NULL) // make sure sensor is valid and of proper type
			{
				if(nodeKnown[pSensor->getTargetCabinetNum()] == false && 
					pSensor->getSensorType() == 'N' || (pSensor->getSensorType() == 'H' && isHOV))
				{
					//get taget and new connection cost
					newEdgeCost = nodeCost[curNodeNum] + calculateEdgeCost(pSensor);	
					targetNum = pSensor->getTargetCabinetNum();

					//if new cost less than old one, replace & reset path
					if(newEdgeCost < nodeCost[targetNum]) 
					{
						sensorTaken[targetNum] = sensorNum;
						nodeCost[targetNum] = newEdgeCost;	//update table entries
						sourceNode[targetNum] = curNodeNum;
					}
				}
				else
					return false;
			}
			else
				return false;	//bad sensor ptr

			sensorNum++;
			sensorListNode = sensorListNode->getNext();	//go to next sensor in list
		}

		//calculate next node to look at by finding smallest cost of free nodes
		int smallestFreeNode = -1;
		double smallestCost = INFINITY;
		for(int i = 0; i < g.getNodeCount(); i++) //find smalled unused node
		{
			if(nodeKnown[i] == false && nodeCost[i] <= smallestCost)
			{
				smallestFreeNode = i;
				smallestCost = nodeCost[i];
			}
		}
		curNodeNum = smallestFreeNode;
	}

	pathCalculated = true;
	return true;	//everything worked
}



bool Route::GetPathDirections(char * directionsBuffer, int bufSize) const
{
	return true;
}

double Route::GetPathCoordinates(Graph & g, double * latitude, double * longitude, int maxArraySize, int & retSize)
{
	if(pathCalculated == false) //must calculate path first
		return 0;

	int index = 0;
	retSize = 0;

	//start at end and work backwords
	int curNodeNum = endID;
	GraphNode * CurNode = g.GetNodeAt(curNodeNum);

	Sensor * pSensor;
	while(CurNode != NULL && index < maxArraySize)
	{
		pSensor = (Sensor *) CurNode->sensorList.GetFirstNode()->GetObj();

		if(pSensor == NULL)						//make sure sensor is valid
			break;

		latitude[index] = pSensor->getLatitude();	//copy coordinates
		longitude[index] = pSensor->getLongitude();

		index++;								//advance array index counter

		curNodeNum = sourceNode[curNodeNum];	//go back one sensor towards begining
		CurNode = g.GetNodeAt(curNodeNum);
	}

	retSize = index;

	if(curNodeNum == startID)	//every thing worked if got to start, return totaltime in min
		return nodeCost[endID];

	return 0;	//something failed
}