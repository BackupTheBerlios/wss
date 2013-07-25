#include <cstdio>
#include <cstdlib>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <complex>
#include <cmath>


#include "symbol/keysym.h"
#include "library/wdslib.h"
#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"
#include "global.h"

#include "echo.h"
#include "matrix.h"

#include "position.h"
#include "object.h"
#include "keyb.h"
#include "retine.h"
#include "eyes.h"
#include "mouse.h"
#include "fox/edit.h"

#include "make.h"
#include "home.h"
#include "lamb.h"
#include "land.h"

extern lamb_c lamb;
extern land_c land;

using namespace std;

make_c::make_c ():
	flag_thread ( 0 )
{
}

void
make_c::read_file ( string name, list<string>& l )
{
	ifstream fin ( name ); 
	string str;
	if( !fin.good ()){
		return;
	}
	while(!fin.eof ()){
		getline ( fin, str );	
		l.push_back ( str );		
	}	
}

string  make_c::get_name ( string & s )
{
	size_t pos = s.rfind("/" );
	if ( pos!= string::npos ){
		return s.substr ( pos+1 );
	}
	return s;
}

string make_c::get_suffix ( string & s )

{
	size_t pos = s.rfind("." );
	return s.substr ( pos );
}

string make_c::get_path_shortname ( string &s )
{
	size_t pos = s.rfind("." );
	if ( pos!= string::npos ){
		return s.substr ( 0, pos );
	}
	return s;
}

string make_c::get_path ( string & s )
{
	size_t pos = s.rfind("/" );
	return s.substr ( 0, pos );
}

string make_c::get_shortname ( string& s )
{
	string s1 = get_name ( s );
	size_t pos = s1.rfind("." );
	if ( pos!= string::npos ){
		return s1.substr ( 0, pos );
	}
	return s1;
}

bool make_c::is_suffix ( string& s, string suffix )
{
	size_t pos = s.rfind("." );
	if ( pos!= string::npos ){
		if ( s.substr ( pos ) == suffix ){
			return true;
		}
	}
	return false;
}

bool make_c::is_in_list ( string& s, list<string>& l )
{
	list<string>::iterator it;
	it = l.begin ();
	for(;it!= l.end ();it++){
		if ( wild_card ( s,*it )){
			return true;
		}
	}
	return false;
}

void make_c::change_list ( list<string>& source, list<string>& destination, string sufix )
{
	list<string>::iterator it;
	it = source.begin ();
	size_t pos;
	string s1, s2;
	for(;it!= source.end ();it++){
		string s1=*it;
		if((pos = s1.rfind(".c" ))!= string::npos ||
			(pos = s1.rfind(".cpp" ))!= string::npos ){
			s2 = s1.substr ( 0, pos );
			s2="mybd/" + s2;
			destination.push_back ( s2 );
		}
	}
}

list<string> exclude={
"video.c",
"font_freetype.c",
"web.c",
"rsvg.c",
"webp.c",
"mygui.c",
"myfbtk.c"
};

bool match ( string& s, string::size_type& i, string& s2, int mode )
{
	const int begin = 2;
	const int middle = 1;
	if ( mode == middle ){
		i = s.find ( s2, i );
		if ( i == string::npos ){
			return false;
		}
		else{
			i = i+s2.size ();
			return true;
		}
	}
	else if ( mode == begin ){
		string ss;
		ss = s.substr ( i, s2.size ());
		if ( ss!= s2 ){
			return false;
		}
		else{
			i = i+s2.size ();
			return true;
		}
	}
}

bool make_c::wild_card ( string& s, string& wc )
{
	const int begin = 2;
	const int middle = 1;
	string::size_type i, iwc, iwc2;
	i = 0;
	iwc = 0;
	int mode;
	for(;iwc!= string::npos && iwc<wc.size ();){
		char ch = wc.at ( iwc );
		if ( ch=='?'){
			iwc++;
			i++;
			continue;
		}
		if ( ch=='*'){
			iwc++;
			mode = middle;
		}
		else{
			mode = begin;
		}
		iwc2 = wc.find_first_of ( string("*?" ), iwc );
		string ss;
		if ( iwc2 == string::npos ){
			ss = wc.substr ( iwc );
		}
		else{
			ss = wc.substr ( iwc, iwc2-iwc );
		}
		if ( match ( s, i, ss, mode ) == false ){
			return false;
		}
		iwc = iwc2;
	}
	if ( i<s.size ()){
		//cout << s << endl;
		return false;
	}
	return true;
}


