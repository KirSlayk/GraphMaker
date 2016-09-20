#include "Windows.h"
#include "resource.h"
#include "calc.h"
#include <stdio.h>
#include <math.h>


void CoordinateAxis ( HWND graph, HDC hdc );

void CreateGraph ( HWND graph, HWND entergraph, HDC hdc );

//HWND button;
HWND graph;
HWND enterfunc;
int doDrawGraph = 0;
char* expr;
HWND widthwindow;
HWND heightwindow;
HWND HWNDstepEDIT;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM,LPARAM);

int __stdcall WinMain( HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR cmdLine, int showCmd )
{
	HWND button;
	HWND option;
	HWND func;
	HWND heightwindowstatic;
	HWND widthwindowstatic;
	HWND inscription;
	HWND stepstatic;
	HWND stepstaticexample;
	MSG msg;
	WNDCLASS wnd;
	const char* ClassName = "Plotter";
	

	memset(&wnd, 0, sizeof( wnd ));

	wnd.lpszClassName = ClassName;
	wnd.lpfnWndProc = WndProc;
	wnd.hInstance = currentInstance;
    wnd.hbrBackground = GetSysColorBrush( COLOR_WINDOW ); 
	wnd.hCursor = LoadCursor( NULL, IDC_ARROW ); 
	wnd.hIcon = LoadIcon (currentInstance, MAKEINTRESOURCE(IDI_ICON1) );
	
	RegisterClass( &wnd );
	
	option = CreateWindow(ClassName, "Ввод уравнения", WS_SYSMENU, 640, 30, 630, 240, 0, 0, currentInstance, 0);

	graph = CreateWindow(ClassName, "График", WS_SYSMENU | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 30,  30, 600, 600, option, (HMENU)0, currentInstance, 0);

	func = CreateWindow("STATIC", "Функция", WS_CHILD, 30, 30, 100, 30, option, (HMENU)0 , currentInstance, 0);

	enterfunc = CreateWindow("EDIT", 0,	WS_CHILD | WS_BORDER, 130, 30, 320, 30, option, (HMENU)0 , currentInstance, 0);

	button = CreateWindow("BUTTON", "Построить", WS_CHILD, 470, 30, 100, 30, option, (HMENU)IDC_BUTTON, currentInstance, 0);
	
	widthwindowstatic = CreateWindow("STATIC", "Интервал по OX", WS_CHILD, 10, 70, 150, 30, option, 0, currentInstance, 0);

	widthwindow = CreateWindow("EDIT", NULL, WS_CHILD | WS_BORDER, 170,  70, 120, 30, option, 0, currentInstance, 0);
	
	heightwindowstatic = CreateWindow("STATIC", "Интервал по OY", WS_CHILD, 10, 110, 150, 30, option, 0, currentInstance, 0);
	
	heightwindow = CreateWindow("EDIT",	NULL, WS_CHILD | WS_BORDER, 170, 110, 120, 30,	option,	0, currentInstance, 0);

	inscription = CreateWindow("STATIC", "[...;...] Вводится без пробелов   Пример: [10;20]", WS_CHILD, 320, 80, 250, 60, option, 0, currentInstance, 0);

	stepstatic = CreateWindow("STATIC", "Точность построения, изменение по OX", WS_CHILD, 10, 150, 150, 30, option, 0, currentInstance, 0);
	
	stepstaticexample = CreateWindow("STATIC", "0.001 — 1", WS_CHILD, 320, 150, 150, 30, option, 0, currentInstance, 0);

	HWNDstepEDIT = CreateWindow( "EDIT", 0,  WS_CHILD | WS_BORDER, 170, 150, 120, 30, option, 0, 0, 0);
		

	ShowWindow ( option, SW_SHOW );
	UpdateWindow( option );
		
	ShowWindow ( func, SW_SHOW );
	UpdateWindow ( func );

	ShowWindow ( widthwindow, SW_SHOW );
	UpdateWindow( widthwindow );

	ShowWindow ( widthwindowstatic, SW_SHOW );
	UpdateWindow( widthwindowstatic );

	ShowWindow ( heightwindow, SW_SHOW );
	UpdateWindow( heightwindow );

	ShowWindow ( heightwindowstatic, SW_SHOW );
	UpdateWindow( heightwindowstatic );
	
	ShowWindow ( enterfunc, SW_SHOW );
	UpdateWindow( enterfunc );
		
	ShowWindow ( button, SW_SHOW );
	UpdateWindow( button );
		
	ShowWindow ( graph, SW_SHOW );
	UpdateWindow ( graph );
		
	ShowWindow ( inscription, SW_SHOW );
	UpdateWindow( inscription );

	ShowWindow ( stepstatic, SW_SHOW );
	UpdateWindow( stepstatic );

	ShowWindow ( stepstaticexample, SW_SHOW );
	UpdateWindow ( stepstaticexample );

	ShowWindow ( HWNDstepEDIT, SW_SHOW );
	UpdateWindow ( HWNDstepEDIT );

	SetWindowText( HWNDstepEDIT, "0.001");
	
		
	CoordinateAxis( graph, NULL );
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg); 
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wparam,LPARAM lparam)
{
	if ( Message ==  WM_CTLCOLORSTATIC ){
		SetBkMode((HDC) wparam, TRANSPARENT);
        SetTextColor((HDC) wparam, RGB(0,0,0));
		return (INT_PTR)GetStockObject(NULL_BRUSH);
	}
	switch( Message ){
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	
	
	case WM_PAINT: if ( hwnd == graph ){
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(graph, &ps);
		
			CoordinateAxis( graph , hdc);
			if (doDrawGraph)
				CreateGraph(graph, enterfunc, hdc);
			EndPaint(graph, &ps);
		}
		break;
	
	case WM_DESTROY: PostQuitMessage(0); break;
	
	case WM_COMMAND: if(LOWORD(wparam)==IDC_BUTTON)
		{
			RECT rc;
			int length;
			doDrawGraph = 1;
			length = GetWindowTextLength ( enterfunc );
			free(expr);
			expr = (char*) calloc (length + 1, sizeof(char));
			GetWindowText ( enterfunc, expr, length + 1 );
			GetClientRect(graph, &rc );
			InvalidateRect(graph, &rc, 1);
		}
		break;
	}
	return DefWindowProc(hwnd,Message,wparam,lparam); 
}

