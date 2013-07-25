#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <string>

#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"

using namespace std;

void message_c::walk(string s,list<string>& l)
{
	that->walk(s,l);
} 

int message_c::system_echo ( string& command, stringstream& echo )
{
	that -> system_echo ( command, echo );
}

void message_c::change_pwd(string directory)
{
	that->change_pwd(directory);
}

int message_c::nop_system(int x)
{
	return that->nop_system(x);
}

void message_c::open()
{
	that->open();	
}

void message_c::mouse_move(int x,int y)
{
	that->mouse_move(x,y);
}

void message_c::idle()
{
	that->idle();
}

void message_c::key_event(bool pressed,unsigned short us, unsigned char uc)
{
	that->key_event(pressed,us,uc);
}

void message_c::exit(int code)
{
	that->exit(code);
}

bool message_c::is_idle()
{
	return that->is_idle();
}

void message_c::expose_image()
{
	that->expose_image();
}

bool message_c::download(string& url,stringstream& result)
{
	return that->download(url, result);
}

void message_c::ungrab_system_cursor(int x, int y)
{
	that->ungrab_system_cursor(x,y);
}

void message_c::init()
{
	that->init();
}

void message_c::expose(int* px, int* py, char ** pcp, retine_c** retine)
{
	that->expose(px,py,pcp,retine);
}

void message_c::config_change(int x,int y)
{
	that->config_change(x,y);
}
		
void message_c::focus()
{
	that->focus();
}

void message_c::grab_pointer(int x, int y)
{
	that->grab_pointer(x,y);
}

void message_c::button_pressed(int x)
{
	that->button_pressed(x);
}

void message_c::get_clipboard(string& s)
{
	that->get_clipboard(s);
}

void message_c::set_clipboard(string s)
{
	that->set_clipboard(s);
}

void message_c::file_service ( stringstream& ss )
{
	that -> file_service ( ss );
}

void message_c::window_management(string s)
{
	that->window_management(s);
}				