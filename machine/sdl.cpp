/* lamb SDL system interface */
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <map>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fts.h>

#include <X11/Xlib.h>

#include <SDL.h>

#include "symbol/keysym.h"
#include "../wcp.h"
#include "../matrix.h"
#include "../position.h"
#include "../library/wdslib.h"
#include "../retine.h"
#include "../message.h"
#include "posix_sys.h"
#include "sdl.h"
#include "socket.h"

#include "../global.h"

using namespace std;

namespace pointer{
enum {
	disabled,
	outside,
	inside,
	owner_start,
	owner
};
}

sdl_c sdl;

int sdl_c::nop_system ( int x )
{
	cout << "nop sytem:" << x << endl;
	return x + 1;
}

sdl_c::sdl_c ():flag ( pointer::disabled )
{
}

void sdl_c::walk ( string path, list<string>& l )
{
	cout << "machine walk" << endl;
	char* dl[]={(char*)path.c_str (), 0};
	FTS* fts = fts_open ( dl, FTS_PHYSICAL, 0 );
	FTSENT* ftsent;
	while ( ftsent = fts_read ( fts )){
		string s = ftsent->fts_name;
		if ( true ){
			l.push_back ( ftsent->fts_path );
		}
	}
	fts_close ( fts );
	return;
}

void sdl_c::change_pwd ( string s )
{
	chdir ( s.c_str ());	
}

bool sdl_c::download ( string& url, stringstream& ss )
{
	string result;
	echo << "download" << endl;
	socket_c socket;
	bool b = socket.get_webpage ( url, result );
	ss << result;
	return b;
}

void sdl_c::get_clipboard ( string& s )
{
	char* pc= SDL_GetClipboardText ();
	s.assign ( pc );
	echo << "get clipboard:" <<  s << endl;
}

void sdl_c::set_clipboard ( string s )
{

	string paths = get_lambs_path () + "/tmp/clip";
	ofstream f ( paths );
	f << s;
	f.close ();
	
	// kde special case, maybe xclip on other desktop
	bool kde = true;
	if ( kde ){
		string co="qdbus org.kde.klipper /klipper setClipboardContents \"$(<" + paths +" )\"" ; 	
		system ( co.c_str ());
	}
	else{	
		bool b = SDL_SetClipboardText ( s.c_str ());
		if(!b ){
			string er ( SDL_GetError ());	
			echo << "set clipboard error:" << er << endl;
		}
	}
	echo << "set clipboard:" <<  s.c_str () << endl;
}


void sdl_c::window_management ( string s )
{
	if ( s == "fs" ){
		SDL_SetWindowFullscreen ( window, SDL_TRUE );
	}
	else if (s == "ns" ){
		SDL_SetWindowFullscreen ( window, SDL_FALSE );
	}
	else if (s == "min" ){
		SDL_MinimizeWindow ( window );
	}
	else if (s == "max" ){
		SDL_MaximizeWindow ( window );
	}
	else if (s == "res" ){
		SDL_RestoreWindow ( window );
	}
	else if (s == "rai" ){
		SDL_RaiseWindow ( window );
	}
}