void CoordinateAxis ( HWND graph, HDC hdc )
{
	HWND hwnd = graph;
	RECT rc;
	
	GetClientRect( hwnd, &rc );
	FillRect( hdc, &rc, (HBRUSH) RGB (0,0,0));
	
	MoveToEx(hdc, rc.left, (rc.bottom - 15), 0);
	LineTo( hdc, rc.right, (rc.bottom - 15));	
	MoveToEx(hdc, (rc.right - 26), rc.top, 0);
	LineTo(hdc, (rc.right - 26), rc.bottom);
}

void DrawGrid( RECT rc, HDC* hdc, int width, int height, int leftX, int rightX, int topY, int botY)
{
	double wndwidth = (rc.right - rc.left);
	double wndheight = (rc.bottom - rc.top);
	double kwidth = wndwidth / width;
	double kheight = wndheight / height;
	int i = 0;
	char string[8];
	double xcoord = rc.left;
	double ycoord = rc.bottom;
	int numofline = 0;
	int numofnum = 0;

	HPEN pensil = CreatePen( 0, 1, RGB( 175, 173, 163 ) );
	HGDIOBJ oldPensil = SelectObject( *hdc, pensil );

	
	
	if ( width >= height )
		numofline = numofnum = width / 10;
	else if ( height >= width )
		numofline = numofnum = height / 10;
	if ( !numofline )
		numofline = numofnum = 1;
	
	
	
	for (i = -width/2; i < width/2 + 1; i++){
		MoveToEx(*hdc, xcoord , rc.bottom , 0);
		if ( i % numofline == 0 )
			LineTo(*hdc, xcoord , rc.top);
		xcoord+=kwidth;
	}
				
	for (i = -height/2; i < height/2 + 1; i++){
		MoveToEx(*hdc, rc.left , ycoord, 0);
		if ( i % numofline == 0 )
			LineTo(*hdc, rc.right, ycoord);
		ycoord-=kheight;
	}

	xcoord = rc.left;
	ycoord = rc.bottom;
	
	pensil = CreatePen( 0, 2, RGB( 255, 0, 0 ) );
	oldPensil = SelectObject( *hdc, pensil );

	for (i = leftX; i < rightX + 1; i++){
		MoveToEx(*hdc, xcoord , rc.bottom - 11 , 0);
		if (i % numofnum == 0 && i != rightX){
			LineTo(*hdc, xcoord , rc.bottom - 19);
			sprintf(string, "%d", i);
			TextOut(*hdc, xcoord+2, rc.bottom - 14, string, strlen(string));
		}
		xcoord+=kwidth;
	}
				
	for (i = botY; i < topY + 1; i++){
		MoveToEx(*hdc, rc.right - 22 , ycoord, 0);
		if (i % numofnum == 0){
			LineTo(*hdc, rc.right - 30 , ycoord);
			sprintf(string, "%d", i);
			TextOut(*hdc, rc.right - 22, ycoord+3 , string, strlen(string));
		}	
		ycoord-=kheight;
	}			
}