void make_c::work ()
{
	ifstream fi("makescan-back" );
	ofstream fo("makescan" );
	string s;
	while(!fi.eof ()){
		getline ( fi, s );
		fo << s << endl;
		fo << get_path_shortname ( s ) + ".h" << endl;
	}
}

list<string> license={
"/*",
"License for the program White Source Software",
"",
"Copyright (C ) 2012 Henry Steyer",
"",
"This program is free software; you can redistribute it and/or modify",
"it under the terms of the GNU General Public License as published by",
"the Free Software Foundation; either version 2 of the License, or",
"(at your option ) any later version.",
"",
"This program is distributed in the hope that it will be useful,",
"but WITHOUT ANY WARRANTY; without even the implied warranty of",
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"GNU General Public License for more details.",
"",
"You should have received a copy of the GNU General Public License",
"along with this program; if not, write to the Free Software",
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA",
"*/",
"",
""
};

list<string> excluded_files={
//	"*.h"
};

list<string> included_files={
	"*.cpp",
	"*.h"
};

void make_c::copyright ( int nr )
{
	lamb_c& lamb = *land.get_lamb ();	
	string copyr="/* this is my copyright \n yes*/";
	list<string> sourcess, sources;
	list<string> dest;
	list<string>::iterator it, it2;
	lamb.walk("./", sourcess );
	it = sourcess.begin ();
	cout << sourcess.size () << endl;
	for(;it!= sourcess.end ();it++){
		if ( is_in_list(*it, included_files )){
			sources.push_front(*it );
		}	
	}

	it = sources.begin ();
	for(;it!= sources.end ();it++){
		cout << *it << endl;
	}
	fstream f;
	stringstream ss;
	it = sources.begin ();
	if ( nr == 1 ){
		cout << "11111" << endl;
		for(;it!= sources.end ();it++){
			cout << *it << endl;
			f.open(*it );
			ss.clear ();
			ss.str ( string ());
			ss << f.rdbuf ();
			f.seekp ( 0 );
			it2 = license.begin ();
			for(;it2!= license.end ();it2++){
				f << *it2 << endl;
			}
			f << ss.rdbuf ();
			f.close ();
		}
	} 
	else if ( nr == 2 ){
		cout << "2222" << endl;
		for(;it!= sources.end ();it++){
			//cout << *it << endl;
			f.open(*it );
			ss.clear ();
			ss.str ( string ());
			ss << f.rdbuf ();
			ss.clear ();
			bool match = true;
			list<string>::iterator it2;
			it2 = license.begin ();
			for(;it2!= license.end ();it2++){
				string s;
				getline ( ss, s );
				if ( s!=*it2 ){
					match = false;
					break;
				}
			} 
			if ( match ){
				f.close ();
				ofstream fo(*it, ios::trunc );
				fo << ss.rdbuf ();
			}
			else{
				echo << "copyright not found" << *it << endl;
			}
			f.close ();
		}
		cout << "end 222" << endl;
	}
	if ( nr == 3 ){
		cout << "333" << endl;
	}
	return;
}

void make_c::scan ()
{
	lamb_c& lamb = *land.get_lamb ();
	list<string> sources;
	list<string> dest;
	list<string>::iterator it;
	cout << "scan" << endl;
	lamb.walk("./", sources );
	it = sources.begin ();
	ofstream f("so" );
	for(;it!= sources.end ();it++){
		string s;
		s = it->substr ( 2 );
		if ( is_in_list ( s, included_files ) && !is_in_list ( s, excluded_files )){
			cout << s << endl;

			f << s << endl;
		}
	}
	f.close ();
	return;
}

