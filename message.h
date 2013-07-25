#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <list>

using namespace std;

class message_c;

message_c* get_machine ();

class message_c
{	
	public:
	virtual int  nop_system ( int );
	virtual int system_echo ( string& command, stringstream& echo );
	virtual void walk ( string, list<string>&);
	virtual void change_pwd ( string directory );
	virtual void open ();
	virtual void mouse_move ( int, int );
	virtual void idle ();
	virtual void key_event ( bool, unsigned short, unsigned char );
	virtual void exit ( int );
	virtual bool is_idle ();
	virtual void expose_image ();
	virtual bool download ( string&, stringstream&);
	virtual void ungrab_system_cursor ( int, int );
	virtual void init ();
	virtual void expose ( int*, int*, char**, retine_c**);
	virtual void focus ();
	virtual void grab_pointer ( int, int );
	virtual void button_pressed ( int );
	virtual void config_change ( int, int );
	virtual void get_clipboard ( string&);
	virtual void set_clipboard ( string );	
	virtual void window_management ( string );
	virtual void file_service ( stringstream& ss );
	message_c* that;
};

#endif //MESSAGE_H
