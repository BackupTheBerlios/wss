#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/fb.h>
#include <linux/input.h>
#include <linux/vt.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <list>

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
#include "socket.h"

#include "event.h"
#include "loop.h"
#include "linux.h"

#include "../symbol/keysym.h"

using namespace std;

linux_c linux;

/* Use this variable to remember original terminal attributes. */     struct termios saved_attributes;

void
reset_input_mode (void )
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes );
}

void
set_input_mode (void )
{
	struct termios tattr;
	char *name;
	/* Make sure stdin is a terminal. */
	if (!isatty (STDIN_FILENO ))
	{
		fprintf (stderr, "Not a terminal.\n" );
		exit (EXIT_FAILURE );
	}
	/* Save the terminal attributes so we can restore them later. */
	tcgetattr (STDIN_FILENO, &saved_attributes );
	atexit (reset_input_mode );

	/* Set the funny terminal modes. */
	tcgetattr (STDIN_FILENO, &tattr );
	tattr.c_lflag &= ~(ICANON|ECHO ); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr );
}

int
main_ (void )
{
	char c;
	set_input_mode ();
	while (1 )
	{
		read (STDIN_FILENO, &c, 1 );
		if (c == '\004')          /* `C-d' */
			break;
		else
			putchar (c );
	}
	return EXIT_SUCCESS;
}


int
testtty ( void )
{
	int fd;
	char *device = "/dev/vcsa1";
	char *console = "/dev/tty1";
	struct {unsigned char lines, cols, x, y;} scrn;
	unsigned short s;
	unsigned short mask;
	unsigned char ch, attrib;

	fd = open ( console, O_RDWR );
	if (fd < 0 ) {
		perror ( console );
		return 1;
		exit ( EXIT_FAILURE );
	}
	if (ioctl ( fd, VT_GETHIFONTMASK, &mask ) < 0 ) {
		perror("VT_GETHIFONTMASK" );
		return 1;
		exit ( EXIT_FAILURE );
	}
	(void ) close ( fd );
	fd = open ( device, O_RDWR );
	if (fd < 0 ) {
		perror ( device );
		return 1;
		exit ( EXIT_FAILURE );
	}
	(void ) read ( fd, &scrn, 4 );
	(void ) lseek ( fd, 4 + 2*(scrn.y*scrn.cols + scrn.x ), 0 );
	(void ) read ( fd, &s, 2 );
	ch = s & 0xff;
	if (attrib & mask )
		ch |= 0x100;
	attrib = ((s & ~mask ) >> 8 );
	printf("ch='%c' attrib = 0x%02x\n", ch, attrib );
	attrib ^= 0x10;
	(void ) lseek ( fd, -1, 1 );
	(void ) write ( fd, &attrib, 1 );
	return 0;
	exit ( EXIT_SUCCESS );
}

void linux_c::exit ( int value )
{
save_configuration ();
	cout << "exit value:" << value << endl;
	string path = get_lambs_path ();
	path+="/config/exitcfg";
	ofstream  f ( path.c_str ());
	f << value;
	f.close ();
	destroy_window ();
	::exit ( 0 );
}

void linux_c::expose_image ()
{
	int w, h;
	char *ar;
	retine_c* retine;
	expose(&w,&h,&ar,&retine );
	ar = (char*)retine->surfaces.l.front ().frame;
//	expose(&w,&h,&ar );
	if ( bits_per_pixel == 32 ){
		uint32_t *p32= (uint32_t*) fb;
		uint32_t* ps=(uint32_t*)ar;
		int c = 0;
		for ( int y = 0;y<h;y++){
			for ( int x = 0;x<w;x++){
				p32[x+pos_x+(y+pos_y )*VarInfo.xres]=*(ps+c );
				c++;
			}
		}
	}
	else if ( bits_per_pixel == 16 ){
		uint16_t *p16= (uint16_t*) fb;
		uint32_t* ps=(uint32_t*)ar;
		int c = 0;
		for ( int y = 0;y<h;y++){
			for ( int x = 0;x<w;x++){
				p16[x+pos_x+(y+pos_y )*VarInfo.xres]=*(ps+c );
				c++;
			}
		}
	
	}
	cout << "expose image" << endl;
}

map<int, int> linux_x11={ 
{0x1e, XK_a},
{0x30, XK_b},
{0x2e, XK_c},
{0x20, XK_d},
{0x12, XK_e},
{0x21, XK_f},
{0x22, XK_g},
{0x23, XK_h},
{0x17, XK_i},
{0x24, XK_j},
{0x25, XK_k},
{0x26, XK_l},
{0x32, XK_m},
{0x31, XK_n},
{0x18, XK_o},
{0x19, XK_p},
{0x10, XK_q},
{0x13, XK_r},
{0x1f, XK_s},
{0x14, XK_t},
{0x16, XK_u},
{0x2f, XK_v},
{0x11, XK_w},
{0x2d, XK_x},
{0x15, XK_y},
{0x2c, XK_z},
{0x02, XK_1},
{0x03, XK_2},
{0x04, XK_3},
{0x05, XK_4},
{0x06, XK_5},
{0x07, XK_6},
{0x08, XK_7},
{0x09, XK_8},
{0x0a, XK_9},
{0x0b, XK_0},
{0x0c, XK_minus},
{0x0d, XK_equal},
{0x29, XK_grave},
{0x27, XK_semicolon},
{0x28, XK_apostrophe},
{0x2b, XK_backslash},
{0x33, XK_comma},
{0x34, XK_period},
{0x35, XK_slash},
{0x56, XK_less},
{0x1a, XK_bracketleft},
{0x1b, XK_bracketright},
{0x01, XK_Escape},
{0x39, XK_space},
{0x1c, XK_Return},
{0x0f, XK_Tab},
{0x0e, XK_BackSpace},
{0x2a, XK_Shift_L},
{0x36, XK_Shift_R},
{0x1d, XK_Control_L},
{0x61, XK_Control_R},
{0x38, XK_Meta_L},
{0x64, XK_Meta_R},
{0x3b, XK_F1},
{0x3c, XK_F2},
{0x3d, XK_F3},
{0x3e, XK_F4},
{0x3f, XK_F5},
{0x40, XK_F6},
{0x41, XK_F7},
{0x42, XK_F8},
{0x43, XK_F9},
{0x44, XK_F10},
{0x57, XK_F11},
{0x58, XK_F12}
};

