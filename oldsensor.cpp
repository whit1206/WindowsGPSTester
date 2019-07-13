#include <string.h>
#include "sensor.h"

Sensor::Sensor() : occupancy(0), speed(0), volume(0), dataValid(false),
                   numLoops(0), incident(0), stationName(0),
				   stationAddress(0), cabinetName(0), roadName(0),
				   distToNext(0), direction(0), targetCabinetNum(-1),
				   SensorType(0), longitude(0), latitude(0)
{

}

Sensor::Sensor(const Sensor & s) : occupancy(0), speed(0), volume(0), dataValid(false),
                   numLoops(0), incident(0), stationName(0),
				   stationAddress(0), cabinetName(0), roadName(0),
				   distToNext(0), direction(0), targetCabinetNum(-1),
				   SensorType(0), longitude(0), latitude(0)
{
	*this = s;	//let = operator do all copy work
}



Sensor::~Sensor()		//Deconstructor - cleans up memory
{
	if(stationName != 0)
		delete stationName;
	if(stationAddress != 0)
		delete stationAddress;
	if(cabinetName != 0)
		delete cabinetName;
	if(roadName != 0)
		delete roadName;
	
	stationName = 0;
	stationAddress = 0;
	cabinetName = 0;
	roadName = 0;

}


Sensor & Sensor::operator=(const Sensor & rhs)	//do a deep copy
{
	if(this != &rhs) //only do copy if not ourselves
	{
		occupancy = rhs.occupancy;	//copy congestion info
		speed = rhs.speed;
		volume = rhs.volume;
		dataValid = rhs.dataValid;
		numLoops = rhs.numLoops;
		incident = rhs.incident;
		
		if(stationName != 0)	//delete old strings if they exist
			delete stationName;
		if(stationAddress != 0)
			delete stationAddress;
		if(cabinetName != 0)
			delete cabinetName;
		if(roadName != 0)
			delete roadName;
		
		stationName = 0;	//set stings blank
		stationAddress = 0;
		cabinetName = 0;
		roadName = 0;

		if(rhs.stationName != 0)	//copy strings only if exist for rhs
		{
			stationName = new char[strlen(rhs.stationName) + 1];
			strcpy(stationName, rhs.stationName);
		}
		if(rhs.stationAddress != 0)
		{
			stationAddress = new char[strlen(rhs.stationAddress) + 1];
			strcpy(stationAddress, rhs.stationAddress);
		}
		if(rhs.cabinetName != 0)
		{
			cabinetName = new char[strlen(rhs.cabinetName) + 1];
			strcpy(cabinetName, rhs.cabinetName);
		}
		if(rhs.roadName != 0)
		{
			roadName = new char[strlen(rhs.roadName) + 1];
			strcpy(roadName, rhs.roadName);
		}

		distToNext = rhs.distToNext;	//copy other sensor info
		direction = rhs.direction;
		targetCabinetNum = rhs.targetCabinetNum;
		SensorType = rhs.SensorType;
		longitude = rhs.longitude;
		latitude = rhs.latitude;

	}

	return *this;  //return ourselves so we can string operator
}