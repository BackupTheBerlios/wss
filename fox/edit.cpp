#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>

#include <boost/regex.hpp>
#include <boost/iterator.hpp>
#include <iterator>

#include "standard.h"

#include "symbol/keysym.h"

#include "library/wdslib.h"

#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"

#include "regexp.h"

#include "global.h"
#include "object.h"
#include "keyb.h"
#include "font.h"
#include "line.h"
#include "retine.h"
#include "texel.h"

#include "edit.h"

#include "land.h"
#include "retine.h"
#include "mouse.h"
#include "eyes.h"
#include "home.h"
#include "lamb.h"
#include "comtab.h"

#include "bookmarks.h"
#include "image.h"
#include "make.h"

extern land_c land;
extern image_c image;
extern int figure;

unsigned long editor_c::type = EDITOR;

using namespace std;

long get_utf32 ( string& s, int& c );
int byte_index ( string& str, int index );


int editor_c::com_tab_initial = 0;

namespace command_table {
void construct_ct (lamb_c& lamb );
}

string clip_string;

void editor_c::clear_glyph_bitmap ()
{
	map<unsigned int, texel_c*>::iterator it;
	it = texel_map.begin ();
	for (;it!= texel_map.end ();it++){
		texel_c* tex = it -> second;
		if ( tex == 0 ) {
			continue;
		}
		tex->get_glyph ().isEmpty = true;
		tex->get_glyph ().color_pixels.clear ();
		tex->get_glyph ().bitmapGlyph = 0;
	}
}

void editor_c::clear_pixel_vectors ()
{
	map<unsigned int, texel_c*>::iterator it;
	it = texel_map.begin ();
	for (;it!= texel_map.end ();it++){
		texel_c* tex = it->second;
		if ( tex == 0 ) {
			continue;
		}
		tex->get_glyph ().isEmpty = true;
		tex->get_glyph ().color_pixels.clear ();
	}
}

void editor_c::clear_texel_map ()
{
	texel_caret_c* tc = get_caret ();
	int position = tc->get_position ();

	string s = get_texels ();
	clear_pixel_vectors ();
	clear_glyph_bitmap ();
	texels.clear ();
	set_texels ( s );
	tc->move_to ( position );
}

unsigned long editor_c::get_type ()
{
	return type;
}

bool editor_c::find_word ( string& s )
{
	if ( s == "" ){
		 return false;
	}
	static bool initialize = true;
	static unordered_set<string> my_set;
	string lambs_path = get_lambs_path ();
	string fn;
	string str;
	if ( initialize ){
		fn = lambs_path+"/dictio2";
		ifstream file ( fn );
		string str;
		while ( file.good ()){
			file >> str;
			my_set.insert ( str );
			initialize = false;
		}
	}
	if ( my_set.find ( s ) == my_set.end ()){
		if ( islower ( s.at ( 0 ))){
			return false;
		}
		s [0] = tolower ( s[0] );
		if ( my_set.find ( s ) == my_set.end ()){
			return false;
		}
	}
	return true;
}

int editor_c::check_spell ( int flag )
{
	if ( flag == 0 ){
		echo << "spell check" << endl;
		vector<texel_c*>::iterator itb, ite;
		string s;
		itb = ite = texels.begin ();
		for (;ite!= texels.end ();){
			s = get_next_dictionary_word ( itb, ite );
			if  ( !find_word ( s )){
				texel_c* ci = texel_c::construct ('\2', texel_map );
				texels.insert ( itb, ci );
				ite++;
				texel_c* co = texel_c::construct ('\3', texel_map );		
				if ( ite!= texels.end ()){
					texels.insert ( ite, co );
				}
			}
		}
		cout << "ch" << endl;
	}
	else if ( flag == 2 ){
		vector<texel_c*>::iterator it;
		it = texels.begin ();
		for (;it!= texels.end ();it++){
			if ((*it )->get_char () == 2 || (*it )->get_char () == 3 ){
				texels.erase ( it );
			}
		}			
	}
}

string editor_c::get_next_dictionary_word ( vector<texel_c*>::iterator& itb, vector<texel_c*>::iterator& ite )
{
	string s;
	itb = ite;
	uint32_t ch = 0;
	for (;ite!= texels.end ();ite++){
		ch=(*ite )->get_char ();
		if ( isalpha ( ch )){
			itb = ite;
			break;
		}
	}
	for (;ite!= texels.end ();ite++){
		ch=(*ite )->get_char ();
		if  ( !isalpha ( ch ) && ch!='-'){
			break;
		}
		s+= ch;
	}
	return s;
}

void editor_c::dump ( std::stringstream& ss )
{
	ss << "frame width: " << frame_width << endl;
	ss << "frame height: " << frame_height << endl;
	ss << "simple text: " << simple_text << endl;
	ss << "file path: " << file_path << endl;
}

int position_index ( string& str, int index )
{
	string::iterator it = str.begin ();
	int count = index;
	
	for (;it!= str.end ();it++){
		char c=*it;
		if ( c&0x80 ){
			if ((c&0xe0 ) == 0xc0 ){
				it++;
				count--;
				index--;
			}
			else if ((c&0xf0 ) == 0xe0 ){
				cout << "xxxx" << endl;
				it+= 2;
				count-= 2;
				index-= 2;
			}
			
			else if ((c&0xf8 ) == 0xf0 ){
				it+= 3;
				count-= 3;
				index-= 3;
			}
		}
		count--;
		if ( count == 0 ){
			break;
		}
	}
	return index;
}

int byte_index ( string& str, int index )
{
	string::iterator it = str.begin ();
	int count = index;
	for (; it!= str.end (); it++){
		count--;
		if ( count == 0 ){
			break;
		}
		uint8_t c=*it;
		if ( (c & 0x80 ) == 0 ){
			continue;
		}
		while ( c & 0x40 ){
			it++;
			index++;
			c = c << 1;
		}
	}
	return index;
}

enum {
	NEW_LINE_U,
	NEW_LINE_D,
	COPY_LINE,
	DELETE_LINE,
	CUT_LINE,
	NEXT_WORD_BEG,
	PREV_WORD_BEG,
	NEXT_WORD_END,
	LINE_BEG,
	LINE_END,
	PAGE_D,
	PAGE_U,
	LINE_D,
	LINE_U,
	HOME,
	END,
	PASTE,
	CUT,
	DELETE,
	COPY,
	CHANGE,
	CUT_WORD,
	INC_COLUMN,
	DEC_COLUMN,
	INC_LINE,
	DEC_LINE,
	INC_ROW,
	DEC_ROW,
	SEL,
	CHARACTER,
	SPACE,
	TAB,
	MARK,
	GO_TO_MARK

};


editor_c* editor_c::create ()
{
    return new editor_c ();
}

void editor_c::serialize ( fstream &f, bool save ){
	if  ( !simple_text ){
		return;
	}	
	if ( save ){
		f << "EDITOR" << endl;
	}
	motion.serialize ( f, save );
	if ( save == false ){
		f >> simple_text;
		f >> frame_width;
		f >> frame_height;
		f >> gravity;
		f >> scroll_down;
		f >> scroll_right;
		string s;
		getline ( f, s );
		getline ( f, s );
		if ( s == "" ){
			getline ( f, s );
			set_texels ( s );
		}
		else {
			file_path = s;
			import_text ( s, true );
		}
	}
	else{
		f << simple_text << endl;
		f << frame_width << endl;
		f << frame_height << endl;
		f << gravity << endl;
		f << scroll_down << endl;
		f << scroll_right << endl;
		string s = file_path;
		f << s << endl;
		if ( s== "" ){
			f << get_texels () << endl;
		}
		else	{
			export_text ( s );
		}
	}
}
 
void editor_c::invalide_visual ( int level )
{
	valide = level;
}

editor_c::editor_c ():
	frame_width ( 40 ),
	frame_height ( 35 ),
	scroll_down ( 0 ),
	scroll_right ( 0 ),
	selector_index ( 0 ),
	is_collapsed ( false ),
	overwrite ( 0 ),
	simple_text ( false ),
	valide ( 0 ),
	gravity ( STATIC )
{
	set_texels ("" );
	texel_caret_c* pc = get_caret ();
	pc->texels=&texels;
	pc->editor = this;
	mode = _COMMAND;
	type = EDITOR;
	load_file_list ( "config/file_list" );
}

