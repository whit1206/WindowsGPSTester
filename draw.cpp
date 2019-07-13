/* draw.cpp
 *
 * The draw class handles all drawing tasks. These include drawing the map,
 * drawing the current location, drawing destination, drawing route, and 
 * drawing current traffic conditions.
 * 
 * 08.04.02		Casey Margell	Initial Coding
 */


#include "draw.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>

const int MAX_ROUTE_NODES = 256;
const int WIDTH = 850;
const int HEIGHT = 700;

Draw::Draw() : x1(0), y1(0), x2(0), y2(0)
{
}

Draw::~Draw()
{

}



Draw::Draw(HWND hWnd, HINSTANCE hinstance)
{
	hwnd = hWnd;
	hbmpSeattleMap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BIGSEATTLEMAP));
	hInstance = hinstance;

	y2 = 47.84;
	y1 = 47.45;
	x1 = -122.461785714286;
	x2 = -121.988214285714;
}

void Draw::DrawGraph(HDC hdc, Graph & g)
{
	GraphNode * gn;
	for(int i = 0; i < g.getNodeCount(); i++)
	{
		gn = g.GetNodeAt(i);
		
		Sensor * s = (Sensor *) gn->sensorList.GetNodeAt(0)->GetObj();
	
		int y = GetYCoord(s->getLatitude());
		int x = GetXCoord(s->getLongitude());
		
		Ellipse(hdc, x-4, y-4, x+4, y+4);
	}

}

void Draw::SetHwnd(HWND hWnd)
{
	hwnd = hWnd;
}

void Draw::SetHInstance(HINSTANCE hinstance)
{
	hInstance = hinstance;
}

void Draw::SetInternals(HWND hWnd, HINSTANCE hinstance)
{
	SetHInstance(hinstance);
	SetHwnd(hWnd);
	y2 = 47.84;
	y1 = 47.45;
	x1 = -122.461785714286;
	x2 = -121.988214285714;
	hbmpSeattleMap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BIGSEATTLEMAP));
}

void DrawDblString(HDC hdc, char * buf, double data, int x, int y)
{
	RECT rect;
	rect.top = y;	
	rect.left = x;
	rect.bottom = y+50;
	rect.right = x+1000;

	char temp2[32];

	_gcvt(data, 8, temp2);
	strcat(buf, temp2);


	SetTextColor(hdc, RGB(255, 0, 0));
	DrawText(hdc, buf, -1, &rect,DT_LEFT);
}

