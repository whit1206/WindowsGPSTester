/* draw.h
 *
 * The draw class handles all drawing tasks. These include drawing the map,
 * drawing the current location, drawing destination, drawing route, and 
 * drawing current traffic conditions.
 * 
 * 08.04.02		Casey Margell	Initial Coding
 */

#ifndef __draw_h__
#define __draw_h__

#include <windows.h>
#include "Graph.h"
#include "Route.h"
#include "gps.h"


class Draw
{
public:
	Draw();
	Draw(HWND hWnd, HINSTANCE hinstance);
	~Draw();
	void SetHwnd(HWND hWnd);
	void SetHInstance(HINSTANCE hinstance);
	void SetInternals(HWND hWnd, HINSTANCE hinstance);
	void DrawMap(GPS & gps, Graph & graph);
	void DrawLocation(double latitude, double longitude);
	void DrawDestination(double latitude, double longitude);
	void DrawRoute(Graph &g, Route &r);
	void DrawTrafficConditions(/*Graph &g*/);
	int GetXCoord(double longitude);
	int GetYCoord(double latitude);

private:
	void DrawGraph(HDC hdc, Graph & g);

	HWND hwnd;
	HDC hdc;
	HBITMAP hbmpSeattleMap;		// bitmap handle for seattle map
	HINSTANCE hInstance;

	double x1,y1,x2,y2;
};




#endif