map < int, int > scan_sdl_x11 = {
{SDL_SCANCODE_A,XK_a},
{SDL_SCANCODE_B,XK_b},
{SDL_SCANCODE_C,XK_c},
{SDL_SCANCODE_D,XK_d},
{SDL_SCANCODE_E,XK_e},
{SDL_SCANCODE_F,XK_f},
{SDL_SCANCODE_G,XK_g},
{SDL_SCANCODE_H,XK_h},
{SDL_SCANCODE_I,XK_i},
{SDL_SCANCODE_J,XK_j},
{SDL_SCANCODE_K,XK_k},
{SDL_SCANCODE_L,XK_l},
{SDL_SCANCODE_M,XK_m},
{SDL_SCANCODE_N,XK_n},
{SDL_SCANCODE_O,XK_o},
{SDL_SCANCODE_P,XK_p},
{SDL_SCANCODE_Q,XK_q},
{SDL_SCANCODE_K,XK_k},
{SDL_SCANCODE_R,XK_r},
{SDL_SCANCODE_S,XK_s},
{SDL_SCANCODE_T,XK_t},
{SDL_SCANCODE_U,XK_u},
{SDL_SCANCODE_V,XK_v},
{SDL_SCANCODE_W,XK_w},
{SDL_SCANCODE_X,XK_x},
{SDL_SCANCODE_Y,XK_y},
{SDL_SCANCODE_Z,XK_z},
{SDL_SCANCODE_1,XK_1},
{SDL_SCANCODE_2,XK_2},
{SDL_SCANCODE_3,XK_3},
{SDL_SCANCODE_4,XK_4},
{SDL_SCANCODE_5,XK_5},
{SDL_SCANCODE_6,XK_6},
{SDL_SCANCODE_7,XK_7},
{SDL_SCANCODE_8,XK_8},
{SDL_SCANCODE_9,XK_9},
{SDL_SCANCODE_0,XK_0},
{SDL_SCANCODE_RETURN,XK_Return},
{SDL_SCANCODE_ESCAPE,XK_Escape},
{SDL_SCANCODE_BACKSPACE,XK_BackSpace},
{SDL_SCANCODE_TAB,XK_Tab},
{SDL_SCANCODE_SPACE,XK_space},
{SDL_SCANCODE_MINUS,XK_minus},
{SDL_SCANCODE_EQUALS,XK_equal},
{SDL_SCANCODE_LEFTBRACKET,XK_bracketleft},
{SDL_SCANCODE_RIGHTBRACKET,XK_bracketright},
{SDL_SCANCODE_BACKSLASH,XK_backslash},
{SDL_SCANCODE_SEMICOLON,XK_semicolon},
{SDL_SCANCODE_APOSTROPHE,XK_apostrophe},
{SDL_SCANCODE_GRAVE,XK_grave},
{SDL_SCANCODE_COMMA,XK_comma},
{SDL_SCANCODE_PERIOD,XK_period},
{SDL_SCANCODE_SLASH,XK_slash},
{SDL_SCANCODE_CAPSLOCK,XK_Caps_Lock},
{SDL_SCANCODE_F1,XK_F1},
{SDL_SCANCODE_F2,XK_F2},
{SDL_SCANCODE_F3,XK_F3},
{SDL_SCANCODE_F4,XK_F4},
{SDL_SCANCODE_F5,XK_F5},
{SDL_SCANCODE_F6,XK_F6},
{SDL_SCANCODE_F7,XK_F7},
{SDL_SCANCODE_F8,XK_F8},
{SDL_SCANCODE_F9,XK_F9},
{SDL_SCANCODE_F10,XK_F10},
{SDL_SCANCODE_F11,XK_F11},
{SDL_SCANCODE_F12,XK_F12},
{SDL_SCANCODE_PRINTSCREEN,XK_Print},
{SDL_SCANCODE_SCROLLLOCK,XK_Scroll_Lock},
{SDL_SCANCODE_PAUSE,XK_Pause}, 
{SDL_SCANCODE_INSERT,XK_Insert},
{SDL_SCANCODE_HOME,XK_Home}, 
{SDL_SCANCODE_PAGEUP,XK_Page_Up},
{SDL_SCANCODE_DELETE,XK_Delete},
{SDL_SCANCODE_END,XK_End},
{SDL_SCANCODE_PAGEDOWN,XK_Page_Down}, 
{SDL_SCANCODE_RIGHT,XK_Right},
{SDL_SCANCODE_LEFT,XK_Left},
{SDL_SCANCODE_DOWN,XK_Down},
{SDL_SCANCODE_UP,XK_Up},
{SDL_SCANCODE_NONUSBACKSLASH,XK_less},
{SDL_SCANCODE_LCTRL,XK_Control_L},
{SDL_SCANCODE_LSHIFT,XK_Shift_L},
{SDL_SCANCODE_LALT,XK_Alt_L},
{SDL_SCANCODE_LGUI,XK_Meta_L}, 
{SDL_SCANCODE_RCTRL,XK_Control_R},
{SDL_SCANCODE_RSHIFT,XK_Shift_R}, 
{SDL_SCANCODE_RALT,XK_Alt_R}, 
{SDL_SCANCODE_RGUI,XK_Meta_R}, 
};

void sdl_c::exit ( int value )
{
	save_configuration ();
	cout << "exit value:" << value << endl;
	string path = get_lambs_path ();
	path+="/config/exitcfg";
	std::ofstream  f ( path.c_str ());
	f << value;
	f.close ();
	SDL_Event event;
	event.type = SDL_QUIT;		
	SDL_PushEvent(&event );
}

void sdl_c::save_configuration ()
{
	string lambs_path = get_lambs_path ();
	string machinecfg;
	string log;
	machinecfg = lambs_path+"/config/machinecfg";
	log = lambs_path+"/lambs.log";
	
	int x, y;
	SDL_GetWindowPosition ( window, &x, &y );
	cout << "position:" << x << ":" << y << endl;
	
	ofstream file ( machinecfg );
	ofstream log_file ( log );
	if ( file.is_open () ){
		file << image_width << endl;
		file << image_height << endl;
		
		file << x-2 << endl;
		file << y-23 << endl;
	}
	else{
		cout << "machinecfg: file error" << endl;
	}
	file.close ();
	log_file.close ();	
}

void sdl_c::load_configuration ()
{
	string cfg = get_lambs_path ();
	cfg+="/config/machinecfg";
	ifstream file ( cfg );
	if ( file.is_open () ){
		file >> image_width;
		file >> image_height;
		file >> pos_x;
		file >> pos_y;
	}
	else {
		image_width = 200;
		image_height = 200;
		pos_x = 20;
		pos_y = 20;
	}
	file.close ();
}

void sdl_c::expose_image ()
{
	int w, h;
	char *ar;
	retine_c* retine;
	expose ( &w, &h, &ar, &retine );
	if ( SDL_MUSTLOCK ( surface ) ){
		if ( SDL_LockSurface ( surface ) < 0 ){
			cout << "lock return\n";
			return;
		}
	}
	memcpy ( surface->pixels, retine->surfaces.l.front ().frame, 4*h*w );
	if ( SDL_MUSTLOCK ( surface ) ){
		SDL_UnlockSurface ( surface );
	}
	SDL_UpdateWindowSurface ( window );
}

