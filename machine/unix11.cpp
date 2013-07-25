/* lamb  UNIX X11  system interface */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdint.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
//#include <X11/StringDefs.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/xfixesproto.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>




#include "symbol/keysym.h"
#include "../wcp.h"
#include "../matrix.h"
#include "../position.h"
#include "../library/wdslib.h"
#include "../retine.h"
#include "../message.h"
#include "posix_sys.h"
#include "socket.h"

#include "../global.h"



#include "../library/wdslib.h"
#include "../message.h"
#include "machine.h"
#include "unix11.h"
#include "socket.h"

#include "../global.h"

//#ifdef X_HAVE_UTF8_STRING
#define TEXT_FORMAT XInternAtom ( display, "UTF8_STRING", False )
//#else
//#define TEXT_FORMAT XA_STRING
//#endif

using namespace std;

x11_c x11;

message_c* get_machine ()
{
	return &x11;	
}

message_c* get_x11 ()
{
	return &x11;
}

x11_c::x11_c ()
{
	windowing_mode = 2;
	system_cursor_visible = true;
	system_cursor_grabed = false;
}

x11_c::~x11_c ()
{
}

void x11_c::set_clipboard ( string s )
{
	string ss("hallo" );
	Atom XA_CLIPBOARD = XInternAtom ( display, "CLIPBOARD", 0 );
	echo << "set clipboard" << endl;
     XSetSelectionOwner ( display, XA_CLIPBOARD, win, CurrentTime );
	Atom format;
//	XStoreBytes ( display, ss.c_str (), ss.size ());
	
	/*
	XChangeProperty ( display, DefaultRootWindow ( display ),
	XA_CLIPBOARD, 0, 8, PropModeReplace,
	(const unsigned char*)ss.c_str (), ss.size ());
	*/
	Window rw = DefaultRootWindow ( display );
	format = TEXT_FORMAT;
/*
	XChangeProperty ( display, DefaultRootWindow ( display ),
	XA_CUT_BUFFER0, format, 8, PropModeReplace,
        (const unsigned char *)text, SDL_strlen ( text ));
*/
	XChangeProperty ( display, DefaultRootWindow ( display ),
	XA_CUT_BUFFER0, format, 8, PropModeReplace,
//	XA_CLIPBOARD, format, 8, PropModeReplace,
        (const unsigned char *)ss.c_str (), ss.size ());
	XChangeProperty ( display, DefaultRootWindow ( display ),
//	XA_CUT_BUFFER0, format, 8, PropModeReplace,
	XA_CLIPBOARD, format, 8, PropModeReplace,
        (const unsigned char *)ss.c_str (), ss.size ());

	XSetSelectionOwner ( display, XA_CLIPBOARD, rw, CurrentTime );
	XSetSelectionOwner ( display, XA_PRIMARY, rw, CurrentTime );
	XSetSelectionOwner ( display, XA_CUT_BUFFER0, rw, CurrentTime );

}

void x11_c::get_clipboard ( string& s )
{
    Atom format;
    Window window = win;
    Window owner;
    Atom selection;
    Atom seln_type;
    int seln_format;
    unsigned long nbytes;
    unsigned long overflow;
    unsigned char *src;
    char *text;
    Atom XA_CLIPBOARD = XInternAtom ( display, "CLIPBOARD", 0 );
    if (XA_CLIPBOARD == None ) {
//        SDL_SetError("Couldn't access X clipboard" );
//        return NULL;
		return;
    }

    text = NULL;
    /* Get the window that holds the selection */

//    window = GetWindow ( _this );
    format = TEXT_FORMAT;
    owner = XGetSelectionOwner ( display, XA_CLIPBOARD );
    if ((owner == None ) || (owner == window )) {
        owner = DefaultRootWindow ( display );
        selection = XA_CUT_BUFFER0;
    } else {

        /* Request that the selection owner copy the data to our window */

        owner = window;
        selection = XInternAtom ( display, "SDL_SELECTION", False );
        XConvertSelection ( display, XA_CLIPBOARD, format, selection, owner, CurrentTime );

        /* FIXME: Should we have a timeout here? */
/*
        videodata->selection_waiting = SDL_TRUE;
        while (videodata->selection_waiting ) {
            SDL_PumpEvents ();
        }
*/
    }
	string sss;
    if (XGetWindowProperty ( display, owner, selection, 0, INT_MAX/4, False,
            format, &seln_type, &seln_format, &nbytes, &overflow, &src )
            == Success ) {
		echo << "success" << endl;
        if (seln_type == format ) {
			sss.assign((const char*)src,(size_t )nbytes );
         //   text = (char *)SDL_malloc ( nbytes+1 );
        //    if (text ) {
         //       SDL_memcpy ( text, src, nbytes );
        //        text[nbytes] = '\0';
        //    }
        }
        XFree ( src );
    }

    if (!text ) {
       // text = SDL_strdup("" );
    }
	echo << " x11 cbg" << endl;
	echo << sss << endl;
	echo << " ... " << endl;
    return ;
  //  return text;
}



