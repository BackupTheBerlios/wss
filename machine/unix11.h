#ifndef UNIX11_H
#define UNIX11_H

class x11_c: public machine_c
{
	public:
	x11_c ();
	~x11_c ();
	virtual void set_clipboard ( string );
	virtual void get_clipboard ( string&);
	Display* display;
	int screen;
	int depth;
	Visual* visual;
	Window win;
	GC copyGC;
	void expose_image ();
	void open ();
	Window create_window ( int x, int y, int width, int height );
	virtual void exit ( int );
	virtual bool is_idle ();
	void hide_system_cursor ();
	void show_system_cursor ();
	void grab_system_cursor ( int* x, int *y );
	void ungrab_system_cursor ( int x, int y );	
	
	bool system_cursor_visible;
	bool system_cursor_grabed;
	
	void save_configuration ();
	void load_configuration ();
	int windowing_mode;
	Atom pseudo_motion;
	XClientMessageEvent xcme;
};
#endif
