#ifndef SDL_H
#define SDL_H

class win_c: public win_sys_c
{
	public:
	win_c(){};



	void load_configuration();	
	void save_configuration();
	void open();
	void expose_image();
//	void ungrab_system_cursor(int x, int y);
//	void grab_system_cursor(int* x, int* y);	
//	virtual void walk(string,list<string>&);
//	virtual void change_pwd(string);
	virtual void exit ( int );
//	virtual int nop_system(int);	
	virtual bool is_idle();
//	virtual bool download(string&,stringstream&);
	virtual void get_clipboard(string&);
	virtual void set_clipboard(string);
//	virtual void window_management(string);
	int flag;
	int terminated;
	HINSTANCE hInstance; 
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int nCmdShow;

	WNDCLASSEX wc;
	HWND hWnd;
	MSG Msg;


};

#endif
