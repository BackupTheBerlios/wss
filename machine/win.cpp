#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdint>

#include <cmath>
#include <list>
#include <vector>
#include <string>

#include "../wcp.h"
#include "../matrix.h"
#include "../position.h"
#include "../retine.h"
#include "../message.h"

#include <map>
#include <io.h>
#include <windows.h>
#include <winuser.h>
#include "standard.h"

#include "symbol/keysym.h"
#include "../library/wdslib.h"
#include "../message.h"
#include "win_sys.h"

#include "win.h"


using namespace std;

void win_c::load_configuration()
{
	string cfg=get_lambs_path();
	cfg+="/config/machinecfg";
	ifstream file(cfg);
//	if ( false ) {
	if(file.is_open()){
		file >> image_width;
		file >> image_height;
		file >> pos_x;
		file >> pos_y;
	}
	else{
		image_width=1500;
		image_height=1000;
		pos_x=400;
		pos_y=30;
	}
	file.close();
}

void fo()
{
	HDC hdc;                 // handle to DC
	CONST BITMAPINFO *pbmi;  // bitmap data
	UINT iUsage;             // data type indicator
	VOID **ppvBits;          // bit values
	HANDLE hSection;         // handle to file mapping object
	DWORD dwOffset;           // offset to bitmap bit values

	HBITMAP hb = CreateDIBSection (
		hdc,                 // handle to DC
		pbmi,  // bitmap data
		iUsage,             // data type indicator
		ppvBits,          // bit values
		hSection,         // handle to file mapping object
		dwOffset           // offset to bitmap bit values
	);
}

void win_c::exit( int value )
{
	save_configuration();
	cout << "exit value:" << value << endl;
	string path=get_lambs_path();
	path+="/config/exitcfg";
	std::ofstream  f(path.c_str());
	f << value;
	f.close();
	PostQuitMessage(0);
	terminated = 1;
}

void win_c::get_clipboard(string& s)
{
	bool bAvailable = IsClipboardFormatAvailable(CF_TEXT);
	if(bAvailable==false){
		return;
	}
	OpenClipboard(hWnd);
	HANDLE hClipMemory=GetClipboardData(CF_TEXT);
	PVOID pClipMemory=GlobalLock(hClipMemory);
//	s=string((char*)pClipMemory);
	s.assign((char*)pClipMemory);
	GlobalUnlock(hClipMemory);
	CloseClipboard();
	cout << "get clipboard:" <<  s << endl;
}

void win_c::set_clipboard(string s)
{
	HGLOBAL hGlobalMemory=GlobalAlloc(GHND,s.size()+1);
	LPVOID pGlobalMemory=GlobalLock(hGlobalMemory);
	strcpy((char*)pGlobalMemory,s.c_str());
	GlobalUnlock(hGlobalMemory);
	OpenClipboard(hWnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hGlobalMemory);
	CloseClipboard();	
	cout << "set clipboard:" <<  s.c_str() << endl;
}

void win_c::save_configuration()
{
	string lambs_path=get_lambs_path();
	string machinecfg;
	string log;
	machinecfg=lambs_path+"/config/machinecfg";
	log=lambs_path+"/lambs.log";
	
	ofstream file(machinecfg);
	ofstream log_file(log);
	if(file.is_open()){
		file << image_width << endl;
		file << image_height << endl;
		
		file << pos_x << endl;
		file << pos_y << endl;
	}
	else{
		cout << "machinecfg: file error" << endl;
	}
	file.close();
	log_file.close();	
}

void win_c::expose_image()
{
	InvalidateRect(hWnd,0,true);
}

bool win_c::is_idle()
{
	return !GetInputState ();
//	return !SDL_HasEvent(SDL_MOUSEMOTION);
}

win_c win;

message_c* get_machine()
{
	return &win;
}

int entry(int,char*[]);

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance,
	LPSTR _lpCmdLine, int _nCmdShow)
{
	cout << "win main" << endl;
	win.hInstance=_hInstance;
	win.hPrevInstance=_hPrevInstance;
	win.nCmdShow=_nCmdShow;			
	char* argv[]={"lamb"};
	return entry(1,argv);
}

const char g_szClassName[] = "myWindowClass";

void dib_section(HDC hdc, int x, int y, uint32_t* p)
{
	BITMAPINFO binfo;
	memset(&binfo,sizeof(binfo),0);
	BITMAPINFOHEADER& bih=binfo.bmiHeader;
	bih.biSize=sizeof(bih);
	bih.biWidth=x;
	bih.biHeight=-y;
	bih.biPlanes=1;
	bih.biBitCount=32;
	bih.biCompression=BI_RGB;
	SetDIBitsToDevice (
		hdc,
		0,
		0,
		x,
		y,
		0,
		0,
		0,
		y,
		p,
		&binfo,
		DIB_RGB_COLORS 
	);
	return;

	for(int cy=0; cy< y;cy++){
		for(int cx=0; cx< x; cx++){
			SetPixel(hdc,x,y,(unsigned int)*p);
			p++;	
		}
	}
}