/*
char *
X11_GetClipboardText ( _THIS )
{
    SDL_VideoData *videodata = (SDL_VideoData *) _this->driverdata;
    Display *display = videodata->display;
    Atom format;
    Window window;
    Window owner;
    Atom selection;
    Atom seln_type;
    int seln_format;
    unsigned long nbytes;
    unsigned long overflow;
    unsigned char *src;
    char *text;
    Atom XA_CLIPBOARD = XInternAtom ( display, "CLIPBOARD", 0 );
    if (XA_CLIPBOARD == None ) {
        SDL_SetError("Couldn't access X clipboard" );
        return NULL;
    }

    text = NULL;
*/
    /* Get the window that holds the selection */
/*
    window = GetWindow ( _this );
    format = TEXT_FORMAT;
    owner = XGetSelectionOwner ( display, XA_CLIPBOARD );
    if ((owner == None ) || (owner == window )) {
        owner = DefaultRootWindow ( display );
        selection = XA_CUT_BUFFER0;
    } else {
*/
        /* Request that the selection owner copy the data to our window */
/*
        owner = window;
        selection = XInternAtom ( display, "SDL_SELECTION", False );
        XConvertSelection ( display, XA_CLIPBOARD, format, selection, owner,
            CurrentTime );
*/
        /* FIXME: Should we have a timeout here? */
/*
        videodata->selection_waiting = SDL_TRUE;
        while (videodata->selection_waiting ) {
            SDL_PumpEvents ();
        }
    }

    if (XGetWindowProperty ( display, owner, selection, 0, INT_MAX/4, False,
            format, &seln_type, &seln_format, &nbytes, &overflow, &src )
            == Success ) {
        if (seln_type == format ) {
            text = (char *)SDL_malloc ( nbytes+1 );
            if (text ) {
                SDL_memcpy ( text, src, nbytes );
                text[nbytes] = '\0';
            }
        }
        XFree ( src );
    }

    if (!text ) {
        text = SDL_strdup("" );
    }
    
    return text;
}

SDL_bool
X11_HasClipboardText ( _THIS )
{
    SDL_bool result = SDL_FALSE;
    char *text = X11_GetClipboardText ( _this );
    if (text ) {
        result = (SDL_strlen ( text )>0 ) ? SDL_TRUE : SDL_FALSE;
        SDL_free ( text );
    }    
    return result;
}
*/

/*
int
X11_SetClipboardText ( _THIS, const char *text )
{
    Display *display = ((SDL_VideoData *) _this->driverdata )->display;
    Atom format;
    Window window;
    Atom XA_CLIPBOARD = XInternAtom ( display, "CLIPBOARD", 0 );
*/
    /* Get the SDL window that will own the selection */
/*
    window = GetWindow ( _this );
    if (window == None ) {
        SDL_SetError("Couldn't find a window to own the selection" );
        return -1;
    }
*/
    /* Save the selection on the root window */
