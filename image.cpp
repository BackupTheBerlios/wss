#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <string>
#include <sstream>
#include <cassert>

#include "symbol/keysym.h"
#include "standard.h"

#include "matrix.h"
#include "position.h"
#include "global.h"
#include "library/wdslib.h"
#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"
#include "object.h"
#include "land.h"
#include "keyb.h"
#include "retine.h"
#include "mouse.h"
#include "eyes.h"
#include "fox/edit.h"
#include "home.h"
#include "lamb.h"
#include "image.h"

extern land_c land;

const unsigned long image_c::type = IMAGE;

using namespace std;

image_c::image_c ():page ( 1 ), file_name ("upclose.ppm" )
{
}

int edit ( lamb_c&, keyboard_c& )
{
}

void serialize ( fstream &file, bool bsave )
{
}

void image_c::convert_pdf_to_png ( string project_name )
{
}

unsigned long image_c::get_type ()
{
	return type;
}

void image_c::convert_png_to_ima ( string name )
{
	name="natpro-1.png";	
	string lambs_path = get_lambs_path ();
	lambs_path+="/";
	string str = name;
	if ( str.substr ( str.size ()-4 )!=".png" ){
		return;
	}
	
	string s = str.substr ( 0, str.size ()-4 );

	//string com="convert " + lambs_path + "tmp/" + s + ".png " + "-resize 500x600 " +  lambs_path + "tmp/" + s + ".bmp";
	
	string com="convert " + lambs_path + "tmp/" + s + ".png " +  lambs_path + "tmp/" + s + ".png";

//	system ( com.c_str ());
		
	cout << "ok" << endl;
	
//	string so = lambs_path + "tmp/" + s + ".bmp";
	string so = lambs_path + "tmp/" + s + ".pmc";
	ifstream fi;
	fi.open ( so );
	if (!fi.good ()){
		return;
	}
	fi.close ();
	string de = lambs_path + "tmp/" + s;
	str = lambs_path + "imex/myimage " + so + " " + de;
	system ( str.c_str ());
	
	fi.close ();
}

void image_c::convert_ppm_to_image ( string& arg1, string& arg2 )
{
		cout << "ppm to image" << endl;
		cout << "arg1:" << arg1 << endl;		
		ofstream fo ( arg2, ios::binary );
		ifstream fi ( arg1 );
		string s;
		fi >> s;
		cout << s << endl;
		size_t columns, rows;		
		fi >> columns;
		fi >> rows;
		fi >> s;
		cout << "columns:" << columns << endl;	
		cout << "rows:" << rows << endl;
		cout << "color:" << s << endl;
		fo.write ((char*)&columns, sizeof ( int32_t ));
		fo.write ((char*)&rows, sizeof ( int32_t ));
		for ( size_t ro = 0;ro < rows; ro++){
			for ( size_t co = 0; co < columns; co++){
				int32_t pi = 0;
				char c1,c2,c3;
				fi.read (&c3, 1 );
				fi.read (&c1, 1 );
				fi.read (&c2, 1 );
				pi = c1;
				pi = (pi << 8) + c2;
				pi = (pi << 8) + c3;
				fo.write ((char*)&pi, sizeof ( int32_t ));
			}
		}
	return; 
}