void Draw::DrawMap(GPS & gps, Graph & graph)
{
 // Calling BeginPaint clears the update region that was set by calls
  // to InvalidateRect(). Once the update region is cleared no more
  // WM_PAINT messages will be sent to the window until InvalidateRect
  // is called again.
  PAINTSTRUCT ps;
  hdc = BeginPaint(hwnd,&ps);
  
  // To paint with a DDB it first needs to be associated
  // with a memory device context. We make a DC that
  // is compatible with the screen by passing NULL to
  // CreateCompatibleDC.
  // Then we need to associate our saved bitmap with the
  // device context.
  
  HDC hdcMem = CreateCompatibleDC(NULL);
  HBITMAP hbmT = SelectBitmap(hdcMem,hbmpSeattleMap);
  
  // Now, the BitBlt function is used to transfer the contents of the 
  // drawing surface from one DC to another. Before we can paint the
  // bitmap however we need to know how big it is. We call the GDI
  // function GetObject to get the relevent details.
  BITMAP bm;
  GetObject(hbmpSeattleMap,sizeof(bm),&bm);
  
  BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);


	char temp[256] = "GPS Time: ";	
	RECT rect;
	rect.top = 10;	
	rect.left = WIDTH + 10;
	rect.bottom = 250;
	rect.right = WIDTH + 200;
	//delete old stuff
	FillRect(hdc, &rect, (HBRUSH) (COLOR_WINDOW+1));


	if(gps.GotTimeYet())
	{
		DrawDblString(hdc, temp, (double) gps.GetTimeInt(), WIDTH + 10, 10);
		
		if(gps.IsPositionValid())
		{
			strcpy(temp, "Altitude (ft): ");
			DrawDblString(hdc, temp, gps.GetAltitudeDouble(), WIDTH + 10, 30);

			strcpy(temp, "Longitude: ");
			DrawDblString(hdc, temp, gps.GetLongitudeDouble(), WIDTH + 10, 50);

			strcpy(temp, "Latitude: ");
			DrawDblString(hdc, temp, gps.GetLatitudeDouble(), WIDTH + 10, 70);

			strcpy(temp, "Speed (mph): ");
			DrawDblString(hdc, temp, gps.GetSpeedDouble(), WIDTH + 10, 90);
		}
		else
		{
			rect.top = 50;	
			rect.left = WIDTH + 10;
			rect.bottom = 100;
			rect.right = WIDTH + 200;

			SetTextColor(hdc, RGB(255, 0, 0));
			DrawText(hdc, "Obtaining Position...", -1, &rect,DT_LEFT);
		}
	}
	else
	{
		rect.top = 20;	
		rect.left = WIDTH + 10;
		rect.bottom = 20+50;
		rect.right = WIDTH + 200;

		SetTextColor(hdc, RGB(255, 0, 0));
		DrawText(hdc, "Waiting for GPS...", -1, &rect,DT_LEFT);
	}


	// Now, clean up. A memory DC always has a drawing
	// surface in it. It is created with a 1X1 monochrome
	// bitmap that we saved earlier, and need to put back
	// before we destroy it.
	SelectBitmap(hdcMem,hbmT);
	DeleteDC(hdcMem);


	if(gps.IsPositionValid())
	{
		static bool wasGreen = true;

		HBRUSH hbrush = CreateSolidBrush((wasGreen?RGB(255,0,0):RGB(0, 255, 0)));
		wasGreen = !wasGreen;

		// Select the new pen and brush, and then draw.
		//SelectObject(hdc, hpen);
		SelectObject(hdc, hbrush);

		int x = GetXCoord(gps.GetLongitudeDouble());
		int y = GetYCoord(gps.GetLatitudeDouble());

		Ellipse(hdc, x-7, y-7, x+7, y+7);
	}

	DrawGraph(hdc, graph);	//draw graph sensors
	
	// EndPaint balances off the BeginPaint call.
	EndPaint(hwnd,&ps);
}

void Draw::DrawLocation(double latitude, double longitude)
{
	// draw some token at (GetYCoord(latitude), GetXCoord(longitude))
}

void Draw::DrawDestination(double latitude, double longitude)
{
	// draw some token at (GetYCoord(latitude), GetXCoord(longitude))
}

void Draw::DrawRoute(Graph &g, Route &r)
{
	int routeSize = 0;
	double coordArrayX[MAX_ROUTE_NODES];
	double coordArrayY[MAX_ROUTE_NODES];


	double travelTime = r.GetPathCoordinates(g, coordArrayY, coordArrayX, MAX_ROUTE_NODES, routeSize);
	if(routeSize == 0)
		return;

	POINT *pArray = new POINT[routeSize];
	for(int i = 0; i < routeSize; i++) {
		pArray[i].x = GetXCoord(coordArrayX[i]);
		pArray[i].y = GetYCoord(coordArrayY[i]);
	}
	Polyline(hdc, pArray, routeSize);
	delete [] pArray;
}


int Draw::GetYCoord(double latitude)
{
	if(latitude <= y2 && latitude >= y1)
		return (int) ((1 - (latitude - y1) / (y2 - y1)) * (double) HEIGHT);
	else
		return -1;	
}

int Draw::GetXCoord(double longitude)
{
	if(longitude <= x2 && longitude >= x1)
		return (int) (((longitude - x1) / (x2 - x1)) * (double) WIDTH);
	else
		return -1;
}