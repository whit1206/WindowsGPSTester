#ifndef _STA_PARSER_
#define _STA_PARSER_

#include <fstream.h>
#include "Sensor.h"
#include <string.h>
#include <stdlib.h>

//instance variables:
static const int PARSER_VERSION = 30;  //the version of webflow.sta this parser 
									   //is designed to work with! (Version 0030)
const int MAX_CHARS = 79;

class STAParser 
{
	public:

		static bool getSensorData(char* fileName, Sensor* &allSensors, int &length);
	
		static bool checkSTACompatibility(const int &thisFileVersion);
	
	private:

		static bool openFile(char* fileName, ifstream inS);


};//end class STAParser

#endif