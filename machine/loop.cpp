#include <unistd.h>
#include <termios.h>
#include <poll.h>

#include <linux/input.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <list>

#include "../X11/keysym.h"

#include "event.h"
#include "loop.h"

using namespace std;

#define TIMEOUT 10000 

int loop_c::input ( event_s& event )
{
	if(!le.empty ()){
		event = le.back ();
		le.pop_back ();
		return 1;
	}

	if ( keyb==-1 ){
		return 0;
	}
	if ( mouse==-1 ){
		close ( keyb );
		return 0;
	}
	
	struct pollfd fds[2];
	bool flip = false;
	int ret;
	/* watch keyb and mouse for input */
	fds[0].fd = keyb; 
	fds[0].events = POLLIN;
	fds[1].fd= mouse;
	fds[1].events = POLLIN;
	
	input_event ev[64];
	int event_size= sizeof ( input_event )*64;
	
	/* watch stdout for ability to write (almost always true ) */
//	fds[1].fd = STDOUT_FILENO;
//	fds[1].events = POLLOUT;
	/* All set, block! */
	ret = poll (fds, 2, TIMEOUT * 1000 );
	if (ret == -1 ) {
		perror ("poll" );
		return 0;
	}
	if (ret == 0 ) {
		cout << "pool seconds elapsed:" << TIMEOUT << endl;
		return 0;
	}
	if (fds[0].revents & POLLIN ){
		cout << "keyb is readable" << endl;
		int s = read ( keyb, ev, event_size );
			if ( s == 0 ){
			cout << "not pressed" << endl;
		}
		if ( s<0 ){
			if( errno == EAGAIN ){
//					cout << "EAGAIN:"<< errno << endl;
			}
			if( errno == EWOULDBLOCK ){
//					cout << "EWOULDBLOCK:" << errno << endl;
			}
		}
		for ( int i = 0;(i+1 )*sizeof ( input_event ) <= s; i++){
			cout << s << ":" << hex << ev[i].type <<":" << ev[i].code << ":" << ev[i].value << endl;
			if ( ev[i].type == 1 ){
				if ( ev[i].value == 1 ){
					event.type = LL_key_pressed;
				}
				else if ( ev[i].value == 0 ){
					event.type = LL_key_released;
				}
				else{
					event.type = LL_no_event;
				}
				event.param1 = ev[i].code;
				event.param2 = ev[i].value;
				le.push_front ( event );
			}
		}
	}
	if (fds[1].revents & POLLIN ){
		cout << "mouse is readable" << endl;
		int s = read ( mouse, ev, event_size );
		if ( s<0 ){
			cout << "mouse error" << endl;
			return 0;
		}
		for ( int i = 0;(i+1 )*sizeof ( input_event ) <= s; i++){
			cout <<  ev[i].type <<":" << ev[i].code << ":" << ev[i].value << endl;
			if ( ev[i].type == 2 ){
				if ( ev[i].code == 8 ){
					if ( ev[i].value == 1 ){
						event.type = LL_pointer_button_4;
						event.param1 = 0;
						event.param2 = 0;
					}
					if ( ev[i].value==-1 ){
						event.type = LL_pointer_button_5;
						event.param1 = 0;
						event.param2 = 0;
					}
				}
				else if ( ev[i].code == 0 ){
					event.type = LL_mouse_move;
					event.param1 = ev[i].value;
					event.param2 = 0;
				}
				else if ( ev[i].code == 1 ){
					event.type = LL_mouse_move;
					event.param1 = 0;
					event.param2 = ev[i].value;
				}
				else {
					continue;
				}
				le.push_front ( event );
			}
			if ( ev[i].type == 1 ){
				switch ( ev[i].code ){
					case 0x110:
						event.type = LL_pointer_button_1;
						break;
					case 0x111:
						event.type = LL_pointer_button_2;
						break;
					case 0x112:
						event.type = LL_pointer_button_3;
						break;
					default:
						continue;
				}
				event.param1 = ev[i].value;
				le.push_front ( event );
			}
		}
	}
//	if (fds[2].revents & POLLOUT ){
//		cout <<  "stdout is writable" << endl;
//	}
	tcflush ( STDIN_FILENO, TCIFLUSH );
	if(!le.empty ()){
		event = le.back ();
		le.pop_back ();
	}
	else {
		event.type = LL_no_event;
	}
	return 1;
}

bool loop_c::wait_for_event ( event_s& e )
{
	input ( e );
	return true;
}

loop_c::loop_c ():
	KEYBOARD("/dev/input/event3" ),
	MOUSE("/dev/input/event2" )
//	KEYBOARD("/dev/input/event2" ),
//	MOUSE("/dev/input/event4" )
{
	struct termios t;
	tcgetattr ( STDIN_FILENO,&saved_terminal );
	t = saved_terminal;
	t.c_lflag &= ~ECHO;
	t.c_cc[VSTOP]= fpathconf ( 0, _PC_VDISABLE );
	tcsetattr ( STDIN_FILENO, TCSANOW,&t );
	
	mouse = open ( MOUSE, O_RDONLY | O_NONBLOCK );
	keyb = open ( KEYBOARD,  O_RDONLY | O_NONBLOCK );
	
	char name[256];
	ioctl ( keyb, EVIOCGNAME (sizeof ( name )), name );
	cout << "keyboard name:" << name << endl;
	ioctl ( mouse, EVIOCGNAME (sizeof ( name )), name );
	cout << "mouse name:" << name << endl;
}

loop_c::~loop_c ()
{	
	close ( mouse );
	close ( keyb );
	tcsetattr ( STDIN_FILENO, TCSANOW,&saved_terminal );
}



