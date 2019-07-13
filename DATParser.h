#ifndef _DAT_PARSER_
#define _DAT_PARSER_

#include <stdio.h>
#include <stdlib.h>
#include "Sensor.h"
#include "STAParser.h"
#include <fstream.h>
#include <sys/stat.h>	//for file size


class DATParser
{
	public:
	
		//static char timeStamp[20];  	//Time (standard)
		//static char dateStamp[20]; 	//Date
		
		//////////////////////////////////////////////////////////////////////////

		//NOTE: !! ?? Sometimes the dat file is empty (no data)... what should we do then?
		//When there is no data, the file contains no information other than the version number
		//needed to decode it... There is no time, date, Sensor data...nothing! But
		//the file does not decrease in size(bytes).. so for now we just check the date
		//and time bytes and if they're '0' then we know the dat file contains no data
		//
		
		DATParser(void);
		~DATParser(void);

		static bool getSensorData(char* fileName, Sensor* &allSensors, const int &length);


	private:

		//instance variables:
		static int STAFileVersion;			//STA File Version for Decoding
		static struct stat fileStats;		//statistics regarding DAT file (ie size in bytes, etc...)


		//static char* convertMilitaryTime(int hours, int mins, int secs);
		static void parseSensorBlock(char * fileContent, Sensor &s, int offset);		
	
};

#endif