void editor_c::load_file_list ( string path )
{
	if ( path == "" ){
		file_list.clear ();
		return;
	}
	string fn = get_lambs_path () + "/" + path;
	ifstream fin;
	fin.open ( fn );
	if ( fin.fail ()){
//		echo << "no such a file_list:" << fn << endl;
		return;
	}
	file_list.clear ();
	stringstream ss;
	ss << fin.rdbuf ();
	while ( ss.good ()){
		string nn;
		ss >> nn;
		file_list.push_back ( nn );
	}
}

void editor_c::change_mode ( int para1, int para2 )
{
	lamb_c& lamb = *land.get_lamb ();
	keyboard.stroke_list.clear ();
	if ( para1 == 0 ){
		mode = para2;
	}
	else if ( para1 == 2 ){
		lamb.mouth.mode = _INSERT;
	}
	else if ( para1 == 3 ){
		if ( lamb.sensor->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)lamb.sensor;
			ed->export_text ( ed->file_path );
		}
		if ( para2 == 0 ){
//			lamb.sensor = land.previous_object ( lamb.sensor );
		}
		else{
//			lamb.sensor = land.next_object ( lamb.sensor );
		}
		if ( lamb.sensor->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)lamb.sensor;			
			ed->import_text ( ed->file_path, true );
		}
	}
	else if ( para1 == 4 ){
		state = state == 0?1:0;
	}
}

int find_next_bracket ( vector<texel_c*>& texels, vector<texel_c*>::iterator& it, size_t& size, bool skeep_return )
{
	int result = 0;
	size = 0;
	vector<texel_c*>::iterator it2;
	int comment = 0;
	for (;it!= texels.end ();it++){
		unsigned long ch=(*it )->get_char ();
		if ( comment == 0 ){
			if ( ch == '/'){
				comment = 1;
			}
		}
		else if (comment == 1 ){
			if ( ch == '/'){
				comment = 2;
			}
			else if ( ch == '*'){
				comment = 4;
			}
			else {
				comment = 0;
			}
		}
		else if ( comment == 2 ){
			if ( ch == '\n'){
				comment = 0;
			}
		}
		else if (comment == 3 ){
			if ( ch == '/'){
				comment = 0;
			}
			else {
				comment = 4;
			}
		}
		else if (comment == 4 ){
			if ( ch == '*'){
				comment = 3;
			}
		}
		if ( comment == 0 && ch=='{'){
			it2 = it-1;
			if ( it!= texels.begin () && (*it2 )->get_char () =='\n'){
				size = 2;
				it = it2;
			}
			else{
				size = 1;
			}
			result = 1;
			break;
		}
		if ( comment == 00 && ch=='}'){
			it2 = it+1;
			if ( it2!= texels.end () && (*it2 )->get_char () =='\n'){
				size = 2;
			}
			else{
				size = 1;
			}					
			result=-1;
			break;
		}
	}
	return result;
}

void editor_c::collapsing ( int para1, int para2 )
{
	static list<pair<size_t, size_t>> collapse_list;
	texel_caret_c* tc = get_caret ();
	if ( !is_collapsed ){
		is_collapsed = true;
		collapse_list.clear ();
		collapse_texels.clear ();
		
		size_t position;
		size_t row;
		
		row = tc->get_row ( 0,&position, 0 );
		
		tc->move_to ( 1 );
		
		vector<texel_c*>::iterator it, itb;
		it = itb = texels.begin ();
		int bracket = 0, prev_bracket = 0;
		pair<size_t, size_t> collapse_block;
		size_t size;
		for (;it!= texels.end ();){
			prev_bracket = bracket;
			bracket+= find_next_bracket ( texels, it, size, false );
			if ((it == texels.end () && bracket == 0 )  || 
			(bracket == 1 && prev_bracket == 0 )){
				if ( itb!= texels.begin ()){
					collapse_block.second = tc->get_position ( itb-1 );
					collapse_list.push_back ( collapse_block );
				}
				for (;it!= itb;itb++){
					collapse_texels.push_back (*itb );
				}
				collapse_block.first = tc->get_position ( it );
			}
			for (;size>0;--size ){
				it++;
			}
			if ((it == texels.end () && bracket!= 0 ) ||
			(bracket == 0 )){
				itb = it;
			}
		}
		list<pair<size_t, size_t>>::iterator itl;
		itl = collapse_list.begin ();
		size_t pos = position;
		for (;itl!= collapse_list.end ();itl++){
			if ( itl->second > position ){
				if ( itl->first < position ){
					pos-= position-itl->first;
				}
				break;
			}
			pos-= itl->second-itl->first+1;
		}
		vector<texel_c*> texba;
		texba = texels;
		texels = collapse_texels;
		collapse_texels = texba;
		tc->move_to ( pos );
		selector_index = pos;
		size_t row2 = tc->get_row ( 0, 0, 0 );
		scroll_down = max ((size_t )0, row2-(row-scroll_down ));
//		dump_collapse ( collapse_list );
	}
	else{
		is_collapsed = false;
		size_t row, pos;
		row = tc->get_row ( 0,&pos, 0 );
		list<pair<size_t, size_t>>::iterator itl;
		itl = collapse_list.begin ();
		for (;itl!= collapse_list.end ();itl++){
			if ( itl->first >pos ){
				break;
			}
			pos+= itl->second-itl->first+1;
		}
		texels = collapse_texels;
		tc->move_to ( pos );
		selector_index = pos;
		size_t row2 = tc->get_row ( 0, 0, 0 );
		scroll_down = max ((size_t )0, row2-(row-scroll_down ));
	}
}

bool flag_ok;
void editor_c::runing ( int para1, int para2 )
{
	lamb_c& lamb = *land.get_lamb ();
	if ( para1 == 0 ){
		int& cn = lamb.command_number;	
		cn = cn<OTHER_PROJECT?++cn:COMPILE_SNOW;
		return;
	}
	export_text ( file_path );
	string lambs_path = get_lambs_path ();
	lambs_path+="/";
	stringstream ss;
	if ( para1 == 1 ){
		switch ( lamb.command_number ){
		case COMPILE_SNOW:
			lamb.compile_and_restart ( false );
			return;
		case CLEAN_SNOW:
			lamb.compile_and_restart ( true );		
			cout << ss;
			return;
		case OTHER_PROJECT:
			cout << "other project" << endl;
			string s = lamb.project;
			string command;
			size_t sl = s.size ();
			cout << "next step" << endl;

			if ( s.substr ( sl-4 ) ==".tex" ){
				cout << "next step" << endl;
				command="pdflatex -interaction = nonstopmode " + lamb.project;
				lamb.system_echo ( command, ss );
				string s1 = s.substr ( 0, sl-4 );
				stringstream ss1;
//				command= "convert " + s1 + ".pdf " + s1 + ".png";
//				lamb.system_echo ( command, ss1 );
			}
			else if ( s.substr ( sl-3 ) ==".ly" ){
				command="lilypond stranger2 >./landout";
				lamb.system_echo ( command, ss );
			}
			echo << command <<endl;
			string s1, s2;
			bool bmv = true;
			int ddeep = 0;
			ss.clear ();
			string str;
			while ( ss.good ()){
				getline ( ss, s1,'\n');
				echo << s1 << endl;
				if ( string::npos!= s1.find ("!" )){
					bmv = false;
					ddeep = 3;
				}
				if ( ddeep!= 0 ){
					str+= s1+"\n";
					ddeep--;
				}
				s1="";
			}
			if ( bmv ){
				echo << "ok" << endl;
				str+="OK";
				flag_ok = true;
			}
			else{
				echo << "error" << endl;
			}
			echo << str << endl;
			return;
		}
	}
}

