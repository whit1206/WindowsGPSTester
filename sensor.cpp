#include <string.h>
#include "sensor.h"

Sensor::Sensor() : occupancy(0), speed(0), volume(0), dataValid(false),
                   numLoops(0), incident(0), sensorName(0),
				   sensorAddress(0), cabinetName(0), 
				   distToNext(0), direction(0), targetCabinetNum(-1),
				   sensorType(0), longitude(0), latitude(0), cabinetNum(0)
{

}

Sensor::Sensor(const Sensor & s) : occupancy(0), speed(0), volume(0), dataValid(false),
                   numLoops(0), incident(0), sensorName(0),
				   sensorAddress(0), cabinetName(0), 
				   distToNext(0), direction(0), targetCabinetNum(-1),
				   sensorType(0), longitude(0), latitude(0), cabinetNum(0)
{
	*this = s;	//let = operator do all copy work
}



Sensor::~Sensor()		//Deconstructor - cleans up memory
{
	if(sensorName != 0)
		delete sensorName;
	if(sensorAddress != 0)
		delete sensorAddress;
	if(cabinetName != 0)
		delete cabinetName;

	sensorName = 0;
	sensorAddress = 0;
	cabinetName = 0;

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
		
		if(sensorName != 0)	//delete old strings if they exist
			delete sensorName;
		if(sensorAddress != 0)
			delete sensorAddress;
		if(cabinetName != 0)
			delete cabinetName;
	
		sensorName = 0;	//set stings blank
		sensorAddress = 0;
		cabinetName = 0;

		if(rhs.sensorName != 0)	//copy strings only if exist for rhs
		{
			sensorName = new char[strlen(rhs.sensorName) + 1];
			strcpy(sensorName, rhs.sensorName);
		}
		if(rhs.sensorAddress != 0)
		{
			sensorAddress = new char[strlen(rhs.sensorAddress) + 1];
			strcpy(sensorAddress, rhs.sensorAddress);
		}
		if(rhs.cabinetName != 0)
		{
			cabinetName = new char[strlen(rhs.cabinetName) + 1];
			strcpy(cabinetName, rhs.cabinetName);
		}
		
		distToNext = rhs.distToNext;	//copy other sensor info
		direction = rhs.direction;
		targetCabinetNum = rhs.targetCabinetNum;
		sensorType = rhs.sensorType;
		longitude = rhs.longitude;
		latitude = rhs.latitude;

	}

	return *this;  //return ourselves so we can string operator
}

void Sensor::setSensorAddress(char* s) 
{
	sensorAddress = new char[strlen(s)];
	strcpy(sensorAddress, s);
}
		
void Sensor::setName(char* s) 
{ 
	cabinetName = new char[strlen(s)];

	strcpy(cabinetName, strtok(s, ":"));

	sensorName = new char[strlen(s)];
	strcpy(sensorName, strtok(NULL, "\n"));	//set sensor name

}


		