bool linux_c::download ( string& url, stringstream& ss )
{
	string result;
	cout << "download";
	socket_c socket;
	bool b = socket.get_webpage ( url, result );
	ss << result;
	return b;
}


bool linux_c::is_idle ()
{
	return true;
}

int linux_c::create_window ()
{

	char* FB_NAME="/dev/fb0";
	m_FBFD = ::open ( FB_NAME, O_RDWR );
	if ( m_FBFD < 0 ){
		cout << "fb open error\n" << endl;
		return 1;
	}
	if ( ioctl ( m_FBFD, FBIOGET_FSCREENINFO,&FixInfo )<0 ){
		cout << "fixed info error\n" << endl;
		close ( m_FBFD );
		return 1;
	}
	if ( ioctl ( m_FBFD, FBIOGET_VSCREENINFO,&VarInfo )<0 ){
		cout << "var info error\n" << endl;
		close ( m_FBFD );
		return 1;
	}
	bits_per_pixel = VarInfo.bits_per_pixel;
	cout << "bits per pixel:" << bits_per_pixel << endl;
	fb_size= FixInfo.line_length* VarInfo.yres;
	fb= mmap ( 0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_FBFD, 0 );
	if ( fb == 0 ){
		cout << "mmap failed\n" << endl;
		close ( m_FBFD );
		return 1;
	}
	else {
		cout << "mape size:" << fb_size << endl;
	}
	uint16_t *p= (uint16_t*) fb;
	int x, y;
	int mx, my;
	mx = image_width;
	my = image_height;
	for ( y = 0; y<my; y++){
		for ( x = 0; x<mx;x++){
//			p[x+pos_x+(y+pos_y )*VarInfo.xres]= 0x00ff00;
		}
	}
	return 0;
}

void linux_c::destroy_window ()
{
	munmap ( fb, fb_size );
	close ( m_FBFD );

}

void linux_c::save_configuration ()
{
	string lambs_path = get_lambs_path ();
	string machinecfg;
	string log;
	machinecfg = lambs_path+"/machine/machinecfg";
	log = lambs_path+"/machine/lambs.log";
	
	int x, y;
	cout << "position:" << x << ":" << y << endl;
	
	ofstream file ( machinecfg );
	ofstream log_file ( log );
	if ( file.is_open ()){
	
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

void linux_c::load_configuration ()
{
	string machinecfg = get_lambs_path ();
	machinecfg+="/config/machinecfg";
	ifstream file ( machinecfg );
	if ( false ){
//	if ( file.is_open ()){
		file >> image_width;
		file >> image_height;
		file >> pos_x;
		file >> pos_y;
	}
	else{
		/*
		image_width = 600;
		image_height = 400;
		pos_x = 50;
		pos_y = 10;
		*/
		image_width = 1400;
		image_height = 1000;
		pos_x = 500;
		pos_y = 10;
	}
	file.close ();
}

loop_c loop;

void linux_c::open ()
{
	int x, y;
	cout << "hi, Linux!" << endl;
	load_configuration ();	
	create_window ();
	init ();
	config_change ( image_width, image_height );	
	event_s event;

	unsigned short sym;
	bool flip = true;
	while ( flip && loop.wait_for_event ( event )){
		switch ( event.type ){
		case LL_key_pressed :
		case LL_key_released : 
			
			if ( event.param1 == 60 ){
				break;
			}
			if ( linux_x11.count ( event.param1 )>0 ){
				sym= linux_x11[event.param1];
			} 
			else {
				sym = event.param1;
			}
		
			bool pressed;
			pressed = event.param2 == 1?true:false;
			cout << "key:" << sym << ":pressed:" << pressed << endl;
			key_event ( pressed, sym, 0 );
			break;
		case LL_mouse_move :
			mouse_move ( event.param1,-event.param2 );
			cout << "mouse move" << endl;
		case LL_pointer_button_1:
		case LL_pointer_button_2:
		case LL_pointer_button_3:
		case LL_pointer_button_4:
		case LL_pointer_button_5:
			if ( event.param1 == 1 ){
				button_pressed ( event.type );
				expose_image ();
			}
		}
		if ( loop.le.empty ()){
			idle ();
		}
	}
	destroy_window ();
}

linux_c::linux_c ()
{
}

message_c* get_machine ()
{
	return &linux;
}

int entry ( int argc, char* argv[]);

int main ( int argc, char* argv[])
{
	cout << "main" << endl;
	return entry ( argc, argv );
}

