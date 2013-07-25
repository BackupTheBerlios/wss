#ifndef LLOOP_H
#define LLOOP_H

using namespace std;

struct event_s
{
	int type;
	int param1;
	int param2; 
};


class loop_c
{
	public:
	loop_c ();
	~loop_c ();
	
	int input ( event_s&);
	bool wait_for_event ( event_s&);
	list<event_s> le;	
	int mouse;
	int keyb;
	char* KEYBOARD;
	char* MOUSE;
	struct termios saved_terminal;

};


#endif