/*
    format = TEXT_FORMAT;
    XChangeProperty ( display, DefaultRootWindow ( display ),
        XA_CUT_BUFFER0, format, 8, PropModeReplace,
        (const unsigned char *)text, SDL_strlen ( text ));

	printf("sdl clipboard\n" );
	
    if (0 && XA_CLIPBOARD != None &&
        XGetSelectionOwner ( display, XA_CLIPBOARD ) != window ) {
        XSetSelectionOwner ( display, XA_CLIPBOARD, window, CurrentTime );
    }

    if (XGetSelectionOwner ( display, XA_PRIMARY ) != window ) {
        XSetSelectionOwner ( display, XA_PRIMARY, window, CurrentTime );
    }
    return 0;
}
*/

Window x11_c::create_window ( int x, int y, int width, int height )
{
	unsigned long attributes_mask;
	XSetWindowAttributes window_attributes;	

	attributes_mask = CWBackPixel | CWBorderPixel;	
	window_attributes.border_pixel = BlackPixel ( display, screen );
	window_attributes.background_pixel = WhitePixel ( display, screen );
	int px = pos_x;
	int py = pos_y;
	int sx = width;
	int sy = height;
	if ( windowing_mode == 1 || windowing_mode == 3 ){
		px = 0;
		py = 0;
	}
	if ( windowing_mode == 1 || windowing_mode == 4 ){
		sx=-1;
		sy=-1;
	}
	win = XCreateWindow(
		display,
		DefaultRootWindow (display ),
		px, py,
		sx, sy,
		0,
		DefaultDepth (display, screen ),
		InputOutput, 
		DefaultVisual (display, screen ),
		attributes_mask,
		&window_attributes
	);
	return win;
}

/*
	int w, h;
	char *ar;
	retine_c* retine;
	expose(&w,&h,&ar,&retine );
	if( SDL_MUSTLOCK ( surface )){
		if ( SDL_LockSurface ( surface ) < 0 ){
			cout << "lock return\n";
			return;
		}
	}
	memcpy (surface->pixels, retine->surfaces.l.front ().frame, 4*h*w );
	if( SDL_MUSTLOCK ( surface )){
		SDL_UnlockSurface ( surface );
	}
	SDL_UpdateWindowSurface ( window );
*/

void x11_c::expose_image ()
{
	XImage* image;
	int w, h;
	char *ar;
	retine_c* retine;
	expose(&w,&h,&ar,&retine );

	ar = (char*)retine->surfaces.l.front ().frame;
	image = XCreateImage ( display, visual, 24, ZPixmap, 0, ar, w, h, 32, 0 );
	XInitImage ( image );
	XPutImage ( display, win, copyGC, image, 0, 0, 0, 0, w, h );
	XFlush ( display );
}

/*
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
*/

void x11_c::exit ( int value )
{
	save_configuration ();
	XDestroyWindow ( display, win );
	XCloseDisplay ( display );
	cout << "exit value:" << value << endl;
	std::string path = get_lambs_path ();
	path+="/config/exitcfg";
	ofstream  f ( path.c_str ());
	f << value;
	f.close ();
	::exit ( EXIT_SUCCESS );
}

