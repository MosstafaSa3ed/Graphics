#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <stack>
#include <vector>
#include <iostream>
#include "menu_header.h"
using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");


bool flag1 = false, flag2 = false, flag3 = false,flag4=false;
int xs, ys, xe, ye, xm, ym,x4,y4;
int menuItemsId;
bool drawCircleMidPoint = false,drawLineMidPoint = false,drawCircle = false,drawLine =false,fill_flood=false,cohen = false,
 hermit=false,bezir=false,polar=false,direct,save=false,load=false;


struct line
{
    int xs,ys,xe,ye;
};
vector<line> lines;

COLORREF screen[768][1366];

void Save(HDC hdc)
{
    for(int i=0;i<768;i++)
        for(int j=0;j<1366;j++)
            screen[i][j]=GetPixel(hdc,i,j);
}

void Load(HDC hdc)
{
    for(int i=0;i<768;i++)
        for(int j=0;j<1366;j++)
            SetPixel(hdc,i,j,screen[i][j]);
}

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

							 /* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(MY_MENU_ID);    /* There is  a menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
											   /* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		_T("Code::Blocks Template Windows App"),       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, nCmdShow);

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}

void swap(int& xs, int& ys, int& xe, int& ye)
{
	int temp = xs;
	xs = xe;
	xe = temp;
	temp = ys;
	ys = ye;
	ye = temp;
}
int roundd(double num)
{
	return (int)(num + 0.5);
}

void DrawLine_(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
    int dy = ye - ys;
	int dx = xe - xs;

    if (dy < dx)
	{
		if (xs>xe)
			swap(xs, ys, xe, ye);
		for (int x = xs; x<= xe; x++)
		{
			int y = roundd(ys + ((x - xs)*(double)(ye-ys)/(xe-xs)));
			SetPixel(hdc, x, y, color);
		}
	}
	else
    {
        if(ys>ye)
        swap(xs, ys, xe, ye);
        for (int y= ys; y<ye; y++)
		{
			int x = roundd(xs + ((y - ys)*(double)(xe-xs)/(ye-ys)));
			SetPixel(hdc, x, y, color);
		}
    }
}


void bezierCurve(HDC hdc, double x1, double y1, double x2, double y2,double x3,double y3,double x4,double y4, COLORREF color)
{

    for(double t=0;t<=1;t+=0.001)
    {
        double a=(1-t);
        double x=(x1*a*a*a)+(x2*3*t*a*a)+(x3*3*t*t*a)+(x4*t*t*t);
        double y=(y1*a*a*a)+(y2*3*t*a*a)+(y3*3*t*t*a)+(y4*t*t*t);
        SetPixel(hdc,roundd(x),roundd(y),color);
    }
}

void hermitCurve(HDC hdc, int x1, int y1, int x2, int y2,int x3,int y3,int x4,int y4, COLORREF color)
{
    double a0=x1,a1=x3,a2,a3,b0=y1,b1=y3,b2,b3;
    a2=(-3*x1)+(3*x2)-(2*x3)-x4;
    a3=(2*x1)-(2*x2)+x3+x4;
    b2=(-3*y1)+(3*y2)-(2*y3)-y4;
    b3=(2*y1)-(2*y2)+y3+y4;
    for(double t=0;t<=1;t+=0.001)
    {
        double x=a0+(a1*t)+(a2*t*t)+(a3*t*t*t);
        double y=b0+(b1*t)+(b2*t*t)+(b3*t*t*t);
        SetPixel(hdc,roundd(x),roundd(y),color);
    }
}

void DrawLine(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
	int dy = ye - ys;
	int dx = xe - xs;
	if (dy == 0 && dx == 0)
	{
		SetPixel(hdc, xs, ys, color);
		return;
	}
	if (dx == 0)
	{
		for (int y = ys; y<ye; y++)
		{
			SetPixel(hdc, xs, y, color);
		}
		return;
	}

	if (dy < dx)
	{
		if (xs>xe)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dy) / dx;
		for (int x1 = xs; x1 <= xe; x1++)
		{
			int y = roundd(ys + ((x1 - xs)*m));
			SetPixel(hdc, x1, y, color);
		}
	}
	else {
		if (ys>ye)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		double m = double(dx) / dy;
		for (int y1 = ys; y1<ye; y1++)
		{
			int x = roundd(xs + ((y1 - ys)*m));
			SetPixel(hdc, x, y1, color);
		}
	}
}
void DrawLineMidPoint(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
    line l;
    l.xs = xs;
    l.ys = ys;
    l.xe = xe;
    l.ye = ye;
    lines.push_back(l);

	int dx = xe - xs;
	int dy = ye - ys;
	int d, ch1, ch2;

	if (dy == 0 && dx == 0)
	{
		SetPixel(hdc, xs, ys, color);
		return;
	}

	if( abs(dy) < abs(dx) && dx != 0)
	{
		if (xs>xe)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		int x = xs, y = ys;
		SetPixel(hdc, x, y, color);

		if (dy > 0) //f(x+1,y+0.5)
		{
			d = dx - 2 * dy;
			ch1 = -2 * dy;
			ch2 = 2 * (dx - dy);
		}
		else //f(x-1,y+0.5)
		{
			d = dx + 2 * dy;
			ch1 = 2 * dy;
			ch2 = 2 * (dx + dy);
		}

		while (x < xe)
		{
			if (d > 0)
			{
				d = d + ch1;
				x++;
			}
			else
			{
				d = d + ch2;
				x++;

				if (dy > 0) //the line is /.
					y++;
				else    //the line is \.
					y--;
			}
			SetPixel(hdc, x, y, color);
		}
	}
	else
	{
		if (ys>ye)
		{
			swap(xs, ys, xe, ye);
			dy *= -1;
			dx *= -1;
		}
		int x = xs, y = ys;
		SetPixel(hdc, x, y, color);

		if (dx > 0)		//f(x+0.5,y+1)
		{
			d = dy - 2 * dx;
			ch1 = -2 * dx;
			ch2 = 2 * (dy - dx);
		}
		else //f(x-0.5,y+1)
		{
			d = dy + 2 * dx;
			ch1 = 2 * dx;
			ch2 = 2 * (dy + dx);
		}

		while (y < ye)
		{
			if (d > 0)
			{
				d = d + ch1;
				y++;
			}
			else
			{
				d = d + ch2;
				y++;
				if (dx > 0) //the line is /.
					x++;
				else	//the line is \.
					x--;
			}
			SetPixel(hdc, x, y, color);
		}
	}
}

void DrawParabolaCurve(HDC hdc, int xs, int ys, int xm, int ym, int xe, int ye, COLORREF color)
{
	for (double t = 0; t < 1; t += 0.001)
	{
		double x = xs + (t*double(4 * xm - 3 * xs - xe)) + (t*t*double(2 * xe + 2 * xs - 4 * xm));
		double y = ys + (t*double(4 * ym - 3 * ys - ye)) + (t*t*double(2 * ye + 2 * ys - 4 * ym));
		//cout<<x<<" "<<y<<endl;
		SetPixel(hdc, roundd(x), roundd(y), color);
	}

}

void Draw8points(HDC hdc, int xc, int yc, int x, int y, COLORREF color)
{
	SetPixel(hdc, xc + x, yc + y, color);
	SetPixel(hdc, xc + x, yc - y, color);
	SetPixel(hdc, xc - x, yc + y, color);
	SetPixel(hdc, xc - x, yc - y, color);
	SetPixel(hdc, xc + y, yc + x, color);
	SetPixel(hdc, xc + y, yc - x, color);
	SetPixel(hdc, xc - y, yc + x, color);
	SetPixel(hdc, xc - y, yc - x, color);
}

 void DrawPolarCircle(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
    double r = sqrt(((xe - xc)*(xe - xc)) + ((ye - yc)*(ye - yc)));
	double x = r;
	double y = 0;
	Draw8points(hdc, x, y,xc,yc , color);
	double dTheta= 1.0/(double)r;

	double c= cos(dTheta);
	double s= sin(dTheta);

	while(x>y)
    {
        double x1= (x*c) - (y*s);
        y=(x*s) + (y*c);
        x=x1;
        Draw8points(hdc,xc, yc, x, y,  color);
    }
}

void DrawCircle(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
	double r = sqrt(((xe - xs)*(xe - xs)) + ((ye - ys)*(ye - ys)));
	int x = 0;
	int y = r;
	Draw8points(hdc, xc, yc, x, y, color);
	while (x < y)
	{
		x++;
		y = roundd(sqrt(r*r - x*x));
		Draw8points(hdc, xc, yc, x, y, color);
	}
}
void DrawCircleMidPoint(HDC hdc, int xc, int yc, int xe, int ye, COLORREF color)
{
	double r = sqrt(((xe - xs)*(xe - xs)) + ((ye - ys)*(ye - ys)));
	int x = 0;
	double y = r;
	Draw8points(hdc, xc, yc, x, roundd(y), color);

	double d = 1 - r;
	while (x < y)
	{
		if (d < 0)
			d += (2 * x + 3);
		else
		{
			d += (2 * (x - y) + 5);
			y--;
		}
		x++;
		Draw8points(hdc, xc, yc, x, roundd(y), color);

	}


}

void MyFloodFill(HDC hdc,int x,int y, COLORREF borderColor,COLORREF fillColor)
{
    stack<int> xp;
    stack<int> yp;
    xp.push(x);
    yp.push(y);

    while(!xp.empty())
    {

        int xtop = xp.top();
        xp.pop();
        int ytop = yp.top();
        yp.pop();
        COLORREF color = GetPixel(hdc,xtop,ytop);
        if(color != borderColor && color != fillColor)
        {
            SetPixel(hdc,xtop,ytop,RGB(0, 255, 0));
            xp.push(xtop+1);
            yp.push(ytop);

            xp.push(xtop);
            yp.push(ytop+1);

            xp.push(xtop-1);
            yp.push(ytop);

            xp.push(xtop);
            yp.push(ytop-1);
        }
    }

}

union Outcode
    {
        unsigned All:4;
        struct{
            unsigned left:1,right:1,bottom:1,top:1;
        };
    };

Outcode GetOutcode (float x,float y,int xleft,int xright,int ybottom,int ytop)
{
    Outcode r;          r.All = 0;
    if(x < xleft)       r.left = 1;
    else if(x > xright) r.right = 1;
    if(y < ybottom)     r.bottom = 1;
    else if(y > ytop)   r.top = 1;
    return r;
}
void VIntersect(int xedge,double xs,double ys,double xe,double ye,double &xin,double &yin)
{
    xin = xedge;
    yin = ys + (xedge - xs)*(ye - ys)/(xe - xs);
}
void HIntersect(int yedge,double xs,double ys,double xe,double ye,double &xin,double &yin)
{
    yin = yedge;
    xin = xs + (yedge - ys)*(xe - xs)/(ye - ys);
}
void CohenSuth(double xs,double ys,double xe,double ye,int xleft,int xright,int ybottom,int ytop,HDC hdc,COLORREF color)
{
    Outcode out1 = GetOutcode(xs,ys,xleft,xright,ybottom,ytop);
    Outcode out2 = GetOutcode(xe,ye,xleft,xright,ybottom,ytop);

    while((out1.All || out2.All) && !(out1.All & out2.All))
    {
        if(out1.All)
        {
            if(out1.left)
            {
                double xin,yin;
                VIntersect(xleft,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else if(out1.right)
            {
                double xin,yin;
                VIntersect(xright,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else if(out1.bottom)
            {
                double xin,yin;
                HIntersect(ybottom,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            else
            {
                double xin,yin;
                HIntersect(ytop,xs,ys,xe,ye,xin,yin);
                xs = xin;
                ys = yin;
            }
            out1 = GetOutcode(xs,ys,xleft,xright,ybottom,ytop);
        }
        else
        {
             if(out2.left)
            {
                double xin,yin;
                VIntersect(xleft,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else if(out2.right)
            {
                double xin,yin;
                VIntersect(xright,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else if(out2.bottom)
            {
                double xin,yin;
                HIntersect(ybottom,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            else
            {
                double xin,yin;
                HIntersect(ytop,xs,ys,xe,ye,xin,yin);
                xe = xin;
                ye = yin;
            }
            out2 = GetOutcode(xe,ye,xleft,xright,ybottom,ytop);
        }

    }
    if(out1.All == 0 && out2.All == 0)
        DrawLine(hdc,xs,ys,xe,ye,color);
}

void Cohen(int xleft,int xright,int ybottom,int ytop,HDC hdc,COLORREF color)
{
    color = RGB(150, 55, 25);
    for(int i = 0;i<lines.size();i++)
    {
        CohenSuth(lines[i].xs,lines[i].ys,lines[i].xe,lines[i].ye,xleft,xright,ybottom,ytop,hdc,color);
    }
}
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT p;
	COLORREF color = RGB(0, 0, 255);
	HDC hdc;

	switch (message)                  /* handle the messages */
	{
	case WM_DESTROY:
		PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
		break;
	case WM_COMMAND:
		menuItemsId = LOWORD(wParam);
		switch (menuItemsId)
		{
		case MY_FILE_EXIT_ID:
			PostQuitMessage(0);
			break;
        case MY_FILLING_ID:
            fill_flood = true;
		case MY_MIDPOINT_CIRCLE_ID:
			drawCircleMidPoint = true;
			break;
        case MY_CARTESIAN_CIRCLE_ID:
			drawCircle = true;
			break;
        case MY_MIDPOINT_LINE_ID:
			drawLineMidPoint = true;
			break;
        case MY_DDA_ID:
			drawLine = true;
			break;
        case COHEN_ID:
			cohen = true;
			break;
        case HERMIT:
            hermit=true;
            break;
        case BEZIER:
            bezir=true;
            break;
        case POLAR:
            polar=true;
            break;
        case DIRECT:
            direct=true;
            break;
		case SAVE:
            save=true;
            break;
        case LOAD:
            load=true;
            break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	    if(save)
        {
            hdc=BeginPaint(hwnd,&p);
            Save(hdc);
            save=false;
            InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
        if(load)
        {
            hdc=BeginPaint(hwnd,&p);
            Load(hdc);
            load=false;
            InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points

        }
	    if(flag1&&flag2&& direct)
        {
            hdc = BeginPaint(hwnd, &p);
			DrawLine_(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			direct = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
	    if(flag1&& flag2&& polar )
        {
            hdc = BeginPaint(hwnd, &p);
			DrawPolarCircle(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			polar = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
        }
	    if(flag1&& flag2&& flag3&& flag4 &&bezir)
        {
            hdc = BeginPaint(hwnd, &p);
            bezierCurve(hdc,xs,ys,xe,ye,xm,ym,x4,y4,color);
            flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			bezir=false;
			InvalidateRect(hwnd, NULL, FALSE); //to paint more points and remain the prev points
        }
	    if(flag1&& flag2&& flag3&& flag4 &&hermit)
        {
            hdc = BeginPaint(hwnd, &p);
            hermitCurve(hdc,xs,ys,xe,ye,xm,ym,x4,y4,color);
            flag1 = false;
			flag2 = false;
			flag3 = false;
			flag4 = false;
			hermit=false;
			InvalidateRect(hwnd, NULL, FALSE); //to paint more points and remain the prev points
        }
		if (flag1 && flag2 && drawCircleMidPoint)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawCircleMidPoint(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawCircleMidPoint = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawCircle)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawCircle(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawCircle = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawLineMidPoint)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawLineMidPoint(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawLineMidPoint = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if (flag1 && flag2 && drawLine)
		{
			hdc = BeginPaint(hwnd, &p);
			DrawLine(hdc, xs, ys, xe, ye, color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			drawLine = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points
		}
		if(flag1 && fill_flood)
        {
            hdc = BeginPaint(hwnd, &p);
			MyFloodFill(hdc, xs, ys, color, RGB(0, 255, 0) );
			EndPaint(hwnd, &p);
			flag1 = false;
			fill_flood = false;
			InvalidateRect(hwnd, NULL, FALSE);  //to paint more points and remain the prev points

        }
        if(flag1 && flag2 && cohen)
        {
            InvalidateRect(hwnd, NULL, true);
            hdc = BeginPaint(hwnd, &p);
			Cohen(xs,xe,ys,ye,hdc,color);
			EndPaint(hwnd, &p);
			flag1 = false;
			flag2 = false;
			cohen = false;
			  //to paint more points and remain the prev points

        }
		break;
	case WM_LBUTTONDOWN:
		if (!flag1)
		{
		    xs = LOWORD(lParam);
			ys = HIWORD(lParam);
			flag1 = true;
			break;
		}
		else if (!flag2)
		{
			xe = LOWORD(lParam);
			ye = HIWORD(lParam);
			flag2 = true;
		}
		else if(!flag3)
        {
            xm = LOWORD(lParam);
			ym = HIWORD(lParam);
			flag3 = true;
        }
        else{
           {
            x4 = LOWORD(lParam);
			y4 = HIWORD(lParam);
			flag4 = true;
        }
        }
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