void editor_c::searching ( size_t* position )
{
	size_t pos = *position;
	string s = get_texels ();
	size_t inz = byte_index ( s, pos );
	string found;
	if  ( regexp.find_section ( s, inz, found ) ) {
		texel_caret_c* tc = get_caret ();
		pos = position_index ( s, inz );
		tc -> move_to ( pos + 1 + regexp.look_before );
		selector_index = pos + found.size () + 2 + regexp.look_after;
		int r = tc->get_row ( 0, 0, 0 );
		scroll_down = max ( 0, r-10 );
		*position = pos+2;
		return;
	}
	*position = string::npos;
	return;
}

void editor_c::run_search_replace ( int& flag, uint16_t v, string& c )
{
	lamb_c& lamb = *land.get_lamb ();
	static size_t pos = 0;
	static string stro;
	static size_t pos_o;
	static size_t scroll_o;
	static string mem_file;
	bool automatic = false;
	if ( flag == -1 ) {
		automatic = true;
		flag = 0;
	}
	if ( flag == 0 ){
		export_text ( file_path );
		echo << "run search replace" << endl;
		clip_string = regexp.replace;
		itfl = file_list.begin ();
		echo << "file_list size " << file_list.size () << endl;
		flag = 3;
		c = "n";
		pos = 0;
		stro = file_path;
		pos = pos_o = get_caret ()->get_position ();
		scroll_o = scroll_down;
		mode = 5;
	}
	do {
		if ( v == XK_Escape ){
			export_text ( file_path );
			file_path = stro;
			import_text ( stro );
			scroll_down = scroll_o;
			get_caret ()->move_to ( pos_o );
			selector_index = pos_o;
			land.impress ( lamb );
			mode = _COMMAND;
			flag = 0;		
		}
		else if ( c == "n" || c == "y" || c == "a" ){
			static bool is_changed = false;
			if ( pos!= string::npos && pos!= 0 ){
				if ( c == "y" || c == "a" ){
					selector_index--;
					editing ( CHANGE );
					is_changed = true;
					if ( c == "a" ) {
						automatic = true;
					}
				}
			}
			while ( true ){
				if ( pos == string::npos || pos == 0 ){
					if ( is_changed ){
						echo << "exporting" << endl;
						echo << file_path << endl;
						export_text ( file_path );
						is_changed = false;
					}
					if ( itfl == file_list.end ()){
						file_path = stro;
						import_text ( stro );
						scroll_down = scroll_o;
						get_caret ()->move_to ( pos_o );
						selector_index = pos_o;
						land.impress ( lamb );
						mode = _COMMAND;															flag = 0;		
						automatic = false;
						break;
					}
					string fn = *itfl;
					if ( fn == stro ){
						itfl++;
						continue;
					}
					file_path = fn;
					echo << fn << endl;
					import_text ( fn );
					itfl++;
					pos = 1;
					if ( ! automatic ) {
						land.impress ( lamb );
					}
				}
				searching ( &pos );
				if ( flag == 2 && pos >= pos_o ){
					flag = 1;
					pos = string::npos;
					continue;
				}
				if ( flag == 3 && pos == string::npos ){
					flag = 2;
					pos = 1;
					continue;
				}
				if ( pos != string::npos ) {
					if ( ! automatic ) {
						land.impress ( lamb );
					}
					break;
				}
			}
		}
	} while ( automatic );
}

int editor_c::command2 ( unsigned short v, string& c, string& st )
{
	lamb_c& lamb = *land.get_lamb ();
	static int flag = 0;
	string s, s1, s2, s3;
	stringstream ss ( st );
	ss >> s;
	if ( s == ":s" || flag > 0 ){
		if ( flag == 0 ) {
			lamb.motors.remove (&lamb );
			lamb.motors.push_front (&lamb );
			lamb.motor = &lamb;
			ss >> s1;
			if ( s1 != "" ){
				regexp.search = s1;
				char dum;
				ss >> noskipws >> dum;
				getline ( ss, regexp.replace );
			}
		}
		run_search_replace ( flag, v, c );
		return 1;
	}
	if ( flag == 0 ){
		if ( v == XK_Return ){
			if ( st.substr ( 0, 1 ) =="." ){
				string str = st.substr ( 1 );
				echo << st << endl;
				stringstream sss;
				lamb.system_echo ( str, sss );
				echo << sss.str () << endl;
			}
			else if (st.substr ( 0, 1 )!=":" ){
					lamb.command ( st );
			}
			if ( s == ":rx" ) {
				regexp.command ( ss );
			}
			else if ( s == ":q" ){
				echo << "quit" << endl;
				cout << "quit\n";
				stringstream sss;
				sss << "comed " << file_path << " " << scroll_down <<  " " <<get_caret ()->get_position ();
				lamb.start_command = sss.str ();
				lamb.exit ( 0 );
			}
			else if ( s == ":new" ) {
				clear ();				
				file_path = "";
			}
			else if ( s == ":w" ){
				export_text ( file_path );
				land.impress ( lamb );
			}
			else if ( s == ":r" ){
				import_text ( file_path );
				land.impress ( lamb );
			}
			else if ( s == ":e" ){
				ss >> s1;
				file_path = s1;
				import_text ( s1 );
				valide = 1;
				lamb.motors.remove (&lamb );
				lamb.motors.push_front (&lamb );
				lamb.motor = &lamb;
				mode = _COMMAND;
			}
			else if ( s == ":we" ){
				ss >> s1;
				export_text ( file_path );
				file_path = s1;
				import_text ( s1 );
				mode = _COMMAND;
			}
			else if ( s == ":u" ){
				import_webpage ( s1 );
				mode = _COMMAND;
			}
			else if ( s == ":es" ){
				ss >> s1;
				export_text ( file_path );
				string lambs_path = get_lambs_path ();
				lambs_path+="/";
				file_path = lambs_path+s1;
				import_text ( file_path );
				mode = _COMMAND;
			}
			else if ( s == ":h0" ){
				lamb.mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
				lamb.pointer = {0, 0, 0};
				lamb.local_position = {0, 0, 0};
			}
			else if ( s == ":h1" ){
				lamb.mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
				lamb.pointer = {0, 0, 0};
				lamb.local_position = {-310, 190, 0};
			}
			else if ( s == ":sc0" ){
				check_spell ( 0 );
			}
			else if ( s == ":sc1" ){
				check_spell ( 1 );
			}
			else if ( s == ":sc2" ){
				check_spell ( 2 );
			}
			else if ( s == ":proper" ) {
				ss >> s1 >> s2;
				static make_c make;
				make.proper ( 1, s1, s2 );
//				make.proper ( 0, s1, s2 );
				echo << "proper" << endl;
			}
			else if ( s == ":mc" ){
				ss >> s1 >> s2;
				string s="make";
				lamb.service ( s, s1, s2 );
			}
			else if ( s == ":md" ){
				ss >> s1 >> s2;
				string s="mkproper";
				lamb.service ( s, s1, s2 );
			}
			else if ( s == ":cbs" ){
				lamb.set_clipboard ( clip_string );
			}
			else if ( s == ":cbg" ){
				lamb.get_clipboard ( clip_string );
			}
			else if ( s == ":ima" ){
				image_c image;
				image.draw ( lamb );
			}
			else if ( s == ":lp" ){
				system ("lp -d HP_LaserJet_1100 -P 2 /home/henry/shelf/cpp/lamb/doc/natpro.pdf" );
			}
			else if ( s == ":lp1" ){
				system ("lp -d HP_LaserJet_1100 -P 8 /home/henry/shelf/newvinland/mathoflife/mathoflife.pdf" );
			}
			else if ( s == ":conv" ){
				ss >> s1;
				if ( s1 == "" ){
					image.convert ( lamb.project );
				}
				else {
					image.convert ( s1 );
				}
			}
			else if ( s == ":conv2" ){
				ss >> s1;
				image.convert_png_to_ima ( s1 );
			}
			else if ( s == ":p" ){
//!!!				image.page= stoi ( s1 );
			}
			else if ( s == ":view" ){
				ss >> s1;
				image.file_name = s1;
			}
			else if ( s == ":wm" ){
				ss >> s1;
				lamb.window_management ( s1 );
			}
			else if ( s == ":ss" ){
				unsigned char ch;
				string s = get_texels ();
				sheck_string ( s, 0,&ch );
			}
			else if ( s == ":fn" ){
				ss >> s1;
				file_path = s1;
			}
			else if ( s == ":dump" ){
				stringstream sss;			
				dump ( sss );
				echo << ss.str () << endl;
			}
			else if ( s == ":res" ) {
				make_c make;
				make.remove_experimental_section ("../td" );
			}
			else if ( s == ":lfl" ) {
				ss >> s1;
				load_file_list ( s1 );				
				echo << "load file list " << endl;
			}
			else if ( s == ":kbl" ) {
				lamb_c& lamb = *land.get_lamb ();
				ss >> s1;
				lamb.keyboard.set_layout ( s1 ); 
			}
		}
	}
	return 0;
}

