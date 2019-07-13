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

		void setSensorAddress(char* s);
		void setName(char* s);
		void setOccupancy(double o) 	{occupancy = o;}
		void setSpeed(double s) 		{speed = s;}
		void setVolume(int v) 			{volume = v;}
		void setDataValid(bool d) 		{dataValid = d;}
		void setNumLoops(int n)			{numLoops = n;}
		void setIncident(int i)			{incident = i;}
		void setDirection(char d)		{direction = d;}
		void setSensorType(char t)		{sensorType = t;}
		void setCabinetNum(int n)		{cabinetNum = n;}
		void setLatitude(double l)		{latitude = l;}
		void setLongitude(double l)		{longitude = l;}
		void setDistToNext(double d)	{distToNext = d;}

		double 	getOccupancy() 			{return occupancy;}
		double	getSpeed() 				{return speed;}
		int	 	getVolume() 			{return volume;}
		bool	getDataValid() 			{return dataValid;}
		int	 	getNumLoops()			{return numLoops;}
		int		getIncident()			{return incident;}
		char*	getStationAddress()		{return sensorAddress;}
		char*	getCabinetName()        {return cabinetName;}
		char*   getSensorName()			{return sensorName;}
		char	getDirection()			{return direction;}
		int		getTargetCabinetNum()	{return targetCabinetNum;}
		char	getSensorType()			{return sensorType;}
		double  getLongitude()			{return longitude;}
		double	getLatitude()			{return latitude;}
		int 	getCabinetNum()			{return cabinetNum;}
		double	getDistToNext()			{return distToNext;}
			


	private:
		//---------congestion info from sensors--------------
		double  occupancy;		//as a Percentage(occupancy = scanCount / 12)
		double  speed;		    //in mph(speed = 60*(Volume/Loops) / (2.4*Occupancy))
		int     volume;			//in vehicles per minute					
		bool	dataValid;      //is data usable or not?
		int     numLoops;		//number of traffic loops at sensor
										
		int     incident;		//NOT CURRENTLY SUPPORTED! (0 = no incident, 
								//1 = Tentative, 2 = Occurred, 3 = Continuing)

		char *	sensorName; 	//sensor(station) name
		char *	sensorAddress;	//where the station is located
		char *	cabinetName;  	//the cabinet that owns the station
		int		cabinetNum;		//the number a cabinet can be referred as (ie ES-055D = 55);
	
		//---------other info-------------------
		double	distToNext;	    //distance in miles to next cabinet, must be hard-coded ? or use lat/longitude to calculate?
		
		char	direction;		//'N'=north,'S'=south,'W'=west,'E'=east

		int     targetCabinetNum;//number of cabinet it connects to (index in graph array)

		char	sensorType;		//'H'=HOV, 'N' = normal lane

		double	longitude;		//longitude and latitude of sensor
		double	latitude;

};

#endif //SENSOR_H
