#ifndef EDIT_H
#define EDIT_H

#include "font.h"
#include "matrix.h"
#include "line.h"
#include "spline.h"
#include "regexp.h"

//Unix LF; Window CR+LF ; Apple CR :LF 0x0a (\n ), 

using namespace std;


class editor_c: public object_c
{
	public :
	bool find_word ( string&);
	string get_next_dictionary_word ( vector<texel_c*>::iterator&, vector<texel_c*>::iterator&);
	int check_spell ( int flag );	
	string mark1;
	string mark2;
	string mark3;
	void scrolling ( int, int );	
	void change_mode ( int, int );
	void editing ( int );
	void marking ( int, int );
	void moving ( int, int );
	void searching ( int, int );
	void deleting ( int, int );
	void replacing ( int, int );
	void inserting_overwriting ( int, int );
	void runing ( int, int );
	void collapsing ( int, int );
	bool is_collapsed;
	void run_search_replace ( int& flag, uint16_t v, string& c );
	void searching ( size_t* position );
	list<string> file_list;
	list<string>::iterator itfl;
	
	void export_text ( string name );
	void import_text ( string name, bool refresh = false );
	void import_webpage ( string url );
	void refresh ( string name );

	virtual void serialize ( fstream& file, bool save );
	void clear_pixel_vectors ();
	void clear_glyph_bitmap ();
	void clear_texel_map ();		
	string str;
	virtual void draw ( lamb_c& lamb );
	public:
	editor_c ();
	virtual ~editor_c ();
	
	font_c font;
	int mode;
	int overwrite;
	int com ( unsigned short, string& c, string& s );
	int command2 ( unsigned short, string&, string& );	
	int selector_index;	
	texel_caret_c* xcaret;
	texel_caret_c* get_caret ();	
	int edit ( lamb_c& lamb, keyboard_c& keyb );
	int sheck_string ( string&, unsigned char, unsigned char*);	
	int convert_to_CRLF_format ( string&);
	int convert_to_LF_format ( string&);
	vector<texel_c*> texels;
	vector<texel_c*> collapse_texels;
	list<size_t> collapse_positions;
	map<unsigned int, texel_c*> texel_map;
	string get_texels ();
	string get_texels ( size_t begin, size_t end );
	void set_texels ( string );
	void set_texels ( vector<texel_c*>&, string );
	keyboard_c keyboard;
	string file_path;
	int gravity;	
	int frame_width;
	int frame_height;
	int cell;
	int scroll_right;
	int scroll_down;
	spline_c spline;		
	void shift_page ( int begin, int end, ll::Direction direction );
	void fit_scrolls ();
	void fit_caret ();	
	void screen ();
	virtual void clear ();
	bool simple_text;
	virtual void invalide_visual ( int level );	
	int valide;
	void dump ( stringstream& ss );	
	bool local;
	static editor_c* create ();
	static unsigned long type;
	virtual unsigned long get_type ();
	void load_file_list ( string );
	reg_exp_c regexp;
	static int com_tab_initial;
};

#endif //EDIT_H

