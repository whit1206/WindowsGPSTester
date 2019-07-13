#include <tchar.h>
#include <windows.h>
#include <iostream.h>

#include "gps.h"
#include "Serial.h"

bool Connected;

//worker thread function for listening to the comport
DWORD WINAPI ListenPort(LPVOID lpParameter);

GPS::GPS()		//constructor, comport is the # of the comport the GPS is connected to
{
	longitude = 0;
	latitude = 0;
	altitude = 0;
	time = 0;
	speed = 0;
	Connected = false;
	PositionValid = false;
	port = 1;
	ThreadID = 0;
	ThreadHandle = 0;
	GotTime = false;

	// create named mutex for access to list
	hMutex = CreateMutex(NULL,FALSE,"GPSListAccessMutex");  

	if (hMutex == NULL) 
	{
		MessageBox(NULL, "Error: Failed to create Mutex!", "GPS Error", MB_OK);
	}

	pGPSCommandStringList = new List;	//create list that will hold commands
}


GPS::~GPS()					//cleans up memory
{
	DisconnectFromComPort();	//closes thread and disconnects comport
	CloseHandle(hMutex);
	delete pGPSCommandStringList;
}

void GPS::UpdateGPSData()
{
	DWORD dwWaitResult; 
	ListNode * ln;
	int i = 0;

	// Request ownership of mutex.
	dwWaitResult = WaitForSingleObject(hMutex,	1000L); //1-second time-out

	if(dwWaitResult == WAIT_OBJECT_0) // The thread got mutex ownership.
	{
		while((ln = pGPSCommandStringList->RemoveHead()) != NULL && i < 10)
		{
			i++;		//make sure only remove up to 10 items not to delay too long
			ParseGPSData((char *) ln->GetObj(), ln->getObjSize());
			delete ln->GetObj();
			delete ln;
		}

		if(pGPSCommandStringList->GetSize() > 15)	//make sure list never gets too big
			pGPSCommandStringList->DeepClear();		//so display never lags too far behind
	}

	if (! ReleaseMutex(hMutex)) { 
		MessageBox(NULL, "Error: failed on release mutex in update!", "Errir", MB_OK);
	}

}

bool GPS::ConnectToComPort(int comport)		//returns true if connected, else false=not connected
{
	Connected = true;

	GPSThreadInfo * tInfo = new GPSThreadInfo;	//deleted later by other thread
	tInfo->comport = comport;
	tInfo->pGPSCommandStringList = pGPSCommandStringList;
	//if we got to here then create a thread that will listen to the com port
	if((ThreadHandle = CreateThread(NULL, 0, ListenPort, (void *) tInfo,0, &ThreadID)) != NULL)
	{
		return true;
	}

	return  false;	//create thread didnt work
}

bool GPS::DisconnectFromComPort()
{
	Connected = false;

	// Close the port again
	CloseHandle(ThreadHandle);

	int i = 0;
	DWORD threadstatus;
	GetExitCodeThread(this->ThreadHandle, &threadstatus);

	while(threadstatus == STILL_ACTIVE && i < 100)
	{
		i++;
		GetExitCodeThread(this->ThreadHandle, &threadstatus);
	}

	return true;
}


bool GPS::IsConnected() const
{
	return Connected;
}


bool GPS::IsPositionValid() const
{
	return PositionValid;
}


double GPS::GetLongitudeDouble() const
{
	return longitude;
}


double GPS::GetLatitudeDouble() const
{
	return latitude;
}


double GPS::GetAltitudeDouble() const
{
	return altitude;
}

double	GPS::GetSpeedDouble() const  //in mph
{
	return speed;
}

int	GPS::GetTimeInt() const
{
	return (time + 170000) % 240000;	//adjusted for seattle time
}

bool GPS::GotTimeYet() const	//returns true if time has been sent from GPS
{
	return GotTime;
}