void editor_c::inserting_overwriting ( int, int )
{
	overwrite = 1;
	mode = 2;
}

void editor_c::deleting ( int para1, int para2 )
{
	if ( para1 == CHARACTER){
		selector_index++;	
		editing ( DELETE );
	}
	else if ( para1 == SPACE){
		bool flag = false;
		texel_caret_c* tc = get_caret ();
		size_t pos_e = tc->get_position ();
		size_t pos = 1;
		vector<texel_c*>::iterator it;
		it = tc->texels->begin ();
		int c = 1;
		for (;it!= tc->texels->end ();it++, c++){
			unsigned long ch=(*it )->get_char ();
			if ( ch=='\n'){
				flag = true;
			}
			else if ( flag == true ){
				flag = false;
				if ( ch == ' ' || ch == '\t' ){
					if ( c>= selector_index && c < pos_e ){
						it = tc->texels->erase ( it );
						it --;
					}
				}
			}
		}
	}
	else if ( para1 == TAB ){
		bool flag = false;
		texel_caret_c* tc = get_caret ();
		size_t pos_e = tc->get_position ();
		size_t pos = 1;
		std::vector<texel_c*>::iterator it;
		it = tc->texels->begin ();
		int c = 1;
		for (; it != tc->texels->end (); it++, c++){
			unsigned long ch=(*it )->get_char ();
			if ( ch=='\n'){
				flag = true;
			}
			else if ( flag == true ){
				flag = false;
				if ( c >= selector_index && c <= pos_e ){
					texel_c* p = texel_c::construct ('\t', texel_map );
					tc->texels->insert ( it, p );
					c--;
				}
			}
		}
	}
}

void editor_c::editing ( int para )
{
	texel_caret_c* tc = get_caret ();
	string str = get_texels ();
	int position = tc->get_position ();
	int index = byte_index ( str, position );
	if ( para == COPY ){
			int cselectorIndex = byte_index ( str, selector_index );
			int bx = min ( cselectorIndex, index )-1;
			int ex = max ( cselectorIndex, index )-1;
			clip_string = str.substr ( bx, ex-bx );
		}
	else if ( para == CUT ){
			int cselectorIndex = byte_index ( str, selector_index );
			int bx = min ( cselectorIndex, index )-1;
			int ex = max ( cselectorIndex, index )-1;
			clip_string = str.substr ( bx, ex-bx );
			str = str.substr ( 0, bx )+str.substr ( ex );
			set_texels ( str );
			tc->move_to ( selector_index );
		}
	else if ( para == DELETE ){
			int cselectorIndex = byte_index ( str, selector_index );
			int bx = min ( cselectorIndex, index )-1;
			int ex = max ( cselectorIndex, index )-1;
			if ( ex < str.size () ) { 
				str = str.substr ( 0, bx )+ str.substr ( ex );
			}
			else {
				str = str.substr ( 0, bx );
			}
			set_texels ( str );
			size_t pp = min ( selector_index, position );
			tc->move_to ( pp );
			selector_index = pp;
	}
	else if ( para == PASTE ){
			str = str.substr ( 0, index-1 )+clip_string+str.substr ( index-1 );
			set_texels ( str );
			tc->move_to ( position );
		}
	else if ( para == CHANGE ){
		editing ( DELETE );
		editing ( PASTE );
	}
	else if ( para == CUT_LINE ){
		size_t pos;
		size_t size;
		tc->get_row ( 0,&pos,&size );
		tc->move_to ( pos );
		selector_index = pos+size;
		index = byte_index ( str, pos );
		int cselectorIndex = byte_index ( str, selector_index );
		int bx = min ( cselectorIndex, index )-1;
		int ex = max ( cselectorIndex, index )-1;
		clip_string = str.substr ( bx, ex-bx );
		str = str.substr ( 0, bx )+str.substr ( ex );
		set_texels ( str );
		tc->move_to ( pos );
		selector_index = pos;
	}
	else if ( para == COPY_LINE ){
		size_t pos;
		size_t size;
		tc->get_row ( 0,&pos,&size );
		index = byte_index ( str, pos );
		int cselectorIndex = byte_index ( str, pos+size );
		int bx = min ( cselectorIndex, index )-1;
		int ex = max ( cselectorIndex, index )-1;
		clip_string = str.substr ( bx, ex-bx );
	}
	else if ( para == CUT_WORD ){
		size_t position = tc->get_position ();
		selector_index = tc->next_word_end ( position );		
		editing ( CUT );
		tc->move_to ( position );
		selector_index = position;
	}
}

void editor_c::scrolling ( int para1, int para2 )
{
	texel_caret_c* tc = get_caret ();
	if ( para1 == PAGE_U ){
		scroll_down = max ( 0, scroll_down-(frame_height-2 ));
	}
	else if ( para1 == PAGE_D ){
		int last_row = tc->get_last_row ();
		scroll_down = min ( scroll_down+frame_height-2, last_row-frame_height );
	}
	else if ( para1 == HOME ){
		echo << "HOME" << endl;
		scroll_down = 0;
	}
	else if ( para1 == END ){
		int last_line = tc->get_last_row ();
		scroll_down = max ( last_line-frame_height, 0 );
	}
	fit_caret ();
	if ( para2 == false ){
		selector_index = tc->get_position ();
	}
}

void editor_c::marking ( int para1, int para2 ){
	texel_caret_c* tc = get_caret ();
	if ( para1 == MARK ){
		stringstream ss;
		ss << file_path << " " << scroll_down << " " << tc->get_position ();
		mark1 = ss.str ();
	}
	else if ( para1 == GO_TO_MARK ){
		if ( mark1!="" ){
			stringstream ss ( mark1 );
			ss >> file_path;
			import_text ( file_path );
			ss >> scroll_down;
			size_t position;
			ss >> position;
			tc->move_to ( position );
			selector_index = position;
		}		
	}
}

void editor_c::moving ( int para1, int para2 )
{
	texel_caret_c* tc = get_caret ();
	if ( para1 == DEC_COLUMN ){
		tc->dec_column ();
		tc->set_column_memory ();
	}
	else if (para1 == INC_COLUMN ){
		tc->inc_column ();
		tc->set_column_memory ();
	}
	else if ( para1 == DEC_ROW ){
		tc->dec_line ();
	}
	else if ( para1 == INC_ROW ){
		tc->inc_line ();
	}		
	else if ( para1 == NEXT_WORD_BEG ){
		int position = tc->get_position ();
		position = tc->next_word ( position );
		tc->move_to ( position ); 
	}
	else if ( para1 == PREV_WORD_BEG ){
		int position = tc->get_position ();
		position = tc->previous_word ( position );
		tc->move_to ( position ); 
	}
	else if ( para1 == NEXT_WORD_END ){
		int position = tc->get_position ();
		position= tc->next_word_end ( position );
		tc->move_to ( position );
	}
	else if ( para1 == LINE_BEG ){
		size_t position;
		size_t size;
		tc->get_row ( 0,&position,&size );
		tc->move_to ( position );
	}
	else if ( para1 == LINE_END ){
		size_t position;
		size_t size;
		tc->get_row ( 0,&position,&size );
		tc->move_to ( position+size-1 );
	}
	else if ( para1 == NEW_LINE_U ){
		size_t position;
		int tabs = tc->get_leading_tabs ();
		
		tc->get_line ( 0,&position, 0 );
		tc->move_to ( position );		
		texel_c* p = texel_c::construct ('\n', texel_map );
		tc->insert ( p );
		for (;tabs>0;--tabs ){
			texel_c* p = texel_c::construct ('\t', texel_map );
			tc->insert ( p );			
		}
		fit_scrolls ();
	}
	else if ( para1 == NEW_LINE_D ){
		size_t position;
		size_t size;
		int tabs = tc->get_leading_tabs ();
		tc->get_line ( 0,&position,&size );
		tc->move_to ( position+size );
		texel_c* p = texel_c::construct ('\n', texel_map );
		tc->insert ( p );
		for (;tabs>0;--tabs ){
			texel_c* p = texel_c::construct ('\t', texel_map );
			tc->insert ( p );			
		}
	}
	if ( para2 == false ){
		selector_index = tc->get_position ();
	}
	fit_scrolls ();
}

