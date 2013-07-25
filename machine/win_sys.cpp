#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <string>

#include "../wcp.h"
#include "library/wdslib.h"
#include "../matrix.h"
#include "../position.h"
#include "../retine.h"
#include "../message.h"
#include "win_sys.h"

using namespace std;

void win_sys_c::get_clipboard ( string& )
{
}

void win_sys_c::set_clipboard ( string )
{
}

int win_sys_c::system_echo ( string& com , stringstream& echo )
{
	string path = get_lambs_path ();
	string fn_err = path + "/tmp/landerr";
	string fn_out = path + "/tmp/landout";
	string redirect;
	
	redirect = " 1>" + fn_out  + " 2>" + fn_err;

	string s = com + redirect;
	cout << s << endl;
	system ( s.c_str () );
	
	ifstream f_e ( fn_err );
	if ( f_e.good () ) {
		echo << f_e.rdbuf ();
	}
	ifstream f_o ( fn_out );
	if ( f_o.good () ) {
		echo << f_o.rdbuf ();
	}
	return 0;
}

void win_sys_c::system_output(bool bopen, FILE* stdf, string* path, FILE** pfile)
{
	cout << "win_sys output" << endl;
	return;
}
