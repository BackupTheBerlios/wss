#ifndef SDL_H
#define SDL_H

class sdl_c: public posix_sys_c
{
	public:
	sdl_c ();
	void load_configuration ();	
	void save_configuration ();
	void open ();
	void expose_image ();
	void ungrab_system_cursor ( int x, int y );
	void grab_system_cursor ( int* x, int* y );	
	virtual void walk ( string, list<string>&);
	virtual void change_pwd ( string );
	virtual void exit ( int );
	virtual int nop_system ( int );	
	virtual bool is_idle ();
	virtual bool download ( string&, stringstream&);
	virtual void get_clipboard ( string&);
	virtual void set_clipboard ( string );
	virtual void window_management ( string );
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *surface;
	int flag;
};

#endif