void image_c::convert ( string name )
{
	cout << "xxxxx convert" << endl;
	string lambs_path = get_lambs_path ();
	lambs_path+="/";
	cout << "convert name:" << name << endl;
	if ( name.substr ( name.size ()-4 )!=".pdf" ){
		return;
	}
	string s = name.substr ( 0, name.size ()-4 );

	string st;
	//st="/home/henry/shelf/cpp/lamb/doc/";
	
	ifstream fi ( st + s + ".pdf" );
	if (!fi.good ()){
		return;
	}
	fi.close ();
	stringstream ss;
	string crop;
//	crop  = " -x 150 -y 100 -W 600 -H 1000 ";	
//	crop  = " -x 200 -y 200 -W 700 -H 1000 -r 130 ";	
	crop  = " -x 200 -y 150 -W 700 -H 1000 -r 120 ";	
	string str ="pdftoppm " + crop  + st + s + ".pdf " + lambs_path + "tmp/" + s;
	cout << str << endl;
//	lamb.system_shell ( str, ss );
	land.get_lamb () -> system_echo ( str, ss );
	for ( ptrdiff_t c = 1;;c++){
		string n = sc11::to_string ( c );

		string so = lambs_path + "tmp/" + s + "-" + n + ".ppm";
		fi.open ( so );
		cout << "file source:" << so << endl;

		if (!fi.good ()){
			fi.close ();
			so = lambs_path+ "tmp/" + s + "-0" + n + ".ppm";
			fi.open ( so );
			if (!fi.good ()){
				break;
			}
		}
		string de = lambs_path + "tmp/" + s + "-" + n;
		
		convert_ppm_to_image ( so, de );
		cout << "command:" << str << endl;
		fi.close ();
	}
	return;
}

void image_c::load ( string& name )
{
	string lambs_path = get_lambs_path ();
	lambs_path+="/";
	string s;
	s="./myimage "+lambs_path+"png4.png " + lambs_path+"image3";
	system ( s.c_str ());
	ifstream fi;
	fi.open ( lambs_path+"image3", ios::binary );
	fi.read ((char*)&columns, sizeof ( int32_t ));
	fi.read ((char*)&rows, sizeof ( int32_t ));
	for ( size_t ro = 0; ro < rows ; rows++){
		for ( size_t co = 0; co < columns ; columns++){
						
		}
	}
}

void image_c::draw ( lamb_c& lamb )
{
	matrix_c < FT > vA = motion.object_vector ( 1 );
	matrix_c < FT > base = motion.object_base ();
	matrix_c < FT > ux = base .get_column ( 2 );
	matrix_c < FT > uy = base.get_column (  3 );
	
	string lambs_path = get_lambs_path ();
//	lambs_path+="/";
//	string n = sc11::to_string ( page );
//	string str = file_name + "-" + n;
	string s = lambs_path + "/desk/" + file_name;
	string s2 = lambs_path + "/desk/image";
	ifstream fi;
	fi.open ( s );
	if (!fi.good ()){
		echo << "file bad " << endl;
		return;
	}
	fi.close ();
	convert_ppm_to_image ( s, s2 ); 
	fi.open ( s2 );
	fi.read ((char*)&columns, sizeof ( int32_t ));
	fi.read ((char*)&rows, sizeof ( int32_t ));
	
	matrix_c<FT> v0 = {0, 0, 0};
	matrix_c<FT> vt,v;

	v = lamb.world_to_view ( vA );	
	matrix_c < FT > vvA = {{ v[1], v[2]}};
	vt = base.get_column ( 2 );
	v =  lamb.world_to_view ( vt ) - lamb.world_to_view ( v0 );
	matrix_c < FT > vvx = {{v[1], v[2]}};
	vt = base.get_column ( 3 );
	v = lamb.world_to_view ( vt ) - lamb.world_to_view ( v0 );	
	matrix_c < FT > vvy = {{v[1], v[2]}};
	matrix_c < FT > m = vvA + 8*vvx + 4* vvy;
	for ( size_t ro = 0; ro < rows ; ro++){
		for ( size_t co = 0; co < columns ; co++){
			matrix_c < FT > m = vvA + co*vvx + (rows - ro)* vvy;
			int32_t* color;
			fi.read ((char*)&color, sizeof ( int32_t ));
//			lamb.matrices_to_view ( );
			lamb.eyes.set_pixel ( 0, m [1], m [2], 0, (long )color, true );
		}
	}
	ux = columns*ux;
	uy = rows*uy;
	frame.control_points = { 
		vA,
		vA + ux,
		vA + ux + uy,
		vA + uy,
		vA
	};
	frame.draw ( lamb );

}

