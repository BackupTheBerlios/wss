#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <string>

#include "library/wdslib.h"
#include "wcp.h"
#include "matrix.h"
#include "position.h"

#include "retine.h"
#include "message.h"
#include "object.h"
#include "posix_sys.h"
#include "line.h"
#include "spline.h"
#include "global.h"

using namespace std;

void posix_sys_c::get_clipboard(string&)
{
}

void posix_sys_c::set_clipboard(string)
{
}

void posix_sys_c::file_service ( stringstream& ss )
{
}

int posix_sys_c::system_echo ( string& com , stringstream& echo )
{
	string path = get_lambs_path ();
	string fn_err = path + "/tmp/landerr";
	string fn_out = path + "/tmp/landout";
	
	ofstream f ( fn_err );
	f.close ();
	f.open ( fn_out );
	f.close ();
	
	string redirect;
	redirect = " 1>" + fn_out  + " 2>" + fn_err;

	string s = com + redirect;
	system ( s.c_str () );
	
	ifstream f_e ( fn_err );
	while ( f_e.good () ) {
		char c = f_e.get ();
		if ( ! f_e.eof () ){
			echo << c;
		}
	}
	ifstream f_o ( fn_out );
	while ( f_o.good () ) {
		char c = f_o.get ();
		if ( ! f_o.eof () ){
			echo << c;
		}
	}
	return 0;
}

