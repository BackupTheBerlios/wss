#ifndef POSIX_SYS_H
#define POSIX_SYS_H


class posix_sys_c : public message_c
{
	public:
	posix_sys_c(){};
	~posix_sys_c(){};
	virtual void walk(string,list<string>&){};
	virtual void load_configuration(){};
	virtual void save_configuration(){};
	virtual void grab_system_cursor(int *x, int *y){};
	virtual void ungrab_system_cursor(int x,int y){};
	virtual void change_pwd(string directory){};	
	virtual void open(){};
	long image_width;
	long image_height;
	int pos_x;
	int pos_y;
	int system_echo ( string& com , stringstream& echo );
	virtual void get_clipboard(string&);
	virtual void set_clipboard(string);
	virtual void file_service ( stringstream& ss );
};

#endif