void x11_c::hide_system_cursor ()
{
	system_cursor_visible = false;
	
	XColor xcol1, xcol2, xcol3, xcol4, xcol5;
	
	xcol1.pixel = 0xff0000;
	xcol1.red = 0xffff;
	xcol1.green = 0x0;
	xcol1.blue = 0x0;
	xcol1.flags = DoRed | DoGreen | DoBlue;
	xcol1.pad = 0;
	
	xcol2.pixel = 0xff00;
	xcol2.red = 0x00;
	xcol2.green = 0xffff;
	xcol2.blue = 0x00;
	xcol2.flags = DoRed | DoGreen | DoBlue;
	xcol2.pad = 0;

	xcol3.pixel = 0xff;
	xcol3.red = 0x00;
	xcol3.green = 0x00;
	xcol3.blue = 0xffff;
	xcol3.flags = DoRed | DoGreen | DoBlue;
	xcol3.pad = 0;
	
	xcol4.pixel = 0xffffff;
	xcol4.red = 0xffffff;
	xcol4.green = 0xffffff;
	xcol4.blue = 0xffffff;
	xcol4.flags = DoRed | DoGreen | DoBlue;
	xcol4.pad = 0;

	xcol5.pixel = 0x00;
	xcol5.red = 0x00;
	xcol5.green = 0x00;
	xcol5.blue = 0x00;
	xcol5.flags = DoRed | DoGreen | DoBlue;
	xcol5.pad = 0;
	
	unsigned char bmp1[]={0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff};	
	unsigned char bmp2[]={0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
	unsigned char bmp3[]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};	
	unsigned char bmp4[]={0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	
	Pixmap pixmap1, pixmap2;
	pixmap1 = XCreatePixmapFromBitmapData ( display, win,(char*)bmp2, 8, 8, 1, 0, 1 );	
	pixmap2 = XCreatePixmapFromBitmapData ( display, win,(char*)bmp2, 8, 8, 0, 0, 1 );	
	Cursor cursor = XCreatePixmapCursor ( display, pixmap1, pixmap2,&xcol1,&xcol5, 1, 1 );
	XDefineCursor ( display, win, cursor );

}

void x11_c::show_system_cursor ()
{
	system_cursor_visible = true;
	XUndefineCursor ( display, win );
}


void x11_c::save_configuration ()
{
	std::string lambs_path = get_lambs_path ();
	std::string unixcfg;
	std::string log;
	unixcfg = lambs_path+"/unixcfg";
	log = lambs_path+"/lambs.log";
	int xp = 0;
	int yp = 0;
	unsigned int sxp = 0;
	unsigned int syp = 0;
	unsigned int bwp = 0;
	unsigned int depthp = 0;
	unsigned int children_n = 0;
	Window wroot, wparent, wdum, child_return;

	Window* children_list;
	
	XQueryTree ( display, win,&wdum,&wparent,&children_list,&children_n );	
	XFree ( children_list );
	
	XGetGeometry ( display, wparent,&wroot,&xp,&yp,&sxp,&syp,&bwp,&depthp );
	
	int dest_x_return, dest_y_return;
	
	//wroot = DefaultRootWindow ( display );
	
	bool b = XTranslateCoordinates ( display, win, wroot,-xp,-yp,&dest_x_return,&dest_y_return,&child_return );
	std::cout << xp<< ":" << yp << ":" << sxp << ":" << syp << ":" << image_width << ":bw:" << bwp << std::endl;
	
	std::ofstream file ( unixcfg );
	std::ofstream log_file ( log );
	if ( file.is_open ()){
	
		file << image_width << std::endl;
		file << image_height << std::endl;
		
		file << dest_x_return << std::endl;
		file << dest_y_return << std::endl;
		
	}
	else{
		std::cout << "unixcfg: file error" << std::endl;
	}
	file.close ();
	log_file.close ();	
}


void x11_c::load_configuration ()
{
	std::string unixcfg = get_lambs_path ();
	unixcfg+="/unixcfg";
	std::ifstream file ( unixcfg );
	if ( file.is_open ()){
		file >> image_width;
		file >> image_height;
		file >> pos_x;
		file >> pos_y;
	}
	else{
		image_width = 200;
		image_height = 200;
		pos_x = 20;
		pos_y = 20;
	}
	file.close ();
}

bool x11_c::is_idle ()
{
	int n = XEventsQueued ( x11.display, QueuedAlready ); //QueuedAfterFlush
	cout << "machine is_idle" << endl;

	if ( n == 0 ){
		return true;
	}
	return false;
}

void x11_c::grab_system_cursor ( int* x, int* y )
{
	if ( x!= 0 && y!= 0 ){
		Window root_return, child_return;
		int x_root, y_root;
		unsigned int mask_return;
		XQueryPointer ( display, win,&root_return,&child_return,&x_root,&y_root, x, y,&mask_return );
	}
//	std::cout << "query pointer:" << b << "\n";
	XGrabPointer ( display, win, true, PointerMotionMask| ButtonPressMask|EnterWindowMask| LeaveWindowMask , GrabModeAsync, GrabModeAsync, None, None, CurrentTime );
	system_cursor_grabed = true;
}


void x11_c::ungrab_system_cursor ( int x, int y )
{
	XWarpPointer ( display, None, win, 0, 0, 0, 0, x, y );
	XUngrabPointer ( display, CurrentTime );
	XSync ( display, true );
	system_cursor_grabed = false;
}


//============================================
//					us keyboard scan
//
//	`	1	2	3	4	5	6	7	8	8	0	-	=	BKS
//
//		TAB 	Q	W	E	R	T	Y	U	I	O	P	[	]	\¹	   
//
//		CAP	A	S	D	F	G	H	J	K	L	;	'	\	RET
//
// SHIFT	<	Z	X	C	V	B	N	M	,	.	/	SHIFT
//	
// ¹ backslash position on some keyboard
//=============================================

map < int, int > scan_x11_x11 = {
	{47, XK_semicolon},
	{34,XK_bracketleft},
	{35,XK_bracketright},
	{20,XK_minus},
	{21,XK_equal},
	{22,XK_BackSpace},
	{9,XK_Escape},
	{51,XK_backslash},	
	{61,XK_slash},
	{36, XK_Return},
	{49, XK_grave},
	{29, XK_y},
	{51, XK_z}
};

/*
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
*/

map < int, int >& scan = scan_x11_x11;


void x11_c::open ()
{
	bool private_cursor;
	private_cursor = false;
	private_cursor = true;

	
	display = XOpenDisplay(":0" );
	
	if ( display == 0 ){
		cout << "display error" << endl;
		return;
	}
	int opcode, event, error;
	if(!XQueryExtension ( display, "XInputExtension", &opcode, &event, &error )){
		cout << "X Input extension not available" << endl;
		return;
	}
	int major = 2, minor = 0;
	if ( XIQueryVersion ( display,&major, &minor ) == BadRequest ) {
		cout << "XI2 not available. Server support " << major <<"." << minor << endl;
		return;
	}

	pseudo_motion = XInternAtom ( display,"pseudo_motion", false );
	Atom last_client_message;
	
	screen = DefaultScreen ( display );
	depth = DefaultDepth ( display, screen );
	visual= DefaultVisual ( display, screen );
	
	load_configuration ();	
	std::cout << pos_x <<":"<< pos_y<< ":" << image_width << ":" << image_height << std::endl;
	
	win = create_window ( pos_x, pos_y, image_width, image_height );
	XStoreName ( display, win,"White Source Software: x11" );	
	xcme.type = ClientMessage;
	xcme.display = display;
	xcme.window = win;
	xcme.message_type = pseudo_motion;
	xcme.format = 8;

	unsigned long mask;	
	XSizeHints* size_hints = XAllocSizeHints ();
	mask= PPosition;
	size_hints->flags = mask;
	XSetWMNormalHints ( display, win, size_hints );
	XFree ( size_hints );
	copyGC = XCreateGC ( display, win, 0, NULL );
	mask = ExposureMask | KeyReleaseMask | KeyPressMask | PointerMotionMask | ButtonMotionMask | ButtonPressMask | StructureNotifyMask | EnterWindowMask | FocusChangeMask;
	XSelectInput ( display, win, mask );
	XMapRaised ( display, win );
	
//	XChangePointerControl ( display, true, true, 1, 10, 1 );	
	int actualx = 0;
	int actualy = 0;
	int centerx= 30;
	int centery= 30;
	int deltax = 0;
	int deltay = 0;
	
//	bool pseudo_motion = false;
	
//	XEvent event_dum;
	cout << "x11 looop " << endl;
	init ();	
	config_change ( image_width, image_height );	

	while ( true ) {
		XEvent event;
		XNextEvent ( display, &event );
		switch ( event.type ) {
		case MapNotify:
			std::cout << "MapNotify" << std::endl;
			init ();
			break;
		case ConfigureNotify:
			cout << "configue notify " << endl;
			if ( event.xconfigure.width!= image_width || 
				event.xconfigure.height!= image_height ){
				image_width = event.xconfigure.width;
				image_height = event.xconfigure.height;
				config_change ( image_width, image_height );
			}
			break;
		case FocusIn:
			cout << "focus" << endl;
			focus ();
			break;
		case FocusOut:
			break;
		case LeaveNotify:
			std::cout << "LeaveNotify" << std::endl;	
			break;
		case EnterNotify:
			cout << "enter notify" << endl;
			XCrossingEvent* xewe;
			xewe=(XCrossingEvent*)&event;
			if ( xewe->mode == NotifyNormal && system_cursor_grabed == false && private_cursor == true ){
				std::cout << "EnterNormal" << std::endl;
				actualx = xewe->x;
				actualy = xewe->y;
				int x, y;
				grab_system_cursor(&x,&y );
				std::cout << actualx << "::" << actualy << "\n";
				std::cout << x << "::" << y << "\n";
				grab_pointer ( actualx, actualy );
				hide_system_cursor ();	

			}
			else if ( xewe->mode == NotifyGrab ){
				std::cout << "NotifyGrab" << std::endl;
			}
			else if ( xewe->mode == NotifyUngrab ){
				std::cout << "NotifyUngrab" << std::endl;
			}
			break;
		case Expose:
			cout << "expose "<< endl;
			expose_image ();
			break;            
		case KeyPress:
		case KeyRelease:
			unsigned short sym;
			sym = XLookupKeysym(&event.xkey, 0 );
			unsigned short keycode;
			keycode = event.xkey.keycode;
			bool pressed;
			pressed = event.type == KeyPress?true:false;
			switch ( sym ){
			case XK_F12:
				if ( pressed ){
					if ( system_cursor_visible ){
						hide_system_cursor ();
					}
					else{
						show_system_cursor ();
					}
				}
				break;
			case XK_F11:
				if ( pressed ){
					if ( private_cursor == true ){
						show_system_cursor ();
						ungrab_system_cursor ( 0, 50 );
						private_cursor = false;
					}
					else {
						hide_system_cursor ();
						private_cursor = true;
						int x, y;
						grab_system_cursor(&x,&y );
						system_cursor_grabed = true;
						XWarpPointer ( display, None, win, 0, 0, 0, 0, centerx, centery );
						actualx = centerx;
						actualy = centery;
					}
				}
				break;
			case XK_F2:
				exit ( 0 );
				break;
			default:
				echo << "sym:" << sym << "   code:" << hex << keycode << endl;
				if ( scan.count (  (unsigned short )event.xkey.keycode ) > 0 ) {
					sym = scan [ (unsigned short) event.xkey.keycode];
				}
				key_event ( pressed, sym, 0 );
			}
			break;
		case ClientMessage:
			XClientMessageEvent* cme;
			cme=(XClientMessageEvent*)&event;
			last_client_message = cme->message_type;			
			std::cout << "client Message:" <<XGetAtomName ( display, last_client_message ) << "\n";
			break;
		case  MotionNotify:
			if(!system_cursor_grabed ){
				break;
			}
			if ( last_client_message!= pseudo_motion ){
				XSendEvent ( display, win, false, 0,(XEvent*)&xcme );
				XWarpPointer ( display, None, win, 0, 0, 0, 0, centerx, centery );
				deltax = event.xmotion.x-actualx;
				deltay = event.xmotion.y-actualy;
				actualx = event.xmotion.x;
				actualy = event.xmotion.y;
				mouse_move ( deltax,-deltay );				
				std::cout << "motion:";
			}
			else{
				actualx = centerx;
				actualy = centery;
				last_client_message = 0;
				std::cout << "pseudo motion:";
			}
			
			std::cout << event.xmotion.x << ":" << event.xmotion.y << "\n";
			break;
		case ButtonPress:
			int number;
			switch ( event.xbutton.button ){
			case Button4 :
				number = 4; break;
			case Button5 :
				number = 5; break;
			case Button1 :
				number = 1; break;
			case Button2 :
				number = 2; break;
			case Button3 :
				number = 3; break;
			default :
				number = 0;
			}
			button_pressed ( number );
			expose_image ();
			break;
		}
		int n = XEventsQueued ( x11.display, QueuedAlready );
		if ( n == 0 ){
			std::cout << "no event queued\n";
			idle ();
		}
		else{
			std::cout << n <<" events queued\n";
		}
	}
	return;
}

int entry ( int argc, char* argv[]);

int main ( int argc, char* argv[])
{
	cout << "main" << endl;
	return entry ( argc, argv );
}