bool sdl_c::is_idle ()
{
	return !SDL_HasEvents ( SDL_KEYDOWN, SDL_MOUSEMOTION );
}

void sdl_c::grab_system_cursor ( int* x, int * y )
{
//	echo << "grab system cursor" << endl;
	SDL_GetMouseState ( x, y );
}

void sdl_c::ungrab_system_cursor ( int x, int y )
{
//	echo << "ungrab system cursor" << endl;
	SDL_SetRelativeMouseMode ( SDL_FALSE );
	SDL_SetWindowGrab ( window, SDL_FALSE );
	SDL_WarpMouseInWindow ( window, x, y );
	int sx, sy;
	SDL_GetWindowSize ( window,&x,&y );
	if ( x<0 || x>sx || y<0 || y >sy ){
		flag = pointer::outside;
	}
	else {
		flag = pointer::inside;
	}
}

void sdl_c::open ()
{
	int x, y;

	flag = pointer::outside;
	if ( SDL_Init ( SDL_INIT_VIDEO ) == -1 ){
		cout << "SDL: video error:" << SDL_GetError () << endl;
		exit ( 1 );
	}
	load_configuration ();	
	
	window = SDL_CreateWindow(
		"White Source Software; SDL",
		pos_x,
		pos_y,
		image_width,
		image_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
	if ( window == 0 ){
		cout << "SDL: window creation error:" << endl;
		exit ( 1 );
	}

	surface= SDL_GetWindowSurface ( window );
	if ( surface == 0 ){
		cout << "SDL: surface error" << endl;
	}
	else {
		cout << "SDL: surface ok" << endl;
	}
	if ( flag!= pointer::disabled ){
		if ( -1 != SDL_SetRelativeMouseMode ( SDL_TRUE )){
			cout << "SDL: relative mouse mode ok" << endl;
		}
		else {
			cout << "SDL: relative mouse mode not supported" << endl;
		}
		SDL_SetRelativeMouseMode ( SDL_FALSE );
	}
	echo << "SDL: ok" << endl;
	
	init ();	
	config_change ( image_width, image_height );	
	
	bool begin = true;
	bool done = false;
	
	SDL_Event event;
	while ( ! done && SDL_WaitEvent ( &event ) ){
		switch ( event.type ){
		int s;
		case SDL_QUIT:
			done = true;
			break;
		case SDL_WINDOWEVENT:
			switch ( event.window.event ){
				case SDL_WINDOWEVENT_EXPOSED:
					break;
				case SDL_WINDOWEVENT_SHOWN:
					break;
				case SDL_WINDOWEVENT_ENTER:
					SDL_GetMouseState(&x,&y );
					if ( flag != pointer::outside ){
						break;
					}
					flag = pointer::owner_start;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					if ( flag == pointer::inside ){
						flag = pointer::outside;
					}
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					focus ();
					break;
				case SDL_WINDOWEVENT_RESIZED:
					image_width = event.window.data1;
					image_height = event.window.data2;
					SDL_FreeSurface ( surface );
					surface = SDL_GetWindowSurface ( window );
					config_change ( image_width, image_height );
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			if ( flag == pointer::owner_start ){
				SDL_GetMouseState(&x, &y );
				SDL_SetRelativeMouseMode ( SDL_TRUE );
				SDL_SetWindowGrab ( window, SDL_TRUE );
				grab_pointer ( x, y );
				flag = pointer::owner;
				break;
			}
			if ( flag == pointer::owner ){
				mouse_move ( event.motion.xrel, -event.motion.yrel );
			}
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if ( event.key.keysym.scancode == SDL_SCANCODE_F2 ) {
				exit ( 0 );
				done = true;
				break;
			}
			unsigned short sym;
			if ( scan_sdl_x11.count ( event.key.keysym.scancode ) > 0 ) {
				sym = scan_sdl_x11 [event.key.keysym.scancode ];
			}
			bool pressed;
			pressed = event.type == SDL_KEYDOWN?true:false;
			key_event ( pressed, sym, 0 );
			break;
		case SDL_MOUSEWHEEL:
			if ( event.wheel.y < 0 ){
				button_pressed ( 4 );
			}
			else if (event.wheel.y > 0 ){
				button_pressed ( 5 );
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			int number;
			switch ( event.button.button ){
			case SDL_BUTTON_LEFT :
				number = 1; break;
			case SDL_BUTTON_RIGHT :
				number = 2; break;
			case SDL_BUTTON_MIDDLE :
				number = 3; break;
			default :
				number = 0;
			}
			button_pressed ( number );
			expose_image ();
			break;
		}	
		if(!SDL_HasEvent ( 0 )){
			idle ();
		}
	}
	SDL_Quit ();
}

message_c* get_machine ()
{
	return &sdl;
}

int entry ( int argc, char* argv[]);

int main ( int argc, char* argv[])
{
	cout << "main" << endl;
	return entry ( argc, argv );
}

