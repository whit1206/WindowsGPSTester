#ifndef _LL_PARSER_
#define _LL_PARSER_

#include <fstream.h>
#include "Sensor.h"
#include <windows.h>
#include "STAParser.h" //for MAX_CHARS

const int MAX_SENSORS = 999;

class LLParser
{

	public:
		
		//Data in TXT file is and must be in the following line format:
		//CABINET_ID,COORD_TYPE,AUTHORITY,VALUE1,VALUE2,VALUE3
		//	(ie: ES-055D,geodetic,WSDOT,47.4374,-122.27,NULL)
		static bool getLLData(char* fileName, Sensor* &allSensors, int &length);


	private:

		static bool openFile(char* fileName, ifstream inS);

		struct LLNode
		{
			LLNode(void) :  source(NULL),	//entity of source of data
  							cabinetName(NULL),
							type(NULL),
							longitude(0.00),	//in ddd.dddd format
							latitude(0.00),
							x(-1),		    //-1 means dont draw, else on screen coordinate
							y(-1),
							cabinetNum(0) { }

			~LLNode()		//cleans up memory
			{
				if(source)
					delete [] source;
				if(cabinetName)
					delete [] cabinetName;
				if(type)
					delete [] type;
			}
	

			char*  source;	//entity of source of data
			char*  cabinetName;
			int	   cabinetNum;
			char*  type;
			double longitude;	//in ddd.dddd format
			double latitude;
			double x;	//-1 means dont draw, else on screen coordinate
			double y;
			
		};




};

#endif