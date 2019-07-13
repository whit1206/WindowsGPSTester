//Sensor.h - class to hold all sensor info

#ifndef SENSOR_H
#define SENSOR_H

class GraphNode;	//prototype

class Sensor
{
	public:
		Sensor();
		Sensor(const Sensor & s);		//Constructor - inits all vars to empty/not used
		~Sensor();		//Deconstructor - cleans up memory

		Sensor & operator=(const Sensor & rhs);


	//private:
		//---------congestion info from sensors--------------
		double  occupancy;		//as a Percentage(occupancy = scanCount / 12)
		double  speed;		    //in mph(speed = 60*(Volume/Loops) / (2.4*Occupancy))
		int     volume;			//in vehicles per minute					
		bool	dataValid;      //is data usable or not?
		int     numLoops;		//number of traffic loops at sensor
										
		int     incident;		//NOT CURRENTLY SUPPORTED! (0 = no incident, 
								//1 = Tentative, 2 = Occurred, 3 = Continuing)

		char *	stationName; 	//sensor(station) name
		char *	stationAddress;	//where the station is located
		char *	cabinetName;  	//the cabinet that owns the station
		char *	roadName;		//name of road it belongs on
		
		//---------other info-------------------
		double	distToNext;	    //distance in miles to next cabinet
		
		char	direction;		//'N'=north,'S'=south,'W'=west,'E'=east

		int     targetCabinetNum;//number of cabinet it connects to (index in graph array)

		char	SensorType;		//'H'=HOV, 'N' = normal lane

		double	longitude;		//longitude and latitude of sensor
		double	latitude;

};

#endif //SENSOR_H