void editor_c::clear ()
{
	texel_caret_c* tc = get_caret ();
	size_t position = 1;
	set_texels ( string ());
	tc->move_to ( position );
	selector_index = position;
	fit_scrolls ();		
}

texel_caret_c* editor_c::get_caret ()
{
	vector<texel_c*>::iterator it;
	it = texels.begin ();
	for (;it!= texels.end ();it++){
		texel_c* tc=*it;
		if ((*it )->get_char () =='\a'){
			return (texel_caret_c*)*it;
		}
	}
	return 0;
}

bool _download ( string& url, stringstream& ss )
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	ifstream f ( lynxs_path +"/pages/pb/" + url );
	if ( !f.good ()){
		return false;
	}
	ss << f.rdbuf ();
	return true;
}

void editor_c::import_webpage ( string url )
{
	lamb_c& lamb = *land.get_lamb ();
	static int c = 0;
	string s, s1;
//	url="www.a"+ to_string (++c );
	s="hier is the world\n hihi" + sc11::to_string (++c );
	bookmarks_c bm;
	string page;
	string file_name;
	stringstream ss;
	if ( !bm.look_in_cash ( url, file_name )){
		if ( _download ( url, ss )){
			bm.add_to_cash ( url, ss );
			cout << "url pseudo download" << endl;
		}
		else if ( lamb.download ( url, ss )){
			bm.add_to_cash ( url, ss );
			cout << "url downloaded" << endl;
		}
		else{
			cout << "url download fail" << endl;
			return;
		}
	}
	else {
		ifstream f ( file_name );
		ss << f.rdbuf ();
		cout << "file name:" << file_name << endl;
		cout << "url from cash" << endl;
	}
	str = ss.str ();
	file_path = url;
	convert_to_LF_format ( str );
	set_texels ( str );
	fit_scrolls ();
	draw ( lamb );
	return;
}

void editor_c::export_text ( std::string filename )
{
	lamb_c& lamb = *land.get_lamb ();
	if ( is_collapsed ){
		return;
	}
	string fn = filename;
	echo << "write file: " + fn << endl;
	ofstream fout ( fn, std::fstream::binary );
	string str1 = get_texels ();
	fout << str1;
	fout.close ();
	list<string>::iterator it;
	it = lamb.ed_mem.begin ();
	for (;it!= lamb.ed_mem.end ();){
		if ( it->substr ( 0, filename.size ()) == filename ){
			it = lamb.ed_mem.erase ( it );
			continue;
		}
		else {
			if (*it == "" ){
				it = lamb.ed_mem.erase ( it );
				continue;
			}
		}
		it++;
	}
	stringstream ss;
	ss << filename << " ";
	ss << scroll_down << " ";
	ss << get_caret ()->get_position ();
	lamb.ed_mem.push_front ( ss.str ());
	if ( lamb.ed_mem.size ()>20 ){
		lamb.ed_mem.resize ( 20 );
	}
	refresh ( filename );
}

void editor_c::import_text ( string filename, bool refresh )
{
	lamb_c& lamb = *land.get_lamb ();
	if ( filename == "" ){
		echo << "no file name" << endl;
		return;
	}
	ifstream fin ( filename );
	stringstream ss;
	if ( fin.good () ) {
		ss << fin.rdbuf ();
		echo << "read file: " + filename << endl;
	}
	str = ss.str ();
	unsigned char chdum;
	convert_to_LF_format ( str );
	sheck_string ( str, 0, &chdum );
	texel_caret_c* tc = get_caret ();
	size_t position = 1;
	if ( tc!= 0 ){
		position = tc->get_position ();
	}
	set_texels ( str );
	list<string>::iterator it;
	it = lamb.ed_mem.begin ();
	for (;it!= lamb.ed_mem.end ();it++){
		string s;
		s=*it;
		ss.seekg ( 0 );
		ss.seekp ( 0 );
		ss << s;
		ss >> s;
		if ( s == filename && refresh == false ){
			ss >> scroll_down;
			ss >> position;
			break;
		}
	}
	tc->move_to ( position );
	selector_index = position;
	fit_scrolls ();
//	draw ( lamb );
}

void editor_c::refresh ( std::string name )
{
	list<object_c*>::iterator it;
	it = land.llist.begin ();
	for (;it!= land.llist.end ();it++){
		if ((*it )->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)*it;
			if ( ed->file_path == name ){
				ed->import_text ( name, true );
			}
		}
	}
}

editor_c::~editor_c ()
{
}

void editor_c::fit_caret ()
{
	size_t row, column, position;
	texel_caret_c* caret = get_caret ();
	row = caret->get_row ( 0, 0, 0 );
	if ( row > frame_height+scroll_down ){
		caret->to_row ( frame_height+scroll_down );
	}
	else if ( row <= scroll_down ){
		caret->to_row ( scroll_down+1 );
	}
}

void editor_c::fit_scrolls ()
{
	texel_caret_c* tc = get_caret ();
	size_t row, column, size;
	row = tc->get_row ( 0, 0, 0 );
	if ( row>frame_height+scroll_down ){
		scroll_down = row-frame_height;
	}
	else if ( row<= scroll_down ){
		scroll_down = row-1;
	}
}

