#include <windows.h>
#include "DATParser.h"

int		DATParser::STAFileVersion = 0;		//STA File Version for Decoding
struct	stat DATParser::fileStats;
//char*	DATParser::timeStamp;  				//Time (standard)
//char*	DATParser::dateStamp; 				//Date


DATParser::DATParser()
{


}

DATParser::~DATParser(void)
{
	

}

//////////////////////////////////////////////////////////////////////////
bool DATParser::getSensorData(char* fileName, Sensor* &allSensors, const int &length)
{
	char*	fileContent = NULL;		//stores contents of DAT file

	bool res = true;	//return result


	//open DAT file for random access
	ifstream inFile(fileName, ios::in | ios::binary);
	
	if(!inFile)
	{
		MessageBox(NULL, "Unable To Locate DAT File.", "ERROR", MB_OK);
		return false;
	}

	//generate file statistics...
	//NOTE: fileStats.st_size is equal to the number of bytes
	//in the DAT file
	if(!stat(fileName, &fileStats) == 0)
        MessageBox(NULL, "Unable To Generate DAT File Stats.", "ERROR", MB_OK);


	//fill the fileContent array with bytes
	//starting at position 0, and reading until 
	//position 'fileStats.st_size'
	fileContent = new char[fileStats.st_size];
	inFile.read(fileContent, fileStats.st_size);	//read in entire DAT file contents


	//set STA file version needed to decode the DAT file
	STAFileVersion = (int) fileContent[0];

	//check Required STA File Version against our STA Parser Version
	if(!STAParser::checkSTACompatibility(STAFileVersion))
	{
		//cerr << "WARNING: STA File Version MisMatch Detected...Possible Incompatibility." << endl;
		//return false;
	}

	if(((int)fileContent[4]) > 1) //check this byte and use it to determine if the data is VALID
	{		//check to make sure it has data and date is greater than 1
		//grab time stamp & date Stamp from DAT File...
		//timeStamp = convertMilitaryTime((int)fileContent[7], (int)fileContent[8], (int)fileContent[9]);
	
		//char* temp = new char[MAX_CHARS];

		//itoa((int)fileContent[5], dateStamp, 10); //base 10
		//strcat(dateStamp, "/");
		//itoa((int)fileContent[6], temp, 10);
		//strcat(dateStamp, temp);
		//strcat(dateStamp, "/");
		//itoa((int)fileContent[4] + 2000, temp, 10);
		//strcat(dateStamp, temp);

		//read and decode file contents
		for(int i = 10; i < fileStats.st_size - 5; i+=5)
		{
			parseSensorBlock(fileContent, allSensors[((i-10) / 5)], i); 
		}

		//delete [] temp;		//for military time conversion
		//temp = NULL;

	}
	else
	{
		MessageBox(NULL, "No Data In DAT File. Parsing Unsuccessful.", "ERROR", MB_OK);
		res = false;
	}
	
	delete [] fileContent;	//delete file buffer


	inFile.close();

	return res;
}


//////////////////////////////////////////////////////////////////////////

//get a Sensor's 5 byte block and fill in it's traffic information
void DATParser::parseSensorBlock(char * fileContent, Sensor &s, int offset)
{

	int scanCount = 0;
	int volume = 0;
	double speed = 0;


	//& = and, | = or, ^ = xor
	//get first byte info
	s.setIncident(((int)fileContent[offset] & 0xC0) >> 6); //1100 0000
	
	//is the last(7th) bit set(1), if so, data is valid
	if(((fileContent[offset + 1] & 0x80) >> 7) == 1)
		s.setDataValid(true);
	else
		s.setDataValid(false);
	
	//get number of loops in Sensor
	s.setNumLoops(((int)fileContent[offset + 1] & 0x70) >> 4); //0111 0000, ">> 4" means
														  //shifting bits right 4 spaces													  
	scanCount = ((int)fileContent[offset + 1] & 0x07) << 8;	  //0000 0111 1111 1111
	scanCount += ((int)fileContent[offset + 2] & 0xFF);
	
	s.setOccupancy((double) scanCount / 12);	//now set occupancy percentage
	
	//4th data byte is high 8 bits of volume data + 5th data byte (low 8 bits of volume data)
	volume = (((int)fileContent[offset + 3] & 0xFF) << 8) + ((int)fileContent[offset + 4] & 0xFF); 
	
	s.setVolume(volume);
	
	//finally, set the APPROXIMATE Average Speed for each Sensor
	speed = (60 * ((double) volume / (double) s.getNumLoops()));
	speed = speed / (2.4 * ((double) scanCount / 12));
	s.setSpeed(speed); 	
	

}

//////////////////////////////////////////////////////////////////////////
/*
char* DATParser::convertMilitaryTime(int hours, int mins, int secs)
{
	char* amPm = (hours > 11 ? "pm" : "am");
	char* temp = new char[MAX_CHARS];
	char* retString = new char[MAX_CHARS];
		
	itoa(hours % 12, retString, 10); //10 = decimal number system (base 10)
	strcat(retString, ":");
	itoa(mins, temp, 10);
	strcat(retString, temp);
	strcat(retString, ":");
	itoa(secs, temp, 10);
	strcat(retString, temp);
	strcat(retString, amPm);
	strcat(retString, " (Pacific Daylight Time)\0");


	return retString;

}
*/
//////////////////////////////////////////////////////////////////////////