DWORD WINAPI ListenPort(LPVOID lpParam)
{
	//get copy of info passed to us and delete structure 
	GPSThreadInfo * tInfo = (GPSThreadInfo *) lpParam;
	int comport = tInfo->comport;
	List * pGPSCommandStringList = tInfo->pGPSCommandStringList;
	delete tInfo;

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "GPSListAccessMutex");

	if(hMutex == NULL)
	{
		MessageBox(NULL, "Error: failed to open mutex in GPS!", "Error", MB_OK);
		Connected = false;
		return 0;
	}

	CSerial * pSerialPort = new CSerial;	//create a serial port
	
	LONG    lLastError = ERROR_SUCCESS;

	if(comport < 1 && comport > 4)
		comport = 1;	//check for only comports 1-4

    // Attempt to open the serial port
    if(comport == 1)
		lLastError = pSerialPort->Open(_T("COM1"));
	else if(comport == 2)
		lLastError = pSerialPort->Open(_T("COM2"));
	else if(comport == 3)
		lLastError = pSerialPort->Open(_T("COM3"));
	else 
		lLastError = pSerialPort->Open(_T("COM4"));

	if (lLastError != ERROR_SUCCESS)	//was error?
	{
		MessageBox(NULL, "Error: Unable to open COM-port for GPS", "GPS Error", MB_OK);
		Connected = false;
		return 0;
	}

    // Setup the serial port (4800,N81) to NMEA standard using hardware handshaking
    lLastError = pSerialPort->Setup(CSerial::EBaud4800,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Error: Unable to set COM-port setting for GPS", "GPS Error", MB_OK);
		Connected = false;
		return 0;
	}

	// Setup handshaking
    lLastError = pSerialPort->SetupHandshaking(CSerial::EHandshakeOff);	//no flow control
	if (lLastError != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Error: Unable to set COM-port handshaking for GPS", "GPS Error", MB_OK);
		Connected = false;
		return 0;
	}


	//now begin reading in from comport...	
	bool stringBegin = false;
	char * pData = new char[THREAD_STORE_BYTES];	//create buffer

	char * GPSString;// = new char[128];
	int GPSStrlen = 0;
	char * pStrBeg = NULL;

	DWORD bytesRead = 0;		//# of bytes we got from com portb

	while(Connected)	//read data from com port into buffer
	{
		bytesRead = 0;
	    lLastError = pSerialPort->Read((void *) pData, THREAD_STORE_BYTES, &bytesRead);

		Sleep(1);	//done to make the processor not take 99% of the processor!!!!
		
		if (lLastError == ERROR_SUCCESS && bytesRead > 0)
		{
			pStrBeg = pData;	//start copying from begining of buffer
			
			while((DWORD) pStrBeg <= (DWORD) (pData + bytesRead)) //go thru buffer
			{
				if(*pStrBeg == '$' || GPSStrlen > 126)	//if too big str or $ then start string over
				{
					GPSString = new char[128];	//create new string to go into list
					stringBegin = true;
					GPSStrlen = 0;
				}
				
				if(GPSString && stringBegin && *pStrBeg != 0)		//copy characters between $ and CR inclusive
					GPSString[GPSStrlen++] = *pStrBeg;
				
				if(*pStrBeg == 13 && GPSString != NULL)	//carriage return
				{
					GPSString[GPSStrlen++] = '\0';	//make it a normal string
					stringBegin = false;
					
					//put string in GPS's buffer
					DWORD dwWaitResult; 

					// Request ownership of mutex.
 
					dwWaitResult = WaitForSingleObject(hMutex,	2500L); //2.5-second time-out
 
					if(dwWaitResult == WAIT_OBJECT_0) // The thread got mutex ownership.
					{
						pGPSCommandStringList->AddItemToEnd(GPSString, GPSStrlen);
						GPSStrlen = 0;
						GPSString = NULL;	//stored in list now
						// Release ownership of the mutex object.
						if (! ReleaseMutex(hMutex)) { 
							MessageBox(NULL, "Error: failed on release mutex in thread!", "Errir", MB_OK);
						}
					}


				}	//end of while still data in buffer

				pStrBeg++;//advance buffer data pointer
			}
		}
	}

	if(GPSString)
		delete [] GPSString;

	delete [] pData;	//cleanup buffer

	delete pSerialPort;

	CloseHandle(hMutex);

	return 0;
}