void make_line ( stringstream& ss, list<string>&sources, list<string>::iterator& it )
{
	bool begin = true;
	while ( it!= sources.end ()){
		string s=*it;
		if ( s!="" && (s.substr ( 0, 1 )!="<" )){
			if(!begin ){
				ss << " ";
			}
			else {
				begin = false;
			}
			ss << s;
			it++;
		}
		else{
			break;
		}
	}
	ss << endl;
}

list < string > incl = {
//"*.h",
//"*.cpp",
"*_in",
//"*.txt",
//"*.tex",
"LICENSE",
"COPYING",
"README",
"*.ttf"
};

list < string > excl = {
};

string dir_name;

int make_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	string c = keyb.get_char ();
	if ( flag_thread != 0  ) {
		static reg_exp_c orx;
		static list < string > ofl;
		static string of;
		static size_t pos_o;
		static size_t scroll_o;
		static int ofr;
		static int flag;
		static editor_c* e; 
		string s1, s2;
		if (flag_thread == 1 ){
			e = (editor_c*) lamb.sensor;
			list < string > inc = {
				"*.h",
				"*.cpp",
				"*_in"
			};
			list < string > exc = {
//				"make.cpp"
			};
			orx = e -> regexp;
			ofl = e -> file_list;
			of = e -> file_path;
			e -> file_list.clear ();
			reg_exp_c rx;
			rx.search = "^//expsecb";
			rx.search_end = "^//expsece";
			rx.replace = "";
			rx.look_after = 1;
			e -> regexp = rx;
			
			list < string > sourcess;
			list < string > dest;
			lamb.walk ( proper_directory_name + "/", sourcess );
			for ( auto x : sourcess ) {
				if ( is_in_list ( x, inc ) && ! is_in_list ( x, exc ) ) {
					dest.push_back ( x );
				}
			}
			
			e -> file_list = dest;
			flag = 0;
			string c = "";
			for ( auto x : e -> file_list ) {
				echo << x << endl;
			}
			lamb.motors.remove(&lamb );
			lamb.motors.push_front(&lamb );
			lamb.motor = &lamb;
			lamb.hook = this;		
			texel_caret_c* tc = e -> get_caret ();
			pos_o = tc -> get_position ();
			scroll_o = e -> scroll_down;
			s1 = "";
			s2 = ":w";
			e -> command2 ( XK_Return, s1,   s2 );
			s2 = ":new";
			e -> command2 ( XK_Return, s1,   s2 );
			e -> run_search_replace ( flag, 0, c );
			if ( flag == 0 ) {
				flag_thread = 3;
			}
			else {
				flag_thread = 2;
				return 0;
			}
		}
		if ( flag_thread == 2 ) {
			e -> run_search_replace ( flag, 0, c );
			if ( flag == 0 ){
				flag_thread = 3;
			}
			else {
				return 0;
			}
		}
		if ( flag_thread == 3 ) {
			e -> file_list = ofl;
			e -> regexp = orx;
			lamb.hook = nullptr;
			flag_thread = 0;
			proper ( 2, proper_directory_name, "n" );
			s2 = ":e " + of;
			e -> command2 ( XK_Return, s1, s2 );
			texel_caret_c* tc = e -> get_caret ();
			tc -> move_to ( pos_o );
			e -> scroll_down = scroll_o;
			e -> selector_index = pos_o;
		}
		return 0;
	}
}

void make_c::remove_experimental_section ( string dir_name )
{
	string lp = get_lambs_path ();
	lamb_c& lamb = *land.get_lamb ();
	if ( lamb.sensor -> get_type () != EDITOR ) {
		return;
	}
	keyboard_c keyb;
	flag_thread = 1;
	proper_directory_name = dir_name;
	edit ( lamb, keyb );
};