namespace command_table{

template<>
symbol_to_identifier_map_c<editor_c> command_base_c<editor_c>::symbols = {
marking,&editor_c::marking,
inserting_overwriting,&editor_c::inserting_overwriting,
collapsing,&editor_c::collapsing,
moving,&editor_c::moving,
change_mode,&editor_c::change_mode,
editing,&editor_c::editing,
deleting,&editor_c::deleting,
scrolling,&editor_c::scrolling,
runing,&editor_c::runing
};

/*
command_table_c<editor_c> cteditor{
};
*/

command_table_c<editor_c> cteditor{
COMMAND, ul_t ( 0 ),"r",	inserting_overwriting, CHARACTER, 1,
COMMAND, ul_t ( 0 ),"n",	collapsing, 0, true,
COMMAND, ul_t ( 0 ),"m",	marking, MARK, 1,
COMMAND, ul_t ( 0 ),"g",	marking, GO_TO_MARK, 1,
COMMAND, XK_h,			moving, DEC_COLUMN, false,
COMMAND, XK_l,			moving, INC_COLUMN, false,
VISUAL, XK_h,			moving, DEC_COLUMN, true,
VISUAL, XK_l,				moving, INC_COLUMN, true,
COMMAND, XK_k,			moving, DEC_ROW, false,
COMMAND, XK_j,			moving, INC_ROW, false,
VISUAL, XK_k,			moving, DEC_ROW, true,
VISUAL, XK_j,				moving, INC_ROW, true,
COMMAND, ul_t ( 0 ),"w",	moving, NEXT_WORD_BEG, false,
VISUAL, ul_t ( 0 ),"w",		moving, NEXT_WORD_BEG, true,
COMMAND, ul_t ( 0 ),"b",	moving, PREV_WORD_BEG, false,
VISUAL, ul_t ( 0 ),"b",		moving, PREV_WORD_BEG, true,
COMMAND, ul_t ( 0 ),"e",	moving, NEXT_WORD_END, false,
VISUAL, ul_t ( 0 ),"e",		moving, NEXT_WORD_END, true,
COMMAND, ul_t ( 0 ),"0",	moving, LINE_BEG, false,
VISUAL, ul_t ( 0 ),"0",		moving, LINE_BEG, true,
COMMAND, ul_t ( 0 ),"$",	moving, LINE_END, false,
VISUAL, ul_t ( 0 ),"$",		moving, LINE_END, true,
COMMAND, ul_t ( 0 ),"O",	moving, NEW_LINE_U, false,
COMMAND, ul_t ( 0 ),"o",	moving, NEW_LINE_D, false,

COMMAND, ul_t ( 0 ),"i",	change_mode, 0, _INSERT,
COMMAND, ul_t ( 0 ),"v",	change_mode, 0, _VISUAL,
COMMAND, ul_t ( 0 ),"s",	change_mode, 4, 0,
COMMAND, ul_t ( 0 ), XK_Escape,	change_mode, 0, _COMMAND,
INSERT, ul_t ( 0 ), XK_Escape,		change_mode, 0, _COMMAND,
VISUAL, ul_t ( 0 ), XK_Escape,		change_mode, 0, _COMMAND,

COMMAND, ul_t ( LL_KA_RC ),"b",	scrolling, PAGE_U, false,
COMMAND, ul_t ( LL_KA_RC ),"f",		scrolling, PAGE_D, false,
COMMAND, ul_t ( 0 ),"1G",			scrolling, HOME, false,
COMMAND, ul_t ( 0 ),"F",			scrolling, HOME, false,
COMMAND, ul_t ( 0 ),"G",			scrolling, END, false,
VISUAL, ul_t ( LL_KA_LC ),"b",		scrolling, PAGE_U, true,
VISUAL, ul_t ( LL_KA_RC ),"f",		scrolling, PAGE_D, true,
VISUAL, ul_t ( 0 ),"1G",				scrolling, PAGE_U, true,
VISUAL, ul_t ( 0 ),"G",				scrolling, PAGE_D, true,
COMMAND, ul_t ( 0 ),"x",			deleting, CHARACTER, 1,
VISUAL, ul_t ( 0 ),"a",				deleting, SPACE, 1,
VISUAL, ul_t ( 0 ),"s",				deleting, TAB, 1,
COMMAND, ul_t ( LL_KA_LC ),"o",		runing, 0, 0,
INSERT, ul_t ( LL_KA_LC ),"o",		runing, 0, 0,
COMMAND, ul_t ( LL_KA_RC ),"t",		runing, 1, 0,
INSERT, ul_t ( LL_KA_RC ),"t",		runing, 1, 0,
COMMAND, ul_t ( 0 ),"dd",			editing, CUT_LINE,
COMMAND, ul_t ( 0 ),"dw",			editing, CUT_WORD,
COMMAND, ul_t ( 0 ),"yy",			editing, COPY_LINE,
VISUAL, ul_t ( 0 ),"y",				editing, COPY,
VISUAL, ul_t ( 0 ),"d",				editing, CUT,
VISUAL, ul_t ( 0 ),"c",				editing, CHANGE,
VISUAL, ul_t ( 0 ),"p",				editing, PASTE,
COMMAND, ul_t ( 0 ),"p",			editing, PASTE,
};

} // end namespace command_table
int editor_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	keyboard_c& kb = keyb;
	bool proceeded = false;
	
	if ( valide > 1 ){
		 valide = 1;
//		return false;
	}
	if ( !kb.is_pressed () ) {
//		land.impress ( lamb );
		return false;
	}
	string c = kb.get_char ();
	int v = kb.get_stroke ();
	unsigned long control = kb.get_controls ();
	
	if ( mode == 5 ){
		string sss = "";
		command2 ( v, c, sss );
		return false;
	}
	if ( command_table::cteditor.find ( this, con_c ( mode ), ul_t ( control ), int ( v ), c )){
		return (true );
	}
	
	if ( mode == 4 ){
		return false;
	}
	string st;
	if ( !kb.is_representable ( v ) ||  control != 0 )
	{
		if ( v == XK_Down && control == LL_KA_C ){
			scroll_down++;
			fit_caret ();
			texel_caret_c* tc = get_caret ();
			selector_index = tc->get_position ();
			proceeded = true;
		}
		else if ( v == XK_Up && control == LL_KA_C ){
			scroll_down--;
			fit_caret ();
			texel_caret_c* tc = get_caret ();
			selector_index = tc->get_position ();
			proceeded = true;
		}
		else if ( v == XK_Escape ) {
			keyboard.stroke_list.clear ();
			proceeded = true;
		}
		else if ( v=='f' && control == LL_KA_C ) {
		}
		else if ( mode== _INSERT && v == XK_BackSpace ){
			int count = 0;
			texel_caret_c* tc = get_caret ();
			tc->erase ();
			fit_scrolls ();
			selector_index = tc->get_position ();
			proceeded = true;

		}
		else if ( mode== _INSERT && v == XK_Return ){
			texel_c* p = texel_c::construct ('\n', texel_map );
			texel_caret_c* tc = get_caret ();
			int clt = tc->get_leading_tabs ();
			tc->insert ( p );
			for (;clt!= 0;clt--){
				tc->insert ( texel_c::construct ('\t', texel_map ));
			}
			fit_scrolls ();
			selector_index = tc->get_position ();
			proceeded = true;

		}
		else if ( mode == _INSERT && v == XK_Tab && control == 0 ){
			echo << "edit in" << endl;
			texel_c* p = texel_c::construct ('\t', texel_map );
			texel_caret_c* tc = get_caret ();
			tc->insert ( p );
			fit_scrolls ();
			selector_index = tc->get_position ();
				proceeded = true;
		}
	}
	else {
		if ( c!="" && mode== _INSERT && !is_collapsed ){
			int count = 0;
			long chr = get_utf32 ( c, count );
			texel_c* p = texel_c::construct ( chr, texel_map );
			texel_caret_c* tc = get_caret ();
			if ( overwrite == 1 ){
				tc->inc_column ();
				tc->erase ();
			}
			tc->insert ( p );
			fit_scrolls ();
			selector_index = tc->get_position ();
			proceeded = true;

		}
	}
	if ( overwrite == 1 ){
		overwrite = 0;
		mode = 1;
	}
	if ( lamb.motor == &lamb.mouth ){
		string s;
		if ( v == XK_Return ){
			texel_caret_c* tc = get_caret ();
			int line_number = tc->get_last_line ();
			s = tc->get_string ( line_number-1 );
			cout << s << endl;
			echo << s << endl;
			if ( lamb.sensor != 0 ){
				lamb.sensor->command2 ( v, c, s );			
			}
			proceeded = true;
		}

	}
	return proceeded;
}

void editor_c::shift_page ( int begin, int end, ll::Direction direction )
{
	lamb_c& lamb = *land.get_lamb ();
	int fline;
	int fbegin;
	int fend;
	if ( begin == 0 ){
		fbegin = 1; 
	}
	else if ( begin==-1 ){
		fbegin = frame_height;
	}
	else{
		fbegin = begin-scroll_down;
	}
	if ( end == 0 ){
		fend = 1;
	}
	else if ( end==-1 ){
		fend = frame_height;
	}
	else{
		fend = end-scroll_down;
	}
	if ( fbegin <= fend ){
		if ( fbegin > frame_height || fend<1 ){
			 return;
		}
	}
	else if ( fbegin>fend ){
		if ( fbegin<1 || fend>frame_height ){
			return;
		}
	}
	matrix_c<FT> pA = motion.object_vector ( 1 );
	matrix_c<FT> m = motion.object_base ();
	matrix_c<FT> pab ({0,(FT )-20*fbegin, 0});
	matrix_c<FT> pa ({(FT )20*frame_width, 0, 0});
	matrix_c<FT> pb ( 3, 1 );
	matrix_c<FT> pc ( 3, 1 );
	matrix_c<FT> pt ( 3, 1 );
	pb[2]=(FT )-20*(fend-fbegin );
	if ( direction == ll::up ){
		pt[2]= 20;							
	}
	else {
		pt[2]=-20;
		pab[2]= pab[2]+20;
	}			
	pa = m*pa; pb = m*pb; pab = m*pab; pc = m*pb; pt = m*pt;
	pA = pA+pab;
	lamb.eyes.translate ( pA, pa, pb, pc, pt );
	lamb.expose_image ();
}