bool GPS::ParseGPSData(char *pData, int pDataSize)
{
	if(pData[0] == '$' && pData[1] == 'G' && pData[2] == 'P')	//$GP
	{
		if(CompareType(pData + 3, "GGA"))
			return parseGPGGA(pData + 7, pDataSize);			
		else if(CompareType(pData + 3, "GLL"))
			return parseGPGLL(pData + 7, pDataSize);		
		else if(CompareType(pData + 3, "GSA"))
			return parseGPGSA(pData + 7, pDataSize);		
		else if(CompareType(pData + 3, "GSV"))
			return parseGPGSV(pData + 7, pDataSize);			
		else if(CompareType(pData + 3, "RMC"))
			return parseGPRMC(pData + 7, pDataSize);		
		else 
			return false;
	} else
		return false;
}

// The compare type function gets two char *'s. The first is a pointer
// to an index in the main data array and the second is the three digit 
// character code we are comparing with it. If they match return true, else
// return false.

bool GPS::CompareType(char *pData, char *pCode)
{
	if(pData[0] == pCode[0] && pData[1] == pCode[1] && pData[2] == pCode[2])
		return true;
	else
		return false;
}


// The GetNextToken function copies a token from the source thread (the 
// thread from the GPS) to the destination thread which will be used to 
// store variables.

void GPS::GetNextToken(char* pSrc, char* pDest, int sourceSize, int destSize, int &i)
{
	int j;
	// clear out buffer
	for(j = 0; j < destSize; j++)
		pDest[j] = 0;

	// copy a token to the temp thread
	for(j = 0; i < sourceSize && j < destSize; i++, j++)
		if(pSrc[i] != ',' && pSrc[i] != 13 && pSrc[i] != '*')
			pDest[j] = pSrc[i];
		else
			break;

	i++;
}