void make_c::proper ( int part, string dir_name , string option )
{
	list < string > incl = {
		"*.h",
		"*.cpp",
		"*_in",
		"*.ttf",
//		"makewin",
//		"makesdl",
//		"makex11",
//		"makelinux",
//		"makeproper",
//		"makewcp",
		"*.txt",
		"*.tex",
		"LICENSE",
		"COPYING",
		"README"
	};
	
	list < string > excl = {
		".hg/last-message.txt",
		"test.cpp",
	};
	
	list < string > exclex = {
	};
	
	if ( dir_name == "" ) {
		echo << "no name " << endl;
		return ;
	}
	bool mercurial = false, rm_ex = false;
	if ( option.find ( "-m" ) != string::npos ){
		mercurial = true;
		echo << "with mercurial " << endl;
	}
	if ( option.find ( "-re" ) != string::npos ){
		rm_ex = true;
		echo << "remove expesec " << endl;
	}

	if ( part == 1 ) {
		string sc, s, s1, s2;
		stringstream ss;
		string lp = get_lambs_path ();
		lamb_c& lamb = *land.get_lamb ();
		sc = "mkdir " + dir_name;
		lamb . system_echo ( sc, ss );
		list < string > sourcess;
		lamb . walk ( lp + "/", sourcess );
		auto it = sourcess . begin ();
		for (; it != sourcess.end (); it++ ) {
			s = it -> substr ( lp.size () + 1 );
			 if ( is_in_list ( s, incl ) == true  && 
				is_in_list ( s, excl ) == false && 
				( rm_ex == false || is_in_list ( s, exclex ) == false  ) ){
				s1 = s;
				size_t sz = 0;
				for (;;) {
					sz = s.find ( "/", sz );
					if ( sz == string::npos ) {
						break;
					}
					s1 = s.substr ( 0, sz );
					sc = "mkdir " + dir_name + "/" + s1;  
					lamb . system_echo ( sc, ss );
					echo  << sc << endl;
					sz++;
				}
				sc = "cp -r " + *it + " " + dir_name + "/" + s;
				lamb . system_echo ( sc, ss );
				echo << sc << endl;
			}
		}		
		
		sc = "mkdir " + dir_name + "/config";
		lamb.system_echo ( sc, ss );
		string hgempty = dir_name + "/config/.hgempty";
		ofstream ofs ( hgempty );
		ofs.close ();
		sc = "mkdir " + dir_name + "/tmp";
		lamb.system_echo ( sc, ss );
		hgempty = dir_name + "/tmp/.hgempty";
		ofs. open ( hgempty );
		ofs.close ();
		if ( mercurial ) {
			sc = "cp -r " + lp + "/.hg " + dir_name;
			lamb.system_echo ( sc, ss );
			echo << "with mercurial"<< endl;
		}
		if ( rm_ex ) {
			remove_experimental_section ( dir_name  );
		}
	}
	else {
		list < pair < string, string > > l =	{ 
			{"makewcp_in",  "makewcp"},
			{"makesdl_in", "makesdl"},
			{"makelinux_in", "makelinux"},
			{"makex11_in", "makex11"},
			{"makewin_in", "makewin"}
		};
		string mfi, mf;	
		for ( auto p : l ) {
			mfi = dir_name + "/" + p.first;
			mf = dir_name + "/" + p.second;
			make ( mfi, mf );
		} 
		mfi = dir_name + "/makeproper_in";
		mf = dir_name + "/makeproper";
		mkproper ( mfi, mf );
	}
}

void make_c::mkproper ( string& minfile, string& mfile )
{
	cout << "makeproper: " << minfile << " " << mfile << endl;
	list<string> infile;
	read_file ( minfile, infile );
	list<string>::iterator it;
	it = infile.begin ();
	if ( it == infile.end () || *it!="<PROPER>" ){
		cout << "not a correct proper in file" << endl;
		return;
	}
	it++;
	string s;
	string com;
	stringstream ss;
	ss << "all:" << endl;
	for(;it!= infile.end ();){
		if(*it == "<COMMAND>" ){
			it++;
			if ( it == infile.end ()){
				break;
			}
			com=*it;
			it++;
			continue;
		}
		ss << "\t" + com + " " + *it << endl;
		it++;
	}
	ofstream f;
	f.open ( mfile );
	f << ss.str ();
	f.close ();
}

