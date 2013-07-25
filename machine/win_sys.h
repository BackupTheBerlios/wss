#ifndef WIN_SYS_H
#define WIN_SYS_H


class win_sys_c : public message_c
{
	public:
	win_sys_c(){};
	~win_sys_c(){};
	virtual void walk(string,list<string>&){};
	virtual int system_echo ( string& command, stringstream& echo );
	virtual void load_configuration(){};
	virtual void save_configuration(){};
	virtual void grab_system_cursor(int *x, int *y){};
	virtual void ungrab_system_cursor(int x,int y){};
	virtual void change_pwd(string directory){};	
	virtual void open(){};
	virtual void exit ( int ) {};	
	long image_width;
	long image_height;
	int pos_x;
	int pos_y;
	void system_output(bool bopen,FILE*,std::string*,FILE**);
	virtual void get_clipboard(string&);
	virtual void set_clipboard(string);
};

#endif