bool GPS::parseGPGGA(char *pString, int StringSize)	//GPS fix data
{
	Connected = true;
	char temp[256];
	//	char *temp = new char[THREAD_STORE_BYTES];
	int i = 0;

	// get the time
	GetNextToken(pString, temp, StringSize, 256, i);
	this->time = atoi(temp);

	GotTime = true;


	// get the lattitude
	GetNextToken(pString, temp, StringSize, 256, i);
	latitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the latitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char latDir = *temp;

	// get the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	longitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char lonDir = *temp;

	// figure out the validity of the data
	GetNextToken(pString, temp, StringSize, 256, i);
	if(*temp == '1' || *temp == '2')
		PositionValid = true;
	else {
		PositionValid = false;
//		delete [] temp;
		return false;
	}

	// do we need to clear out the pString array??

	// number of satelites
	GetNextToken(pString, temp, StringSize, 256, i);

	// dilution of precision
	GetNextToken(pString, temp, StringSize, 256, i);
	
	// get temporary altitude
	GetNextToken(pString, temp, StringSize, 256, i);
	altitude = atof(temp);

	// get altitude type
	GetNextToken(pString, temp, StringSize, 256, i);
	char altitudeType = *temp;

	// height above elispe
	GetNextToken(pString, temp, StringSize, 256, i);

	// elipse height type
	GetNextToken(pString, temp, StringSize, 256, i);

	// time of last update
	GetNextToken(pString, temp, StringSize, 256, i);

	// station ID
	GetNextToken(pString, temp, StringSize, 256, i);

	// checksum
	GetNextToken(pString, temp, StringSize, 256, i);

	
	/* This will come in handy for display purposes.
	latitude = latDir + " " + (int) dblLatitude + "° " + (((int) (dblLatitude * 100 )) % 100) + "'" +
			((dblLatitude * 100 ) - ((int) (dblLatitude * 100 ))) * 60 + "\""; */

	double mintodegree = (latitude - ( (int) latitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
	latitude = ( (int) latitude) + mintodegree; //ddd.ddddd format

	if(latDir == 'S')
		latitude *= -1; //needs a - in front if south
	

	/* This will be useful for display class
	longitude = longDir + " " + (int) dblLongitude + "° " + (((int) (dblLongitude * 100 )) % 100) + "'" +
			((dblLongitude * 100 ) - ((int) (dblLongitude * 100 ))) * 60 + "\"";	*/
	mintodegree = (longitude - ( (int) longitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
	longitude = ( (int) longitude) + mintodegree; //ddd.dddd format

	if(lonDir == 'W')
		longitude *= -1; //needs a - in front if west

	if(altitudeType == 'M')
		altitude = altitude * 3.28084; // Convert Meters to feet

//	delete [] temp;

	return true;
}


bool GPS::parseGPGLL(char *pString, int StringSize)
{
	Connected = true;
	char temp[256];
	//	char *temp = new char[THREAD_STORE_BYTES];
	int i = 0;

	// get the lattitude
	GetNextToken(pString, temp, StringSize, 256, i);
	latitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the latitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char latDir = *temp;

	// get the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	longitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char lonDir = *temp;

	char valid = 'A';
	
	if(pString[i - 1] != '*') {
		// get the time
		GetNextToken(pString, temp, StringSize, 256, i);
		time = atoi(temp);
		GotTime = true;

		// get the validity char
		GetNextToken(pString, temp, StringSize, 256, i);		
		valid = *temp;
	}

	if(valid == 'A') {
		double mintodegree = (latitude - ( (int) latitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
		latitude = ( (int) latitude) + mintodegree; //ddd.ddddd format

		if(latDir == 'S')
			latitude *= -1; //needs a - in front if south
		
		mintodegree = (longitude - ( (int) longitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
		longitude = ( (int) longitude) + mintodegree; //ddd.dddd format

		if(lonDir == 'W')
			longitude *= -1; //needs a - in front if west	
		
//		delete [] temp;

		return true;
	}

//	delete [] temp;

	return false;
}


// GPGSA is used to get DOP and active satellites, I don't think we really 
// need this.
bool GPS::parseGPGSA(char *pString, int StringSize)
{
	return true;
}


// GPS satellites in view. I don't think we need this one either.
bool GPS::parseGPGSV(char *pString, int StringSize)
{
	return true;
}


bool  GPS::parseGPRMC(char *pString, int StringSize)
{
	Connected = true;
	char temp[256];
	//char *temp = new char[256];
	int i = 0;

	// get the time
	GetNextToken(pString, temp, StringSize, 256, i);
	time = atoi(temp);

	GotTime = true;

	// figure out the validity of the data
	GetNextToken(pString, temp, StringSize, 256, i);
	if(strcmp(temp, "A") == 0)
		PositionValid = true;
	else {
		PositionValid = false;
	//	delete [] temp;
		return false;
	}

	// get the latitude
	GetNextToken(pString, temp, StringSize, 256, i);
	latitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the latitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char latDir = *temp;

	// get the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	longitude = atof(temp) / 100; //dddmm.mmmm - > ddd.mmmmmm

	// get the direction of the longitude
	GetNextToken(pString, temp, StringSize, 256, i);
	char lonDir = *temp;

	double mintodegree = (latitude - ( (int) latitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
	latitude = ( (int) latitude) + mintodegree; //ddd.ddddd format

	if(latDir == 'S')
		latitude *= -1; //needs a - in front if south
	

	mintodegree = (longitude - ( (int) longitude) ) * 100 / 60; //ddd.mmmmmm -> 0.dddd
	longitude = ( (int) longitude) + mintodegree; //ddd.dddd format

	if(lonDir == 'W')
		longitude *= -1; //needs a - in front if west

	// get the speed
	GetNextToken(pString, temp, StringSize, 256, i);
	speed = atof(temp) * 1.15078; //convert knots to mph

//	delete [] temp;

	return true;
}