void make_c::make ( string& minfile, string& mfile )
{	
	if ( minfile == "" || mfile == "" ){
		return;
	}

	cout << "make: " << minfile << " " << mfile << endl;
	cout << "<BEGIN>" << endl;
	list<string> sources;
	list<string> infile;
	list<string> canon;
	list<string>::iterator it, it1, it2;

	//scan ();
	string s, s1;
	string path;
	string exe;
	string exe_out;
	string library;
	read_file ( minfile, infile );
	bool is_source = false;
	stringstream ss;
	it = infile.begin ();
	for(;it!= infile.end ();)
	{
		string tag=*it;
		if ( tag.substr ( 0, 2 ) == "//" ) {
			 it ++; 
			continue;
		}
		else if ( tag == "" ) {
			it ++;
			continue;
		}
		else if ( tag == "<CC>" ){
			ss << "CC = "; 
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue; 
		}
		else if ( tag == "<LL>" ){
			ss << "LL = ";
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue;
		}
		else if ( tag == "<CFLAG>" ){
			ss << "CFLAG = ";
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue;
		}
		else if ( tag == "<CINCLUDE>" ){
			ss << "CINCLUDE = ";
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue;
		}
		else if ( tag == "<LIB>" ){
			ss << "LIB = ";
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue;
		}
		else if ( tag == "<LFLAG>" ){
			ss << "LFLAG = ";
			make_line ( ss, infile,++it );
			ss << endl;
			is_source = false;
			continue;
		}
		else if ( tag == "<EXE>" ){
			ss << "EXE = ";
			make_line ( ss, infile,++it );
			ss << endl;
//			if ( it!= infile.end ()){
//				exe=*it;
//			}
			is_source = false;
//			it++;
			continue;
		}
		else if ( tag == "<PATH>" ){
			it++;
			if ( it!= infile.end ()){
				path=*it;
			}
			it++;
		}
		else if ( tag == "<SOURCE>" ){
			is_source = true;
			it++;
		}
		else if ( tag!="" && tag.substr ( 0, 1 ) =="<" ){
			cout << "tag error:" << tag << endl;
			return;
		}
		else {
			if ( path!="" ){
				sources.push_back ( path+"/"+tag );
			}
			else {
				sources.push_back ( tag );
			}
			it++;
		}
	}
	ss << "OBJ = ";
	it = sources.begin ();
	for(;it!= sources.end ();it++){
		if(*it == "" ){
			continue;
		}
		if ( is_suffix(*it,".cpp" ) || is_suffix(*it,".c" )){
			ss << get_path_shortname(*it ) << ".o ";
		}
	}	
	ss << endl;
	ss << endl;

	ss << "all : " <<  "$(EXE) $(OBJ) " << endl;
	ss << endl;
		
	ss << "$(EXE) : $(OBJ)"<< endl; 
//	ss << "\t$(LL) $(LFLAG) $(OBJ) " 
	ss << "\t$(LL) $(OBJ) $(LFLAG) " 
	<< " -o $(EXE)" << endl;
	ss << endl;

	it = sources.begin ();
	for(;it!= sources.end ();){
		s=*it;
		if ( s == "" ){
			it++;
			continue;
		}
		ss << get_path_shortname ( s ) << ".o" << " : "
		<< s;
		it++;
		for(;it!= sources.end ();it++){
			s1=*it;
			if ( is_suffix ( s1,".h" )){
				ss << " " << s1;
			}
			else{
				break;
			}
		}
		ss << endl;
		ss << "\t$(CC) $(CFLAG) $(CINCLUDE) -c " <<  s 
		<< " -o " << get_path_shortname ( s ) << ".o" << endl; 
		ss << endl;
	}
	ss << "clean : "  << endl; 
	ss << "\trm " << "$(EXE) $(OBJ)"; 
	ss << endl;

	cout << ss.str () << endl;
	cout << "<END>" << endl;
	ofstream f;
	f.open ( mfile );
	f << ss.str ();
	f.close ();
}

