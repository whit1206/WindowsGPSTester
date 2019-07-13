#include "LLParser.h"



bool LLParser::openFile(char* fileName, ifstream inS)
{
	inS.open(fileName, ios::nocreate); //opens file for reading

	if(inS.fail())
	{
		MessageBox(NULL, "Unable To Locate Coordinates File.", "ERROR", MB_OK);
		return false;
		
	}//end if

	return true;

}//end openFile

bool LLParser::getLLData(char* fileName, Sensor* &allSensors, int &length)
{ 
	ifstream inFile;
	if(!openFile(fileName, inFile))				//open the file
		return false;

	LLNode* cabinetIndex[MAX_SENSORS] = {NULL};	//init to NULL

	int   numCabinets = 0;

	char* buffer	= new char[MAX_CHARS];	//holds whole DAMN line! Biznatchio
	char* temp		= new char[MAX_CHARS];	//used for copy tokens


	inFile.getline(buffer, MAX_CHARS);			//ignore first line of description
	
	for(int i = 0; i < length && !inFile.eof(); i++)	//read till end of file or end of station array
	{
		LLNode newNode;

		inFile.getline(buffer, MAX_CHARS);			//read in whole line into buffer

		strcpy(temp, strtok(buffer, ","));			//get cabinet name, stick in temp
		newNode.cabinetName = new char[strlen(temp) + 1];	//deleted by deconstructor
		strcpy(newNode.cabinetName, temp);

		//set cabinet number (ex: ES-055 = 55)
		temp[0] = newNode.cabinetName[3];
		temp[1] = newNode.cabinetName[4];
		temp[2] = newNode.cabinetName[5];
		temp[3] = '\0';
		newNode.cabinetNum = atoi(temp);
		
		if(temp[0] >= '0' && temp[0] <= '9') //make sure valid sensor with a ### not a letter
		{
			strcpy(temp, strtok(NULL, ","));		//get sensor type
			newNode.type = new char[strlen(temp) + 1];
			strcpy(newNode.type, temp);

			strcpy(temp, strtok(NULL, ","));		//get source of data (ex: WSDOT,etc)
			newNode.source = new char[strlen(temp) + 1];
			strcpy(newNode.source, temp);

			strcpy(temp, strtok(NULL, ","));		//get latitude
			newNode.latitude = atof(temp);

			strcpy(temp, strtok(NULL, ","));		//get longitude
			newNode.longitude = atof(temp);
	
			//add newNode to cabinet array
			if(cabinetIndex[newNode.cabinetNum] == NULL && 
				newNode.cabinetName != NULL && newNode.type != NULL && strcmp(newNode.type, "linear") != 0 &&
				newNode.source != NULL && newNode.longitude != NULL && newNode.latitude != NULL)
			{
				cabinetIndex[newNode.cabinetNum] = new LLNode(newNode);
				
				newNode.cabinetName = NULL;			//set to null so deconstructor doesnt
				newNode.source = NULL;				//erase char strings which were shallow
				newNode.type = NULL;				//copied
			}
		}//end if
	}

	//now set stations latitude and longitude (in allStations array)
	for(i = 0; i < length; i++)
	{
		if(cabinetIndex[allSensors[i].getCabinetNum()] != NULL)
		{
			allSensors[i].setLatitude(cabinetIndex[allSensors[i].getCabinetNum()]->latitude);
			allSensors[i].setLongitude(cabinetIndex[allSensors[i].getCabinetNum()]->longitude);
		}
	}
	
	delete [] buffer;  //cleanup buffers
	delete [] temp;
	temp = NULL;
	buffer = NULL;
	inFile.close();		//close the file

	return true;		//everything worked, returned true (duh! Joe says)
}


/*
public Vector nodeList = new Vector();

	public void addStationNodes(String fileName)
	{
		FileReader fr = null;

		try
		{	
			fr = new FileReader(fileName);

			StringTokenizer st;
			String buffer = new String("");
			BufferedReader reader = new BufferedReader(fr);
		
			if(reader != null)
			{
				reader.readLine();	//ignore first line of description
				String latitude;
				String longitude;
				String cabinet;
				String type;
				String source;
				while((buffer = reader.readLine()) != null)	//read till end of file
				{
					st = new StringTokenizer(buffer, ",");
					if(st != null)
					{
						cabinet = st.nextToken();
						type = st.nextToken();
						source = st.nextToken();	
						latitude = st.nextToken();
						longitude = st.nextToken();
						if(cabinet != null && type != null && type.compareTo("'linear'") != 0 &&
							source != null && longitude != null && latitude != null)
						{
							Node newnode = new Node();
							newnode.cabinet = cabinet.substring(1, cabinet.length() - 1);
							newnode.type = type.substring(1, type.length() - 1);
							newnode.source = source.substring(1, source.length() - 1);
							newnode.longitude = Double.parseDouble(longitude);	//in format ddd.ddddd
							newnode.latitude = Double.parseDouble(latitude);

							boolean found = false;
							for(int i = 0; i < nodeList.size(); i++)
							{
								if(((Node) nodeList.get(i)).cabinet.compareTo(newnode.cabinet) == 0)
								{
									found = true;
									if(newnode.type == "UW ITS group")
										nodeList.set(i, newnode);
									break;
								}
							}

							if(found == false)
								nodeList.add(newnode);
						}
					}
				}
			}

		}
*/