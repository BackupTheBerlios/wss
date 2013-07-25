#ifndef LINUX_H
#define LINUX_H

class linux_c: public machine_c
{
	public:
	linux_c ();
	void open ();
	void load_configuration ();
	void save_configuration ();
	int create_window ();
	void destroy_window ();
	bool is_idle ();
	void expose_image ();
	void* fb;
	struct fb_fix_screeninfo FixInfo;
	struct fb_var_screeninfo VarInfo;
	int m_FBFD;
	int fb_size;
	bool input ( event_s&);
	void exit ( int );
	bool download ( string&, stringstream&);
	uint32_t bits_per_pixel;

};

#endif
