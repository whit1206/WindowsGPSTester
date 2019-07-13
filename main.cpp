#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "resource.h"
#include "gps.h"
#include "draw.h"

// Global objects for project
GPS		gps;	//global GPS object for data access and communication to GPS
Draw	gDraw;
Graph	gGraph;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
long				GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName, 
							LPCSTR lpOutFileName);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "PointBProto";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;



	hWnd = CreateWindow("PointBProto", "PointBProto", WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}


	SetWindowPos(hWnd, NULL, 0,0, 1024, 735, SWP_SHOWWINDOW);

	gDraw.SetInternals(hWnd, hInstance);

	// check for GPS connection
	if(gps.ConnectToComPort(1) == true)		//it worked, so wait for locating us
	{

	}
	else		//didnt work, have them pick source
	{
		MessageBox(hWnd, "Error", "GPS connection not detected. Please manually select your location...", MB_OK);
	}

	return TRUE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;

	switch (message) 
	{
		case WM_CREATE:
			SetTimer(hWnd, 1, 1000, NULL);
			gGraph.BuildGraph();
			break;
		case WM_TIMER:
			gps.UpdateGPSData();
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_PAINT:
			gDraw.DrawMap(gps, gGraph);
			break;
		//case WM_COMMAND:
		//	wmId    = LOWORD(wParam); 
		//	wmEvent = HIWORD(wParam); 
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}
