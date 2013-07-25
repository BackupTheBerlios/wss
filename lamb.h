#ifndef  LAMB_H
#define  LAMB_H

#define INFINITE 1000000

using namespace std;


enum symbols{
	NONE,
	REVERT,
	STATIC,
	NORTH,
	NORTH_WEST,
	WEST_NORTH,
	WEST,
	WEST_SOUTH,
	SOUTH_WEST,
	SOUTH,
	SOUTH_EAST,
	EAST_SOUTH,
	EAST,
	EAST_NORTH,
	NORTH_EAST
};


enum figures{
	SPLINE,
	CARTESIAN,
	LINE,
	FRAME,
	EDITOR,
	CIRCLE,
	LAMB,
	IMAGE
};

enum movements{
//	FROZE,
	_SHIFT,
	_ROTATE,
	_SHIFT_OBJECT,
	_ROTATE_OBJECT,
	_POINT
};
	
enum semantic{
	SPEAK,
	WRITE,
	SILENT
};

enum edit_modes{
	_INSERT,
	_COMMAND,
	_VISUAL
};

enum directions{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD
};

enum execute_environment{
	COMPILE_SNOW,
	CLEAN_SNOW,
	OTHER_PROJECT
};

#include <typeinfo>
#include<tuple>
#include "comtab.h"
class land_c;

class eyes_c;
class object_c;
class editor_c;
class mouse_c;
class lamb_c;

template < typename T>
class command_table_c;

class lamb_c: public object_c
{
	public: 
	lamb_c ();
	~lamb_c ();
	matrix_c<FT> mx;
	matrix_c<FT> local_position;
	matrix_c<FT> pointer;
	void local ( object_c&, matrix_c<FT> vA, matrix_c<FT> vb, matrix_c<FT> vx );	
	matrix_c<FT> view_to_world ( matrix_c<FT>& point );
	matrix_c<FT> world_to_view ( matrix_c<FT>& point );	
	matrix_c<FT> view_to_object ( object_c&, matrix_c<FT>&);
	matrix_c<FT> view_from_object ( object_c&, matrix_c<FT>&);
	virtual void coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag);	mouse_c mouse;
	void draw ( lamb_c& );
	void rotatesel ( int lr, int du, int zlr );
	void rotate ( int lr, int du, int zlr );
	void shift_selection ( int bf, int lr, int du );
	void copy_selection ();
	void shift ( int bf, int lr, int du );
	void point ( int lr, int du, int bf );  
	virtual void move ( int type, int x, int y, int z );
	void write ( bool is_pressed, uint16_t stroke );
	void change_movement ( int para1, int para2 );
	virtual int command2 ( unsigned char, string& , string&);
	bool command ( string str );
	void compile_and_restart ( bool clean = false );
	int command_number;
	keyboard_c keyboard;
	int movement;
	int shift_rotate;
	int keyboard_mouse;
	bool verbal;
	string directory;
	string project;
	string start_command;
	bool is_cursor_captured;
	string pathlamb;
	list<string> ed_mem;
	int return_code;
	static unsigned long type;
	virtual unsigned long get_type (){return type;};
	void grab_pointer ();
	void release_pointer ();
	int service ( string& s1, string& s2, string& s3 );
	virtual void mouse_move ( int, int );
	virtual void idle ();
	void write_status ();	
	virtual void key_event ( bool, unsigned short, unsigned char );
	void to_default ( int, string );
	virtual void init ();
	virtual void expose ( int*, int*, char**, retine_c**);
	virtual void focus ();
	int edit ( lamb_c&, keyboard_c&);
	virtual void grab_pointer ( int, int );
	virtual void button_pressed ( int );
	virtual void config_change ( int, int );
	void resize_fonts ( float f );
	virtual void matrices_to_view ( object_c& object, matrix_c<FT>* base, matrix_c<FT>* translation );
	void shift ( object_c*, int, int, int );
	void shift3 ( matrix_c<FT>&);
	object_c* sensor;
	object_c* motor;
	object_c* hook;
	editor_c mouth;
	editor_c status;
	editor_c ears;
	eyes_c eyes;
	bool is_printing;
	string home;
	string grf_file;
	list<object_c*>motors;
	list<object_c*>sensors;
	list<object_c*>::iterator next_object ( list<object_c*>&, object_c*&, int );
	list<object_c*>::iterator previous_object ( list<object_c*>&, object_c*&, int );
	void test ( int para1, int para2 );	
	void test ( int par1, string s );
	void test2 ( int para1 );
	void test3 ( object_c* po, int para1, int para2, int para3 );
	void test4 ( int d, int par1, string s );
	window_manager_c window_manager;
	spline_c border;
};
#endif