map<int,int> oem_x11={
{0x1e,XK_a},
{0x30,XK_b},
{0x2e,XK_c},
{0x20,XK_d},
{0x12,XK_e},
{0x21,XK_f},
{0x22,XK_g},
{0x23,XK_h},
{0x17,XK_i},
{0x24,XK_j},
{0x25,XK_k},
{0x26,XK_l},
{0x32,XK_m},
{0x31,XK_n},
{0x18,XK_o},
{0x19,XK_p},
{0x10,XK_q},
{0x13,XK_r},
{0x1f,XK_s},
{0x14,XK_t},
{0x16,XK_u},
{0x2f,XK_v},
{0x11,XK_w},
{0x2d,XK_x},
{0x15,XK_y},
{0x2c,XK_z},
{0x02,XK_1},
{0x03,XK_2},
{0x04,XK_3},
{0x05,XK_4},
{0x06,XK_5},
{0x07,XK_6},
{0x08,XK_7},
{0x09,XK_8},
{0x0a,XK_9},
{0x0b,XK_0},
{0x3b,XK_F1},
{0x3c,XK_F2},
{0x40,XK_F6},
{0x29,XK_grave},
{0x28,XK_apostrophe},
{0x1a,XK_bracketleft},
{0x1b,XK_bracketright},
{0x0c,XK_minus},
{0x0d,XK_equal},
{0x33,XK_comma},
{0x34,XK_period},
{0x35,XK_slash},
{0x2b,XK_backslash},
{0x0f,XK_Tab},
{0x27,XK_semicolon},
{0x1c,XK_Return},
{0x39,XK_space},
{0x01,XK_Escape},
{0x38,XK_Alt_L},
{0x138,XK_Alt_R},
{0x2a,XK_Shift_L},
{0x36,XK_Shift_R},
{0x0e,XK_BackSpace},
{0x1d,XK_Control_L},
{0x11d,XK_Control_R},
{0x56,XK_less},
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static bool mouse_start = true;
	static long mouse_x, mouse_y;
	switch(msg)
	{
		case WM_MOUSEWHEEL:
			if ( wParam == 0x780000 ) {
				win.button_pressed ( 5 );

			}
			else if ( wParam ==  0xff880000) {
				win.button_pressed ( 4 );
			}
			break;
		case WM_MOUSEMOVE:
			long x, y;
			x = lParam & 0x0000ffff;
			y = lParam >> 16;
//			cout << "x:" << x << "  y:" << y << endl;
			if ( mouse_start ) {
				mouse_start = false;
				mouse_x = x;
				mouse_y = y;
			}
			else {
				win.mouse_move ( x - mouse_x, mouse_y  - y);
				mouse_x = x;
				mouse_y = y;
			}
			break;
		case WM_LBUTTONDOWN:
			win.button_pressed ( 1 );
			win.expose_image ();
			cout << "left button down" << endl;
			break;
		case WM_RBUTTONDOWN:
			win.button_pressed ( 2 );
			win.expose_image ();
			cout << "left button down" << endl;
			break;		
		case WM_MBUTTONDOWN:
			win.button_pressed ( 3 );
			win.expose_image ();
			cout << "left button down" << endl;
			break;		
		case WM_MOVE:
			win.pos_x=LOWORD(lParam);
			win.pos_y=HIWORD(lParam);
			cout << win.pos_x << ":" << win.pos_y << endl;
			break;
		case WM_SIZE:
			if ( win.terminated ) {
				break;
			}

			cout << "wm size " << endl;
			RECT rect;
			GetClientRect ( hwnd, &rect );
			win.image_width = rect.right;
			win.image_height = rect.bottom;
			cout << rect.right << "::" << rect.bottom << endl;
//			win.config_change(rect.right, rect.bottom);
//			win.config_change(win.image_width,win.image_height);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:	
			if ( win.terminated ) {
				break;
			}

//			cout << "paint" << endl;
			int w,h;
			char *ar;
			
			retine_c* retine;
			win.expose(&w, &h, &ar, &retine );

			ar = (char*)retine -> surfaces.l.front ().frame;

			uint32_t* p;
			p = (uint32_t*)ar;
			hdc = BeginPaint ( hwnd, &ps );
//			dib_section( hdc, win.image_width, win.image_height, p);
			dib_section ( hdc, w, h, p );
			EndPaint ( hwnd, &ps );
			break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			if ( win.terminated ) {
				break;
			}
		unsigned short scan;
			unsigned short sym;
			scan=lParam >> 16;
			scan&=0x1ff;
			if(oem_x11.count(scan) > 0){
				sym=oem_x11[scan];
			}
		        else{
				cout << "not found" << endl;
				break;
			}
			if ( sym == XK_F2 ) {
				PostQuitMessage ( 0 );
				break;
			}
			bool pressed;
			pressed = msg == WM_KEYDOWN ? true : false;	
			win.key_event ( pressed, sym, 0 );
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine, int nCmdShow)
void
win_c::open()
{
	load_configuration();	
	
	terminated = 0;

	WNDCLASSEX wc;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
//		return 0;
	}
	DWORD dwExStyle=WS_EX_CLIENTEDGE;
	DWORD dwStyle=WS_CAPTION | WS_SYSMENU | WS_SIZEBOX;
	RECT rec;
	rec.left=pos_x;
	rec.top=pos_y;
	rec.right=image_width;
	rec.bottom=image_height;
	/*
 	bool b=AdjustWindowRectEx(
		&rec,
		dwStyle,
		false,
		dwExStyle);
	if(b){
		 cout << "ookkk" <<endl;	
	
	}
	*/
	cout << rec.left << ":" << rec.top << ":" << rec.right << ":" <<   rec.bottom <<endl;

	int frame_width=10;
	int frame_height=32;	
	rec.left=pos_x - frame_width;
	rec.top=pos_y - frame_height;
	rec.right=image_width + 2*frame_width;
	rec.bottom=image_height + frame_height+ frame_width;
	
	cout << rec.left << ":" << rec.top << ":" << rec.right << ":" <<   rec.bottom <<endl;

	hWnd = CreateWindowEx(
		dwExStyle,
		g_szClassName,
		"White Source Software",
		dwStyle,
		rec.left,rec.top,
		rec.right,rec.bottom,
	//	CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if(hWnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
//		return 0;
		return;
	}
	
	init();
	config_change( image_width, image_height);	

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	//	return Msg.wParam;
}

