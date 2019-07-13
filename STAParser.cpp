#include <windows.h>
#include "STAParser.h"

//////////////////////////////////////////////////////////////////////////

bool STAParser::getSensorData(char* fileName, Sensor* &allSensors, int &numSensors)
{
	//NOTE: array allSensors[] is populated and consists of all Sensors found in webflow.sta
	
	ifstream inS;							//file input stream
	allSensors = NULL;
	numSensors	= 0;
	int fileVersion = 0;
	char* buffer = new char[MAX_CHARS];		//reading in line, same as all others you fuckhead
	
	if(!openFile(fileName, inS))				//opens a file, duh
		return false;

	//grab file version number
	inS.getline(buffer, MAX_CHARS);

	strtok(buffer, "=");					//gets the file version of the data file
	strcpy(buffer, strtok(NULL, "\n"));
	fileVersion = atoi(buffer);

	//Check Version Compatibility
	if(PARSER_VERSION != fileVersion)
		cerr << "\n\nNOTE: STA FILE VERSION MISMATCH!" << endl << endl;

	//grab number of Sensors
	inS.getline(buffer, MAX_CHARS);
	strtok(buffer, "=");
	strcpy(buffer, strtok(NULL, "\n"));
	numSensors = atoi(buffer);

	//init allSensors array for population
	allSensors = new Sensor[numSensors];	//deleted by main program later

	//read next line from file
	inS.getline(buffer, MAX_CHARS);
	char* temp = new char[MAX_CHARS];		//create token string

	//read rest of STA file
	for(int i = 0; strcmp(buffer, "END") != 0 && i < numSensors; i++)
	{
		strtok(buffer, ", ");				//get cabinet and sensor name
		strcpy(temp, strtok(NULL, "\n"));	//get full address (with roadname)

		//throw away road name
		strtok(temp, " ");

		//set sensor address
		strcpy(temp, strtok(NULL, "\n"));	//get trimmed address

		if(buffer[0] == 'E' && buffer[1] == 'S') //check valid road name
		{
			allSensors[i].setName(buffer);
			allSensors[i].setSensorAddress(temp);

			//set cabinet number
			temp[0] = allSensors[i].getCabinetName()[3];
			temp[1] = allSensors[i].getCabinetName()[4];
			temp[2] = allSensors[i].getCabinetName()[5];
			temp[3] = '\0';
			allSensors[i].setCabinetNum(atoi(temp));

			//set sensor Direction (N, S, E or W)
			allSensors[i].setDirection(allSensors[i].getSensorName()[2]);
			
			//set sensor type ('H' for HOV Lane, 'N' for normal (or Stn type data)
			allSensors[i].getSensorName()[3] == 'H' ? allSensors[i].setSensorType('H') : allSensors[i].setSensorType('N');
		
		}

		//read next line from file
		inS.getline(buffer, MAX_CHARS);
	}

	delete [] temp;			//clean up memory, delete temp :)
	delete [] buffer;
	temp = NULL;
	buffer = NULL;

	inS.close();			//close the file

	return true;			//return true:)

}	


//////////////////////////////////////////////////////////////////////////
		
bool STAParser::openFile(char* fileName, ifstream inS)
{
	inS.open(fileName, ios::nocreate); //opens file for reading

	if(inS.fail())
	{
		MessageBox(NULL, "Unable To Locate STA File.", "ERROR", MB_OK);
		return false;
		
	}//end if

	return true;

}//end openFile

//////////////////////////////////////////////////////////////////////////

bool STAParser::checkSTACompatibility(const int &thisFileVersion)
{
		return (thisFileVersion == PARSER_VERSION);
}	
	
	  