void RangeSetting(int* width, int* height)
{
	if ( !*width )
		*width = 60;
	if ( !*height )
		*height = *width;
	if ((*width) & 1)
			*width+=1;
		
	if ((*height) & 1)
			*height+=1;
}


void IntervalOfGraph (char* widthwin, char* heightwin, int* leftX, int* rightX, int* botY, int* topY, char** error)
{
	int index;
	int negative = 0;
	
	for ( index = 0; widthwin[index]; index++ )
		if ( isalpha(widthwin[index]) )
			*error = "Entered interval data is not correct";
	
	for ( index = 0; heightwin[index]; index++ )
		if ( isalpha(heightwin[index]) )
			*error = "Entered interval data is not correct";

	if ( !*error ){
		for ( index = 0; widthwin[index]; index++ )
			if ( !isdigit(widthwin[index]) )
				if ( (index == 0 && widthwin[index] == '[') || widthwin[index] == ' ')
					continue;
				else if ( widthwin[index] == '-' && isdigit(widthwin[index + 1]) )
					negative = 1;
				else break;
			else if ( isdigit(widthwin[index]) ){
				char symbol = widthwin[index] - '0';
				if ( *leftX )
					*leftX*=10;
				*leftX+= symbol; 
			}
	
		if ( negative )
			*leftX*=-1;
		negative = 0;

	
	
		while (widthwin[index] == ' ')
			index++;
	
		if ( widthwin[index] == ';' )
			for ( index+=1; widthwin[index]; index++ )
				if ( widthwin[index] == '-' && isdigit(widthwin[index + 1]) )
					negative = 1;
				else if ( !isdigit(widthwin[index]) && widthwin[index] != ' ' ) break;
				else if ( widthwin[index] != ' ' ){
					char symbol = widthwin[index] - '0';
					if ( *rightX )
						*rightX*=10;
					*rightX+= symbol; 
				}
	
			
		if ( negative )
			*rightX*=-1;

		if ( !*rightX && !*leftX )
		SetWindowText( widthwindow, "[-30;30]" );
		
		if ( !*rightX && !*leftX ){
			*rightX = 30;
			*leftX = -30;
		}
	
	
		if ( *rightX < *leftX ){
			MessageBox(graph, "Верхняя граница X меньше чем нижняя. График не будет построен","ERROR", MB_OK);
			*error = "Bad interval";
		}
	
	
		for ( index = 0; heightwin[index]; index++ )
			if ( !isdigit(heightwin[index]) )
				if ( index == 0 && heightwin[index] == '[' || widthwin[index] == ' ')
					continue;
				else if ( heightwin[index] == '-' && isdigit(heightwin[index + 1]) )
					negative = 1;
				else break;
			else if ( isdigit(heightwin[index]) ){
				char symbol = heightwin[index] - '0';
				if ( *botY )
					*botY*=10;
				*botY+= symbol; 
			}
		if ( negative )
			*botY*=-1;
		negative = 0;
	
	
		while (heightwin[index] == ' ')
			index++;
	
	
		if ( heightwin[index] == ';' )
			for ( index+=1; heightwin[index]; index++ )
				if ( heightwin[index] == '-' && isdigit(heightwin[index + 1]) )
					negative = 1;
				else if ( !isdigit(heightwin[index]) && heightwin[index] != ' ') break;
				else if ( heightwin[index] != ' ' ){
					char symbol = heightwin[index] - '0';
					if ( *topY )
						*topY*=10;
					*topY+= symbol; 
				}
	
		if ( negative )
			*topY*=-1;
	
		if ( !*topY && !*botY )
			SetWindowText( heightwindow, "[-30;30]" );
		
		
		if ( !*topY && !*botY ){
			*topY = 30;
			*botY = -30;
		}
	
		if ( *topY < *botY ){
			MessageBox(graph, "Верхняя граница Y меньше чем нижняя. График не будет построен","ERROR", MB_OK);
			*error = "Bad interval";
		}
	}
}



