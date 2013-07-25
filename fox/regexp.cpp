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

#include "echo.h"
#include "regexp.h"

#include "global.h"

#include "object.h"
#include "keyb.h"
#include "font.h"
#include "line.h"
#include "edit.h"

using namespace std;

reg_exp_c::reg_exp_c ():
	look_before ( 0 ),
	look_after ( 0 ),
	mode ( 0 ),
	is_search_raw ( false ),
	is_search_excluded ( false ),
	is_search_end_raw ( false ),
	is_search_end_excluded ( false )
{
}

reg_exp_c::reg_exp_c (const reg_exp_c& rx ) 
{
	look_before = rx.look_before;
	look_after = rx.look_after;
	mode = rx.mode;
	is_search_raw = rx.is_search_raw;
	is_search_excluded = rx.is_search_excluded;
	is_search_end_raw = rx.is_search_end_raw;
	is_search_end_excluded = rx.is_search_end_excluded;
	search = rx.search;
	search_end = rx.search_end;
	replace = rx.replace;
}

void reg_exp_c::dump ( stringstream& ss )
{
	ss << hex << mode << endl
	<< "search=============" << endl 
	<< search << endl 
	<< "search_end==========" << endl
	<< search_end << endl
	<< "replace=============" << endl
	<< replace << endl;
	echo << ss.str ();
}

void reg_exp_c::command ( stringstream& ss )
{
	string s1;
	ss >> s1;
	stringstream ss1;
	if ( s1 == "-d" ) {
		dump ( ss1 );
	}
	if ( s1 == "-s" ) {
		ss >> search;
	}
	else if ( s1 == "-sf" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		search = ss.str (); 
		echo << search << endl;
	}
	else if ( s1 == "-se" ) {
		ss >> search_end;
	}
	else if ( s1 == "-sef" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		search_end = ss.str (); 
		echo << search_end << endl;
	}
	else if ( s1 == "-r" ) {
		ss >> replace;
	}
	else if ( s1 == "-rf" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		replace = ss.str (); 
		echo << replace << endl;
	}		
	else if (s1 == "-m" ) {
		int i;
		ss >> hex >> i;
		mode = i;
		is_search_raw = i & 0x8 ? 1:0;
		is_search_excluded = i & 0x4 ? 1:0;
		is_search_end_raw = i & 0x2 ? 1:0;
		is_search_end_excluded = i & 0x1 ? 1:0;	
	}
	else if (s1 == "-la" ) {
		ss >> look_before;
		ss >> look_after;		
		echo << "regex lookaround:" << look_before << ":" << look_after << endl;
	}
}

bool reg_exp_c::find ( 
string& text, size_t& position,
	bool is_raw, string& search,
	string& found ) 
{
	if ( ! is_raw ) {
		if ( position == string::npos || position >= text.size () ) {
			found = "";
			return false;
		}
		boost::regex rgx ( search );
		boost::match_results < string::iterator > smr;
		string::iterator its = text.begin ();
		advance ( its , position );
		if ( boost::regex_search ( its, text.end (), smr, rgx ) ) {
			position += smr.position ();
			found = smr.str ();
			echo << "found:" << found;
			return true;
		}
		position = string::npos;
		return false;
	}
	else {
		size_t pm = position;
		for (; position < text.size (); position ++ ) {
			if ( position + search.size () > text.size () ) {
				found = "";
				return false;
			}
			for ( int c = 0; c < search.size (); c++ ) {
				if ( text [ position + c ] != search [ c ] ) {
					break;
				}
				if ( c + 1 == search.size () ) {
					found = search;
					return true;
				}
			}
		}
		found = "";
	}
}

bool reg_exp_c::find_section ( string& s, size_t& position, string& found )
{

	if ( search_end == "" ) {
		return find ( s, position, is_search_raw, search, found ) ; 
	}

	if ( find ( s, position, is_search_raw, search, found ) ) {
		if ( search_end == "" ) {
			return true;
		}
		size_t pos = position;
		if ( find ( s, pos, is_search_end_raw, search_end, found ) ) {
			found = s.substr ( position, pos - position + found.size () );
			return true;
		}
	} 	
	found = "";
	return false;
}

//juli
//juli

bool reg_exp_c::find_section ( 
	string& s, size_t& position,
	bool is_sb_raw, bool is_sb_excluded, string &search_begin,
	bool is_se_raw, bool is_se_excluded, string &search_end,
	string& found )
{

	if ( search_end == "" ) {
		return find ( s, position, is_sb_raw, search_begin, found ) ; 
	}

	if ( find ( s, position, is_sb_raw, search_begin, found ) ) {
		if ( search_end == "" ) {
			return true;
		}
		size_t pos = position;
		if ( find ( s, pos, is_sb_raw, search_end, found ) ) {
			found = s.substr ( position, pos - position + found.size () );
			return true;
		}
	} 	
	found = "";
	return false;
}