void editor_c::screen ()
{
	lamb_c& lamb = *land.get_lamb ();
	lamb.expose_image ();
}

string get_next_word ( string& str, size_t& ixn )
{
	if ( ixn>= str.size ()){
		return "";
	}
	int ix = ixn;
	if ( !isalnum ( str.at ( ix ))){
		ixn++;
		return str.substr ( ix, 1 );
	}
	while ( ixn<str.size () && isalnum ( str.at ( ixn ))){
		ixn++;
	}
	return str.substr ( ix, ixn-ix );
}

map<unsigned int, string> movements_map={
	{_SHIFT,"s"},
	{_ROTATE,"r"},
	{_SHIFT_OBJECT,"so"},
	{_ROTATE_OBJECT,"ro"},	
	{_POINT,"p"}
};

map<int, string> edit_modes_map={
	{_INSERT,"i"},
	{_COMMAND,"c"},
	{_VISUAL,"v"}
	
};

map<int , string> projects_map={
	{COMPILE_SNOW,"co"},
	{CLEAN_SNOW,"cl"},
	{OTHER_PROJECT,"op"}
};

map< int, string > figures_map = {
	{SPLINE,"spline"},
	{CARTESIAN,"cartesian"},
	{IMAGE,"image"},
	{EDITOR,"editor"},
};

map<int, string> shift_rotate_map = {
	{0, "s"},
	{1, "r"}
};

map <int, string> mouse_keyboard_map = {
	{0, "m"},
	{1, "k"}
};

void editor_c::draw ( lamb_c& lamb )
{
	texel_caret_c* tc = get_caret ();
	if ( valide > 1 ){
//		return;
	}
	matrix_c<FT> v;
		
	font.cell = 20;
	font.tab_width = 40;
	int& cell = font.cell;
	
	matrix_c<FT> vA = motion.object_vector ( 1 );
	matrix_c<FT> vAA = motion.object_vector ( 1 );
	matrix_c<FT> base = motion.object_base ();
	
	float ozoom = lamb.eyes.zoom;
	int flag = 0;
	if ( parent != 0 ){
		parent -> coocked_to_local ( this, base, vA, flag );
		if ( lamb.is_printing == false ){
			lamb.eyes.zoom = 1;
		}
	}
	
	float z = lamb.eyes.zoom;
	matrix_c<FT> ua, ubb, uA, vvA, vvx, vvy;
	
	ua = base.get_column ( 1 )*frame_width*cell;
	ubb = base.get_column ( 2 )*-(frame_height )*cell;

	uint32_t frame_color = 0x0;
	if ( state == 1 ){
		frame_color = 0xff0000;
	}
	else if ( this == lamb.motor && this!= lamb.sensor ){
		frame_color = 0x0000ff;
	}
	else if ( this == lamb.motor && this == lamb.sensor ){
		frame_color = 0xffff;
	}
	else if ( this == lamb.sensor ){
	 	frame_color = 0xff00;
	}
	if ( tc->is_silent == false ){
		visual_memories.erase_mem ( lamb.eyes.retine.surfaces.l.front ());
	}	
	
	if ( !tc->is_silent  && ( simple_text == false || lamb.sensor == this ) ){
		static int  c = 0;
		c ++;
		if ( c > 0 ){
			spline.color = frame_color;
			spline.control_points = {
				{vA + ubb}, 
				{vA}, 
				{vA + ua}, 
				{vA + ua + ubb}
			};
			spline.draw ( lamb );
		}
	}

	if ( tc->is_silent == false ){
		valide = 2;
	}	
	uA = z*lamb.world_to_view ( vA );

	if ( simple_text == true && (state == 1 || this == lamb.sensor )){
		uint32_t color = state == 1?0xff0000:0xff00;
			lamb.eyes.grip ( uA, color );
	}
	int frameshift_X = 2;
	int frameshift_Y = 18;
	
	int minY = scroll_down*cell;
	int maxY=(frame_height + scroll_down )*cell;
	int minX = 0;
	int maxX = frame_width*cell;
	int x = 0;
	int y = 0;
	x = frameshift_X;
	y = frameshift_Y;

	glyph_environment_c glyph_env;
	matrix_c<FT> v0 = {0, 0, 0};
	matrix_c<FT> vt;

	v = lamb.world_to_view ( vA );	
	glyph_env.vA = {{ v[1], v[2]}};
	vt = base.get_column ( 1 );
	v =  lamb.world_to_view ( vt ) - lamb.world_to_view ( v0 );
	glyph_env.vx = {{v[1], v[2]}};
	vt = base.get_column ( 2 );
	v = lamb.world_to_view ( vt ) - lamb.world_to_view ( v0 );	
	glyph_env.vy = {{v[1], v[2]}};
	if ( parent != 0  && flag == 0) {
		glyph_env.vx = { { 1, 0 }};
		glyph_env.vy = { { 0, 1 }};
		glyph_env.vA = { {vAA[2], vAA[3] } };
	}
	glyph_env.no = 0;
	glyph_env.is_selected = false;

	int index = tc->get_position ();
	glyph_env.selectionA = min ( index, selector_index );
	glyph_env.selectionB = max ( index, selector_index );
	glyph_env.caret = index;
	glyph_env.min_x = minX;
	glyph_env.max_x = maxX;
	glyph_env.min_y = minY;
	glyph_env.max_y = maxY;
	
	glyph_env.no = 1;
	glyph_env.x = frameshift_X;
	glyph_env.y=-frameshift_Y+minY;	
	glyph_env.penX = 0;
	glyph_env.penY = 0;

	glyph_env.tc = tc;

	glyph_env.editor = this;
	glyph_env.texels=&texels;
	
	
	font.engrave ( lamb, glyph_env );

	if ( tc->is_silent == false && simple_text == false ){
		glyph_env.caret = -1;
		glyph_env.selectionA = glyph_env.selectionB = 0;
		glyph_env.min_y = 0;
		glyph_env.max_y = cell;
		glyph_env.x = 0;
		glyph_env.y = -((frame_height )*cell+frameshift_Y );
		glyph_env.penX = 0;
		glyph_env.penY = 0;
		stringstream ss;
		size_t linex, rowx, columnx, sizex, widthx;
		widthx = (frame_width-1 )*cell;
		rowx = tc->row;
		sizex = tc->row_size;
		columnx = tc->get_position ()-tc->row_position+1;
		linex = tc->get_line ( 0, 0, 0 );
		ss <<  edit_modes_map[mode];
		if ( this == &lamb.mouth ) {
		ss
			<<  "|" << movements_map[lamb.movement] 
			<< "|" <<  projects_map[lamb.command_number]
			<< "|" << figures_map[figure]
			<< "|" << shift_rotate_map[lamb.shift_rotate]
			<< "|" << mouse_keyboard_map[lamb.keyboard_mouse]
			<< "|o." << land.llist.size ()
			<< "  ";
		}
		ss 
		<<  "  " << linex  
		<< ":" << rowx 
		<< ":" << tc->get_position ()
		<< ":" << columnx 
		<< ":" << sizex 
		<< ":" << texels.size ();
		
		vector<texel_c*> ntexels;
		set_texels ( ntexels, ss.str ());
		glyph_env.texels = &ntexels;
		glyph_env.tc = new texel_caret_c ();
		glyph_env.tc->texels = glyph_env.texels;
		font.engrave ( lamb, glyph_env );
		delete glyph_env.tc;
	}
	lamb.eyes.zoom = ozoom;
	return;
}