void CreateGraph ( HWND graph, HWND enterfunc, HDC hdc )
{
	HWND hwndGraph = graph;
	HWND hwndEnterfunc = enterfunc;
	int length;
	int index;
	int leftX = 0;
	int rightX = 0;
	int topY = 0;
	int botY = 0;
	char* widthwin;
	char* heightwin;
	char* error = 0;
	RECT rc;
	
	
	length = GetWindowTextLength ( widthwindow );
	widthwin = (char*) calloc (length+1, sizeof(char));
	GetWindowText ( widthwindow, widthwin, length + 1 );
	
	length = GetWindowTextLength ( heightwindow );
	heightwin = (char*) calloc (length+1, sizeof(char));
	GetWindowText ( heightwindow, heightwin, length + 1 );

	
	IntervalOfGraph ( widthwin, heightwin, &leftX, &rightX, &botY, &topY, &error );
	
	GetClientRect( graph, &rc );

	if ( expr && *expr && !error )
	{
		double x;
		int width = abs( rightX - leftX );
		int height = abs( topY - botY );
		double step = 0.001;
		char* stepedit;
		
		int firstTime = 1;
		int index = 0;
		int point = 0;
		int discharge = 0;
		
		length = GetWindowTextLength ( HWNDstepEDIT );
		stepedit = (char*) calloc (length+1, sizeof(char));
		GetWindowText ( HWNDstepEDIT, stepedit, length + 1 );
		
		
		RangeSetting( &width, &height );

		DrawGrid(rc, &hdc, width, height, leftX, rightX, topY, botY );
		
		
		if ( length ){
			while ( stepedit[index] )
				if ( !point && stepedit[index] == '.' || stepedit[index] == ',' ){
					point = 1;
					discharge = index;
					index++;
				}
				else if ( !isdigit(stepedit[index++])){
					error ="Data entry error";
					break;
				}
		
			step = atof( stepedit );
			if ( point && !error ){
				discharge = 1 / 10^(index - discharge);
				step = step *  discharge;
			}
			if ( (!step || step < 0.001) && !error  ){	
				error = "Data entry error";
				}
		}																
		
		
		if ( step == 0.001 )
			SetWindowText( HWNDstepEDIT, "0.001" );
		
		if ( !error )
			for (x = leftX; x < rightX; x+=step) {
				double wndwidth = (rc.right - rc.left);
				double wndheight = (rc.bottom - rc.top);
				double kwidth = wndwidth / width;
				double kheight = wndheight / height;
				int magicparam = 0;
				char* falsherror = 0;
				
				static double leftBordersOfX = 0;
			
			
				double y = calculate(expr, &error, x);

				if ( leftX & 1 && !(rightX & 1) )
					magicparam = 1;
				if ( rightX & 1 && !(leftX & 1) )
					magicparam = 1;

				
				if (error && 
					error != "Error evaluating TG. Argument == PI/2 + PI*n" && 
					error != "Error evaluating CTG. Argument == PI + PI*n" && 
					error != "Division by zero" && 
					error != "Under the root of a negative number" && 
					error != "The negative logarithm is not extracted")
					break;
				
				
				if ( abs( y ) > height + 7 )
					error = "Not draw";
				
				if ( !error )
					if (firstTime ) 
						MoveToEx(hdc, (double) (rc.right + (x - rightX - magicparam) * kwidth), (double) (rc.bottom - (y - botY)*kheight), 0);
					else if ( !error ) LineTo(hdc, (double) (rc.right + (x - rightX - magicparam) * kwidth), (double) (rc.bottom - (y - botY)*kheight));
					

				firstTime = 0;
				if (error)
					firstTime = 1;
				error = NULL;
			}
		}
		if ( error )
			MessageBox( graph, error, "ERROR", 0 );
		free(widthwin);
		free(heightwin);
	
	}

