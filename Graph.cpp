#include "Graph.h"
#include "Sensor.h"
#include "STAParser.h"
#include "DATParser.h"
#include "LLParser.h"

////////////////////////////////////// Graph members ////////////////////////////////////////
Graph::Graph() : numNodes(0)		//Constructor - inits all vars to empty/not used
{
	for(int i = 0; i < MAX_NODES; i++) //set array to empty pointers
		gnArray[i] = NULL;
}


Graph::~Graph()		//Deconstructor - cleans up memory
{
	Clear();
}

bool Graph::BuildGraph()
{
	// check for internet connection
	// if a connection is present then download the file containing graph data
	/*long byteCount = GetHTTP("webflow.wa.gov", "/index.html", "C:\\WINDOWS\\Desktop\\Point B Telematics Stuff\\prototype\\index.html");
	 
	if(byteCount <= 0) 
		MessageBox(NULL, "Error Downloading Files...", "Error", MB_OK);
	else
		MessageBox(NULL, "Grabbed File Successfully.", "Success", MB_OK);

	*/

	Sensor* allSensors = NULL; //instantiate temporary array of sensors
	int length = 0;				   //number of sensors in array

	if(!STAParser::getSensorData("C:\\webflow.sta", allSensors, length))
		return false;
	if(!DATParser::getSensorData("C:\\webflow.dat", allSensors, length))
		return false;
	if(!LLParser::getLLData("C:\\latlong.txt", allSensors, length))
		return false;

	if(length == 0)
		return false;

	char* lastCabinet = allSensors[0].getCabinetName();
	GraphNode* gn = AddNode(&allSensors[0]);	//add cabinet plus first sensor

	//NOTE: this routine assumes that all sensors are sorted by 
	//		cabinet name alphanumerically...(in the STA file and sensorArray)

	for(int i = 0; i < length; i++)
	{
		if(strcmp(lastCabinet, allSensors[i].getCabinetName()) == 0)
		{
			gn->addSensor(&allSensors[i]);	//add sensor to cabinet
		}
		else
		{
			gn = AddNode(&allSensors[i]);	//add new node and sensor, since not in list before
			lastCabinet = allSensors[i].getCabinetName();
		}
	}

	return true;
	
}

long Graph::GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName, LPCSTR lpOutFileName)
{
	WORD wVersionRequested = MAKEWORD(1,1); //use winsock 1.1 (old and well supported)
	WSADATA wsaData;
	int nRet;
	
	// Initialize WinSock.dll
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		//fprintf(stderr,"\nWSAStartup(): %d\n", nRet);
		WSACleanup();
		return -1;
	}
	
	// Check WinSock version
	if (wsaData.wVersion != wVersionRequested)
	{
		//fprintf(stderr,"\nWinSock version not supported\n");
		WSACleanup();
		return -1;
	}
	
	// Use inet_addr() to determine if we're dealing with a name
	// or an address
	IN_ADDR		iaHost;
	LPHOSTENT	lpHostEntry;

	iaHost.s_addr = inet_addr(lpServerName);
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(lpServerName);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost, 
						sizeof(struct in_addr), AF_INET);
	}
	if (lpHostEntry == NULL)
	{
		WSACleanup();
		return -1;
	}


	//	
	// Create a TCP/IP stream socket
	//
	SOCKET	Socket;	

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		WSACleanup();
		return -1;
	}

	//
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;

	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;


	// Fill in the rest of the server address structure
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

	// Connect the socket
	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		closesocket(Socket);
		WSACleanup();
		return -1;
	}

	// Format the HTTP request
	char szBuffer[1024];
	long totalsize = 0;
	sprintf(szBuffer, "GET %s\n", lpFileName);

	//send server request for file 
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		closesocket(Socket);	
		WSACleanup();
		return -1;
	}

	
	FILE *f1;
	
	if((f1 = fopen(lpOutFileName, "w+b")) != NULL) //make sure file opened
	{
		while(1)
		{
			// Wait to receive, nRet = NumberOfBytesReceived
			nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
			if (nRet == SOCKET_ERROR)
			{
				//PRINTERROR("recv()");
				break;
			}
			totalsize += nRet;

			// Did the server close the connection?
			if (nRet == 0)
				break;
			// Write to stdout
			fwrite(szBuffer, nRet, 1, f1);
		}

		fclose(f1);
	}
	else
		MessageBox(NULL, "Error: Output file path is invalid!", "Bad output path", MB_OK);

	closesocket(Socket);

	WSACleanup();
	return totalsize;
}




//////////////////////////////////// GraphNode //////////////////////////////




GraphNode * Graph::AddNode(Sensor * s) //add a node and return its pointer, sensor is NOT deep copied
{
	if(s != NULL && numNodes < MAX_NODES) //if a possible address and within # of doable nodes...
	{
		GraphNode * newGN = new GraphNode();
		if(newGN->addSensor(s))
		{					//it worked so add node to list,increment count, and return its new node address
			gnArray[numNodes++] = newGN;
			return newGN;
		}
		else
			delete newGN;	//didnt work so delete new sensor
	}
	
	return NULL;		//if got here, was bad so return bad address (NULL)
}


GraphNode * Graph::GetNodeAt(int pos)	//gets node pointer from position in array
{
	if(pos < 0 || pos >= MAX_NODES)	//make sure within range of max nodes, if not return 0
		return NULL;
	
	return gnArray[pos];	//return node ptr at position
}


void Graph::Clear()				//clear out all nodes
{
	for(int i = 0; i < MAX_NODES; i++)		//if specific node exists...
		if(gnArray[i] != NULL)
		{
			delete gnArray[i];				//delete and set slot to empty
			gnArray[i] = NULL;
		}

	numNodes = 0;
}

int Graph::getNodeCount() const
{
	return numNodes;
}


////////////////////////////////////// GraphNode members /////////////////////////////////////
GraphNode::GraphNode()		//Constructor - inits all vars to empty/not used
{
	graphArrayPosition = -1;
}


GraphNode::~GraphNode()		//Deconstructor - cleans up memory
{
}


bool GraphNode::addSensor(Sensor * s)
{
	return sensorList.AddItemToStart(s);
}