long get_utf32 ( string& s, int& c )
{
	long lc = s.at ( c );
	c++;
	if ((lc&0x80 ) == 0 ){
		return lc;
	};
	if ((lc&0xE0 ) == 0xC0 ){
		long l2 = s.at ( c );
		c++;
		lc = lc & 0x1F;
		lc= lc << 6;
		l2 = l2 & 0x3F;
		lc = lc | l2;
		return lc;
	}
	if ((lc&0xF0 ) == 0xE0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		lc = lc & 0x0F;
		lc= lc << 12;
		l2 = l2 & 0x3F;
		l2= l2 << 6;
		l3= l3 & 0x3F;
		lc = lc | l2 | l3;
		return lc;
	}
	if ( lc&0xF8 == 0xF0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		long l4 = s.at ( c );
		c++;
		return 0;
	}
	echo << "err" << endl;
	return lc;
}

string editor_c::get_texels ( size_t begin, size_t end )
{
	string s;
	begin--;
	end--;
	for ( int c = begin;c < end;c++){
		unsigned long ch = texels[c]->get_char ();
		if ( ch!= 0 && ch!='\a' && ch!= 1 ){
			s+= utf32_to_utf8 ( ch );
		}
	}
	return s;
}

string editor_c::get_texels (){
	string s;
	for ( int c = 0;c < texels.size ();c++){
		unsigned long ch = texels[c]->get_char ();
		if ( ch!= 0 && ch!='\a' && ch!= 1 ){
			s+= utf32_to_utf8 ( ch );
		}
	}
	return s;
}

void editor_c::set_texels ( vector<texel_c*>& texels, string s ){
	texels.clear ();
	texel_c* p;
	p = texel_c::construct ('\a', texel_map );
	texels.push_back ( p );
	for ( int c = 0;c<s.size ();){
		long chr = get_utf32 ( s, c );
		if ( chr == 1 ){
			continue;
		}
		p = texel_c::construct ( chr, texel_map );
		texels.push_back ( p );
	}
}

void editor_c::set_texels ( string s )
{
	set_texels ( texels, s );
	return;
}
// ä
int editor_c::sheck_string ( string& s, unsigned char, unsigned char*)
{
	int i = 1;
	for ( auto c : s ){
		if (  ( unsigned char ) c <  0x1F  &&  c != '\t'  && c != '\x7' && c != '\xa' ){
			echo << hex << (int)(unsigned char )c << endl;
			echo << "at position:" << i << "  char:" << ( int )( unsigned char ) c << endl;
		}	
		i++;
	}	
}

int editor_c::convert_to_LF_format ( string& str )
{
	int c = 0;
	bool dirty = false;
	for (;;){
		c = str.find ('\r', c );
		if ( c == string::npos ){
			break;
		}
		dirty = true;
		if ((c>0 && str.at ( c-1 ) =='\n') || ((c+1 )<str.size () && str.at ( c+1 ) =='\n') ){
			str.erase ( c, 1 );
		} 
		else{
			str.at ( c ) ='\n';
		}
	}
	if ( dirty ){
		echo << "converted to LF format"  <<  endl;
	}
	return 0; 
}

int editor_c::convert_to_CRLF_format ( string& str )
{
	echo << "convert to CR-LF" << endl;
	int c = 0;
	for (;;){
		c = str.find ('\r', c );
		if ( c==-1 ) break;
		c++;
		if ((c < str.size ()) && str.at ( c )!='\n') {
			str.replace ( c, 0, 1,'\n');
			c++;
		}
	}
	for ( c = 0;;){
		c = str.find ('\n', c );
		if ( c==-1 ) break;
		if ((c == 0 ) || (str.at ( c-1 )!='\r')){
			str.replace ( c, 0, 1,'\r');
			c++;
		}
		c++;
	}
	return 0;
}

namespace exeditor{
void entry ( int a, string s, void* pv ){
O ( editor )
	editor_c* pe = (editor_c*)pv;
	if ( a == 1 ){
//		pe->spline.control_points.clear ();
				
	}
	else if ( a == 2 ){
		for (int c = 0 ; c < 1000 ; c++){
		/*
		pe->spline.control_points.clear ();
		pe->spline.segments.clear ();
		pe->spline.controls.clear ();
		pe->spline.state = 1;
		pe->spline.control_points.push_back ({0, 50, 50});
		pe->spline.control_points.push_back ({0, 50, 300});
		*/		
		}
	}
}

}// end namespace exeditor

void ex_entry_editor (int a, string s ){
	lamb_c& lamb = *land.get_lamb ();
	auto it = land.llist.begin ();
	for (; it != land.llist.end (); it++){
	
		if ( (*it )->get_type () == EDITOR ){
			if (*it != &lamb.mouth && *it != &lamb.status ){
				exeditor::entry ( a , s, *it );
			}
		}				
	}
}



/*
string utf32_to_utf8 ( long chr )
{
	std::string s;
	unsigned char ch;
	if ( chr <= 0x7F ){
		s+= chr;
		return s;
	}
	if ( chr <= 0x7FF ){
		ch = chr >> 6;
		ch = ch | 0xC0; 
		s+= ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+= ch;
		return s;
	}
	if ( chr <= 0xFFFF ){
		ch = chr >> 12;
		ch = ch | 0xE0;
		s+= ch;
		ch= chr >> 6;
		ch= ch & 0x3F;
		ch = ch | 0x80;
		s+= ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+= ch;
		return s;		
	}
	if ( chr <= 0x1FFFFF ){
	}
		return s;
}
*/

//*=================
/*
void editor_c::searching ( string strs, string strr, int para, size_t* position )
{
	size_t pos=*position;
	string s = get_texels ();
	size_t inz = byte_index ( s, pos );
	while ( true ){	
		inz = s.find ( strs, inz );
		if ( inz == string::npos ){
			break;
		}
		if ( inz!= 0 ){
			char ch = s.at ( inz-1 );
			if ( ch >= 0x80 || isalnum ( ch )){
				inz++;
				continue;
			}
		}
		if ( inz+strs.size ()< s.size ()){
			char ch = s.at ( inz+strs.size ());
			if ( ch >= 0x80 || isalnum ( ch )){
				inz++;
				continue;
			}
		}
		break;
	}
	if ( inz!= string::npos ){
		texel_caret_c* tc = get_caret ();
		size_t pos = position_index ( s, inz );
		tc->move_to ( pos+1 );
		selector_index = pos+strs.size ()+2;
		int r = tc->get_row ( 0, 0, 0 );
		scroll_down = std::max ( 0, r-10 );
		*position = pos+2;
		return ;
	}
	*position = string::npos;
*/

/*
void editor_c::box_lines ( int x ){
	std::string c, s, sn;
	std::list<std::string>::iterator it, itn;
	it = lines.begin ();
	int line_count = 1;
	for (;it!= lines.end ();){
		s=*it;
		int dx = 0;
		size_t ix, ixn;
		itn = it; itn++;
		for ( ix = ixn = 0;ix<s.size ();ix = ixn ){
			c = get_next_word ( s, ixn );
			matrix_c<FT> m = font.get_text_size ( c );
			dx+= m[1][1];
			if ( c == "\n" ){
				if ( itn == lines.end ()){
					lines.push_back ( std::string ());
					itn = it; itn++;
				}
				if ( ixn == it->size ()){
					break;
				}
				*itn = it->substr ( ixn )+*itn;
				*it = it->substr ( 0, ixn );
				break;
			}
			//if ( false ){
			if ( dx >= frame_width*20 ) {
				if ( itn == lines.end ()){
					lines.push_back ( std::string ());
					itn = it;itn++;
				}
				*itn = it->substr ( ix )+*itn;
				*it = it->substr ( 0, ix );
				break;
			}
			//if ( false ){
			if ( ixn == it->size ()){
				if ( itn == lines.end ()){
					break;
				}
				
				size_t ixe = 0;
				size_t ixen = 0;
				int dxn = 0;
				
				for (;ixe<itn->size ();){
					c = get_next_word (*itn, ixen );
					m = font.get_text_size ( c );
					if ( dxn+m[1][1]>= frame_width*20-dx ){
						break;
					}
					dxn+= m[1][1];
					ixe = ixen;
				}
				it->append ( itn->substr ( 0, ixe ));
				if ( ixe<itn->size ()){
					*itn = itn->substr ( ixe );
				}
				else{
					lines.erase ( itn );
				}
			}
		}		
		it++;
		line_count++;
	}
}
*/




