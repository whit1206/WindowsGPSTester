//GPS.h - class to hold gps data and communicate with gps
//         Note: uses multiple threads

#ifndef GPS_H
#define GPS_H

//#include <windows.h>
//#include "Serial.h"
#include "List.h"

const int THREAD_STORE_BYTES = 256;	//listener threasd gets 256 bytes of buffer to fill

class GPSThreadInfo	//structure used to give thread info
{
	public:
		List * pGPSCommandStringList;
		int comport;
};


//connects to the gps and maintains an up to date data of gps info
//uses a seperate thread to communicate with gps (uses NMEA 0183 ver 2.0 standard)
class GPS
{
	public:
		GPS();		//constructor, comport is the # of the comport the GPS is connected to
		~GPS();					//cleans up memory

		bool	ConnectToComPort(int comport);		//returns true if connected, else false=not connected
										//tries to create thread that will talk to gps on comm port

		bool	DisconnectFromComPort();

		bool	IsConnected() const;	//returns true if connected to gps (time is coming in)

		bool	IsPositionValid() const;		//returns true if altitide/long/lat is valid

		double	GetLongitudeDouble() const;		//return position information
		double	GetLatitudeDouble() const;
		double	GetAltitudeDouble() const;
		double	GetSpeedDouble() const;  //in mph
		int		GetTimeInt() const;

		bool	GotTimeYet() const;

		void	UpdateGPSData();	//causes GPS to parse strings in Command List

	private:
		double	longitude;			//in +/- ddd.ddddd
		double	latitude;			//in +/- ddd.ddddd
		double	altitude;			//in ft
		int		time;				//HHMMSS (UNC time), Seattle = UNC - 7 hours
		double	speed;				// In mph
		bool	PositionValid;		//is the lat/long/alt data valid (true= yes)

		bool	GotTime;			//forst part of connection

		int		port;

		ULONG	ThreadID;
		HANDLE	ThreadHandle;
		HANDLE	hMutex;			//for controlling access to list
		List *	pGPSCommandStringList;	//list of command strings populated by thread

		bool	ParseGPSData(char *pData, int DataSize);

		bool	parseGPGGA(char *pString, int StringSize);
		bool	parseGPGLL(char *pString, int StringSize);
		bool	parseGPGSA(char *pString, int StringSize);
		bool	parseGPGSV(char *pString, int StringSize);
		bool	parseGPRMC(char *pString, int StringSize);
		bool	CompareType(char *pData, char *pCode);
		void    GetNextToken(char* pSrc, char* pDest, int sourceSize, int destSize, int &i);


};

#endif //GPS_H