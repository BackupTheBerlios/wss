// lamb, the future of the lamb, the fox and the lynx

#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <map>
#include <complex>
#include <chrono>
#include <cmath>

#include "symbol/keysym.h"

#include "standard.h"


#include "library/wdslib.h"
#include "wcp.h"

#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"

#include "global.h"

#include "object.h"

#include "mouse.h"

#include "eyes.h"
#include "keyb.h"
#include "home.h"
#include "lamb.h"
#include "comtab.h"

#include "line.h"
#include "spline.h"
#include "image.h"
#include "land.h"
#include "fox/font.h"
#include "texel.h"
#include "keyb.h"
#include "edit.h"
#include "carte.h"
#include "home.h"
#include "make.h"


void ex_entry_lamb ( int, string );
void ex_entry_cartesian ( int, string, void*);
void ex_entry_home ( int, string, void* );

extern void ex_entry_comtab ( int, string );
extern void ex_entry_editor ( int, string );

int byte_index ( string& str, int index );
int position_index ( string& str, int index );

using namespace std;

//int nprog ( int );

unsigned long lamb_c::type = LAMB;

int flag1;
int parrot1 = 0 ;

line_c linex, liney, linez;
line_c nline;

extern land_c land;

image_c image;

long flag = 1;

int figure = CARTESIAN;

namespace command_table {
void construct_ct (lamb_c& lamb );
}

lamb_c::lamb_c (): 
	pointer ( 3, 1 ),
	local_position ( 3, 1 ),
	verbal ( true ),
	shift_rotate ( true ),
	keyboard_mouse ( false ),
	is_printing ( false ),
	hook (nullptr ),
	command_number ( COMPILE_SNOW )
{
	cout << "lamb_c construction" << endl;
	command_table::construct_ct ( *this );

	mouth.parent = this;
	string pathlamb = get_lambs_path ();
	string lambcfg = pathlamb + "/config/lambcfg";
	echo << "lambs path: " <<  pathlamb << endl;
	is_cursor_captured = false;
	movement = _SHIFT_OBJECT;
	ifstream file ( lambcfg );
	string s1;
	string s2;
	string country;
	if ( file.is_open ()){
		cout << "load lamb's configuration file " << endl;
		file >> eyes.width;
		file >> eyes.height;
		file >> eyes.frame_x;
		file >> eyes.frame_y;
		file >> eyes.frame_width;
		file >> eyes.frame_height;
		getline ( file, s1 );
		getline ( file, country );
		getline ( file, home );
		getline ( file, directory );
		getline ( file, grf_file );
		getline ( file, project );
		echo << "work directory: " << directory << endl;
		echo << "grafic file: " << grf_file << endl;
		echo << "project: " << project << endl;
		getline ( file, s1 );
		if ( start_command == "" ){
			start_command = s1;
		}
//		echo << "start command: " << start_command << endl;
		string coms;
		string ef;
		getline ( file, coms );
		if ( coms == ":ed_mem" ){
			for (;!file.eof ();){
				getline ( file, ef );
				if ( ef == ":end" ){
					break;
				}
				ed_mem.push_back ( ef );
			}
		}
	}
	else {
		eyes.width = 200;
		eyes.height = 100;
		home = "blue";
	}
	file.close ();
	keyboard.set_layout ( country );
	font_c::init ();
	mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});
	
	eyes.set_size (*this, eyes.width, eyes.height );
	motor = &mouth;
	motors.push_back (&mouth );
	motors.push_back ( this );
}

lamb_c::~lamb_c ()
{
	string path = get_lambs_path ();
	ofstream file ( path+"/config/lambcfg" );
	file << eyes.width << endl;
	file << eyes.height << endl;
	file << eyes.frame_x << endl;
	file << eyes.frame_y << endl;
	file << eyes.frame_width << endl;
	file << eyes.frame_height << endl;
	file << keyboard.country << endl;
	file << home << endl;
		
	directory = getenv ("PWD" );	
	file << directory << endl;
	file << grf_file << endl;	
	if ( project == "" ){
		project = "comed";
	}
	file << project << endl;
	if ( start_command == "" ){
		start_command = "comed";
	}
	file << start_command << endl;
	file << ":ed_mem" << endl;
	for ( auto s : ed_mem ) {
		file << s << endl;
	}
	file << ":end" << endl;
	font_c::done ();
}

void lamb_c::draw ( lamb_c& lamb )
{
	mouth.draw ( lamb );
	status.draw (lamb );
	eyes.draw ( lamb );
	
	bool roll = true;
	texel_caret_c* tc = ears.get_caret ();
	size_t pos = tc -> get_position (),
	pos_end = tc -> get_last_position (),
	sel_inx = ears.selector_index;

	if ( pos != pos_end   ) {
		roll = false;
	}
	ears.set_texels ( echo.buffer );

	if ( roll == true ) {
		pos_end = tc -> get_last_position () ;
		tc -> move_to ( pos_end );
		ears.selector_index = pos_end;
		ears.fit_scrolls ();
	}		
	else {
		tc -> move_to ( pos );
		ears.selector_index = sel_inx;
	}
	ears.draw ( lamb );
} 

void lamb_c::local ( object_c& object, matrix_c<FT> vA, matrix_c<FT> vb, matrix_c<FT> vx )
{
	object.motion.set_object_vector ( 1, view_to_world ( vA ));
	object.motion.set_object_vector ( 2, view_to_world ( vb ));
	object.motion.set_object_vector ( 3, view_to_world ( vx ));
}

void lamb_c::rotatesel ( int lr, int du, int rlr )
{
	matrix_service_c<FT> matrix_service;
	matrix_c<FT> m = matrix_service.rotate3 ( du, lr,-rlr );	
	matrix_c<FT> A = motion.object_base ();
	matrix_c<FT> center = motion.object_vector ( 1 );
	center = A*mx*pointer + center;
	matrix_c<FT> R = A*mx*m*~mx*~A;

	matrix_c<FT> vb, vx;
	list<object_c*>::iterator iter;
	object_c* obj;
	line_c* pline;
	iter = land.llist.begin ();
	for (; iter != land.llist.end (); iter++){
		obj = *iter;
		if ( obj->get_type () == LINE && obj->state == 1 ){
			pline = (line_c*)obj;
			matrix_c<FT> vA = pline->motion.object_vector ( 1 );
			vA = R*(vA-center ) + center;
			matrix_c<FT> B = pline->motion.object_base ();
			matrix_c<FT> base = ~B*R*B;
			vb = base.get_column ( 1 );
			vx = base.get_column ( 2 );
			pline->motion.set_object ( vA, vb, vx );
		}	
	}
}

void lamb_c::rotate ( int lr, int du, int rlr )
{	
	//x' = Ai*x, x'' = Bi*x' => x'' = Bi*Ai*x <=> x'' = (A*B )i*x
	int step = 1;
	matrix_service_c<FT> matrix_service;
	matrix_c<FT> m;
	m = matrix_service.rotate3 ( du*step, lr*step,-rlr*step );	
	matrix_c<FT> base = motion.object_base ();
	base = base*mx*m*~mx;
	matrix_c<FT> vA, vb, vx;
	vA = motion.object_vector ( 1 );	
	vb = base.get_column ( 1 );
	vx = base.get_column ( 2 );
	motion.set_object ( vA, vb, vx );
	list<object_c*>::iterator it;
	it = land.llist.begin ();
	for (; it != land.llist.end (); it++){
		if ((*it )->get_type () == EDITOR ){
			editor_c* pe = (editor_c*)*it;
			pe->clear_pixel_vectors ();
		}
	}	
}

void lamb_c::copy_selection ()
{
	list<object_c*>& l = land.llist;
	list<object_c*>::iterator it = l.begin ();
	for (; it != l.end (); it++){
		if ( (*it )->state == 1 ){
			object_c* object = (*it )->duplicate ();
			if ( object != 0 ){
				l.insert ( it, object );
				object->state = 1;		
				(*it )->state = 0;
			}
		}
	}	
}

void lamb_c::shift_selection ( int lr, int du, int bf )
{
	object_c* obj;
	line_c* pline;
	list<object_c*>::iterator iter;
	iter = land.llist.begin ();
	matrix_c<FT> v ({(float )lr,(float )du,(float )bf});

	for (; iter != land.llist.end (); iter++){
		obj = *iter;
		if ( obj->state == 1 ){
			obj->shift ( v );
		}
		if ( obj->get_type () == LINE && obj->state == 1 ){
			pline=(line_c*)obj;
//			matrix_c<FT> v ({(float )lr,(float )du,(float )bf});
			pline->vA = pline->vA + v;
		}
		else if ( obj->get_type () == EDITOR && obj->state == 1 ){
			editor_c* pedit;
			pedit = (editor_c*)obj;
//			matrix_c<FT> v ({(float )lr,(float )du,(float )bf});
			matrix_c<FT> vv;
			vv = pedit->motion.object_vector ( 1 );
			vv = vv + v;
			pedit->motion.set_object_vector ( 1, vv );			
		}
	}
}

void lamb_c::shift ( int lr, int du, int bf )
{
	int step = 10;
	matrix_c<FT> v ({(float )lr*step,(float )du*step,(float )bf*step});
	matrix_c<FT> v0 ({0, 0, 0});
	v = view_to_world ( v );
	v0 = view_to_world ( v0 );
	v = v - v0;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	vA = vA + v;
	motion.set_object_vector ( 1, vA );
}

void lamb_c::shift ( object_c* object, int lr, int du, int bf )
{
	echo << "lamb shift2" << endl;
	int step = 10;
	matrix_c<FT> v ({(float )lr*step,(float )du*step,(float )bf*step});
	matrix_c<FT> v0 ({0, 0, 0});
	v = view_to_world ( v );
	v0 = view_to_world ( v0 );
	v = v - v0;
	if ( object != 0 ){
		echo << "not zero" << endl;
		object->shift3 ( v );
	}
}

void lamb_c::shift3 ( matrix_c<FT>& v )
{
	echo << "lamb shift3" << endl;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	vA = vA+v;
	motion.set_object_vector ( 1, vA );
}

void lamb_c::point ( int lr, int du, int bf )
{
	pointer.l[0]+= lr;
	pointer.l[1]+= du;
	pointer.l[2]+= bf;
}

void lamb_c::move ( int type, int nx, int ny, int nz )
{
	int mmx = nx;
	int my = ny;
	int mz = nz;
	switch ( type ){
	case _ROTATE:
		rotate ( mmx, my, mz );
		break;
	case _SHIFT:
		shift ( mmx, my, mz );
		break;
	case _POINT:
		point ( mmx, my, mz );
		if ((pointer[1] < -eyes.width/2 ) || (pointer[1] > eyes.width/2 ) 
		|| (pointer[2] < -eyes.height/2 ) || (pointer[2] > eyes.height/2 )){
			release_pointer ();
			break;
		}
		break;
	case _ROTATE_OBJECT:
		rotatesel ( mmx, my, mz );
		break;
	case _SHIFT_OBJECT:
		shift_selection ( mz, mmx, my );
		break;
	}
	stringstream ss;
	matrix_c<FT>vA, base;
	FT np_x, n_b, n_x, n_A;
	object_c::motion.get_object (&vA,&base,&np_x,&n_b,&n_x,&n_A );
	vA.out ( 0, ss );
	base.out ( 0, ss );
	local_position.out ( 0, ss );
	mx.out ( 0, ss );	
	pointer.out ( 0, ss );
}

matrix_c<FT> lamb_c::view_to_world ( matrix_c<FT>& pt ){
	matrix_c<FT> point = pt;
	point = mx*point;
	point = point+local_position;
	matrix_c<FT> base = motion.object_base ();
	point = base*point;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point+vA;
	return point;
}

matrix_c<FT> lamb_c::world_to_view ( matrix_c<FT>& pt ){
	
	matrix_c<FT> point = pt;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point-vA;
	matrix_c<FT> base = motion.object_base ();
	point = ~base*point;
	point = point-local_position;
	point = ~mx*point;
	return point;
}

matrix_c<FT> lamb_c::view_to_object ( object_c& object, matrix_c<FT>& m ){
	matrix_c<FT> point = m;
	point = mx*point;
	point = to_object ( object, point );
	return point;
}

matrix_c<FT> lamb_c::view_from_object ( object_c& object, matrix_c<FT>& m )
{
	matrix_c<FT> point = m;
	point = from_object ( object, point );
	point = ~mx*point;
	return point;
}

void lamb_c::matrices_to_view ( object_c& object, matrix_c<FT>* base, matrix_c<FT>* translation )
{
	matrix_c<FT> a, b, c, A, B, C;
	a = object.motion.object_vector ( 1 );
	A = object.motion.object_base ();
	b = motion.object_vector ( 1 );
	B = motion.object_base ();
	c = local_position;
	C = mx;
	*base = ~A*B*C;
	*translation = ~A*((B*c ) + b - a );
}

void lamb_c::coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag)
{
	if ( o -> get_type () == EDITOR ) {
		//lbase = motion.object_base ();
		matrix_c < FT > base = motion.object_base ();
		matrix_c < FT > vA = motion.vA;
		matrix_c < FT > obase = o -> motion.object_base ();
		matrix_c < FT > ovA = o -> motion.vA;
		//matrix_c < FT > m = { {
		lbase = base*obase;
		lvA = vA + base*ovA;
		flag = 0;
	}
};

void export_image2 ( lamb_c& lamb, string path, int zoom )
{
	if ( path.size () < 5 ){
		return;
	}
	int sysres = 0;
	string suffix = path.substr ( path.size () - 4, 4 );
	if ( suffix != ".ppm" && suffix != ".png" ){
		return;
	}
	string s = path.substr ( 0, path.size () - 4 );
	
	ofstream fout ( s + ".ppm" );
	
	lamb.is_printing = true;	
	zoom = 4;
	lamb.eyes.zoom = zoom;	
	lamb.resize_fonts ( zoom );
	
	long w = lamb.eyes.frame_width*lamb.eyes.zoom + 1;
	long h = lamb.eyes.frame_height*lamb.eyes.zoom + 1;
	
	matrix_c<FT> m ({0, (float )lamb.eyes.frame_x, (float )lamb.eyes.frame_y});
	lamb.local_position = lamb.local_position + m;
	
	lamb.eyes.width = w;
	lamb.eyes.height = h;
	lamb.eyes.push_surface ( lamb );	
	land.impress ( lamb );
	lamb.is_printing = false;
	fout << "P6" << endl;
	fout << w << endl;
	fout << h << endl;
	fout << 255 << endl;
	
	cout << w << ":" << h << endl;
	for ( int y = (h-1 )/2; y >= -h/2;y--){
		for ( int x = -w/2; x<w-w/2; x++){
			long color = lamb.eyes.get_pixel ( x, y );
				fout.write ((char*)& color, 3 );
		}
	}
	fout.close ();
	lamb.eyes.change_surface ( lamb, 1 );
	lamb.resize_fonts ( lamb.eyes.zoom );
	if ( suffix!=".png" ){
		return;
	}
	string command;
	command = "pnmtopng " + s + ".ppm " + "> " + s + ".png";
	sysres = system ( command.c_str ());
	cout <<  sysres << endl;
	echo << "export image" << endl;
}

void save_file ( string str )
{
	echo << "save file " << endl;
	fstream sfile ( str + ".grf", ios::out );
	echo << str + ".grf" << endl;
	for ( auto o : land.llist ){
		o->serialize ( sfile, true );
	}
	sfile.close ();
	land.get_lamb () -> grf_file = str; 
}

void load_file ( string str )
{
	object_c* pobj;
	land.get_lamb () -> grf_file = str;
	if ( str == "" ) {
		return;
	}
	ifstream ifile ( str + ".grf" );
	if ( ! ifile.good () ){
		echo << "can't open file:" << str + ".grf" << endl;
		return;
	}
	ifile.close ();
	fstream file ( str + ".grf" );
	echo << "load grafic file: " <<  str + ".grf" << endl;

	while ( file.good ()){
		uint32_t ty;
		string s;		
		file >> s;
		if (s == "EDITOR" ){
			ty = EDITOR;
			pobj = object_c::dyncreate ( ty );
			((editor_c*)pobj )->simple_text = true;
			pobj->serialize ( file, false );
			FT x, y;
			land.llist.push_back ( pobj );
			echo << " new editor. " << pobj->get_type () << endl;
		}
		else if ( s == "SPLINE" ){
			pobj =  new spline_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
		else if ( s == "CARTESIAN" ){
			pobj =  new cartesian_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
		else if ( s == "IMAGE" ) {
			pobj =  new image_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
	}
	file.close ();
}

void new_cartesian ()
{
	cartesian_c* cartesian = new cartesian_c (); 
	
	lamb_c& lamb = *land.get_lamb ();
	lamb.local ( *cartesian, lamb.pointer, { 0, 0, -1 }, { 1, 0, 0 } );
	cartesian -> vA = { 0, 0 ,0 };
	cartesian -> vvx = { 0, 100, 0 };
	cartesian -> vvy = { 0, 0, 100 };
	cartesian -> vvz = { 100, 0,  0};
	matrix_c < FT > vvA = { 100, 0, 0 }, vvb = { 0, 1, 0 }, vvx = { 0, 0, 1 };
	cartesian -> tx.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 100, 0 };
	cartesian -> ty.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 0, 100 };
	cartesian -> tz.motion.set_object ( vvA, vvb, vvx );
	land.llist.push_front ( cartesian );
	lamb.sensor = cartesian;
}

void new_spline ()
{
	lamb_c& lamb = *land.get_lamb ();
	spline_c* spline = new spline_c (); 
	matrix_c<FT> m = lamb.pointer;
	m = lamb.view_to_world ( m );
	spline->control_points = { m};
	spline->order = 1;	
	spline->uniform = true;
	spline->open_uniform = true;
	spline->color = 0x0000ff;
	spline->doted = false;
	spline->accept_points = true;	
	land.llist.push_front ( spline );
	lamb.sensor = spline;
}

void new_editor ()
{
	echo << "new editor" << endl;
	editor_c* p_editor = new editor_c;
	p_editor->simple_text = true;
	lamb_c* pl = land.get_lamb ();
	pl -> local (*p_editor, pl -> pointer, {1, 0, 0}, {0, 1, 0} );
	p_editor->frame_height = 2;
	p_editor->frame_width = 10;
	land.llist.push_front ( p_editor );
}

void new_image ()
{
	echo << "new image" << endl;
	image_c* p_image = new image_c;
	lamb_c* pl = land.get_lamb ();
	pl -> local (*p_image, pl -> pointer, {0, 0, -1}, {1, 0, 0} );
	p_image->rows = 50;
	p_image->columns = 100;
	land.llist.push_front ( p_image );
}

void ffind ( lamb_c& lamb, matrix_c<FT> pt )
{
	object_c* pobj;	
	list<object_c*>::iterator it;
	uint32_t m = lamb.keyboard.get_controls ();
	for ( it = land.llist.begin (); it!= land.llist.end (); it++){
		pobj = *it;
		if ( pobj->find ( lamb, pt )){
			if ( m == LL_KA_LC ){
				lamb.sensor = *it;
			}
			else{
				if ( pobj->state != 0 ){
					pobj->state = 0;
				}
				else{ 
					pobj->state = 1;
				}
			}
		}
	}
}

void lamb_c::grab_pointer ( int x , int y )
{
	pointer[1] = x-eyes.width/2;
	pointer[2] = eyes.height/2-y;
}

void lamb_c::release_pointer ()
{
	int x = pointer[1] + eyes.width/2;
	int y = eyes.height/2 - pointer[2];
	ungrab_system_cursor ( x, y );
}

void lamb_c::mouse_move ( int x, int y )
{
	mouse.inertia (&x, &y, 0 );
	int step = 5;
	move ( movement, x, y, 0 );
	land.impress ( *this );
	eyes.valide = false;				
}

void lamb_c::idle ()
{
	if (!eyes.valide ){
//		cout  << "idle and invalide" << endl;
		land.impress ( *this );	
	}
	else {
//		echo << "idle but valide" << endl;
	}
}

void lamb_c::write_status ()
{
	status.clear ();
	stringstream ss;
	matrix_c<FT> m = motion.object_base ();
	m.out ( 3, ss );
	m = ~motion.object_vector ( 1 );
	m.out ( 3, ss );
	ss << "pointer world ";	
	m = ~view_to_world ( pointer );
	m = land.rasterize ( m );
	m.out ( 1, ss );
	ss << "pointer z:" << pointer [3] << endl;
	ss << "raster: " << land.raster;
	status.set_texels ( ss.str () );
}

void lamb_c::key_event ( bool pressed, unsigned short sym, unsigned char c )
{
	write ( pressed, sym );
}

void lamb_c::expose ( int* px, int* py, char** pcp, retine_c** retine )
{
	*px = eyes.width;
	*py = eyes.height;
	*pcp = 0;
	*retine = &eyes.retine;
}

void lamb_c::focus ()
{
	keyboard.stroke_list.clear ();	
}

void lamb_c::button_pressed ( int number )
{
	if (number == 4 || number == 5 ){
		int s = number == 4 ? -1 : 1;
		move ( movement, 0, 0, s );
		land.impress (*this );
	}
	else if ( number == 1 ){
		if ( figure == SPLINE ){
			if ( sensor != 0 ){
				if ( !sensor->set_point (*this ) ){
					new_spline ();
//					sensor = land.llist.back ();
				}
			}
			else{
				new_spline ();
			}
//			pointer = land.rasterize ( pointer );
//			nline.setpoint ( *this, false, false, true, false );
		}
		else if ( figure== CARTESIAN ){
			new_cartesian ();
		}
		else if (figure == EDITOR ) {
			new_editor ();
		}
		else if ( figure == IMAGE ) {
			new_image ();
		}
	}
	else if ( number == 2 ){
		echo << "button 2" << endl;
		ffind (*this, pointer );
	}
}

void lamb_c::config_change ( int x, int y )
{
	eyes.set_size ( *this, x, y );
	home_c ( *this, home );
	for ( auto object : land.llist ){
		object->invalide_visual ( 0 );
	}
	parrot1++;
}

void lamb_c::compile_and_restart ( bool clean )
{
	echo.clear ();

	editor_c* peditor;
	auto it  = land.llist.begin ();
	for (;it!= land.llist.end ();it++){
		object_c* po = *it;
		if ( po->get_type () == EDITOR &&  po != &mouth ){
			peditor = (editor_c*)*it;
			break;
		}	
	}
	editor_c& edit = *peditor;
	string path, make, exe;
	path = get_lambs_path ();
	ifstream f ( path+"/config/environment" );
	f >> make;
	f >> exe;
	f.close ();
	cout << make << endl;
	cout << path << endl;
	cout << exe << endl;
	string command;
	stringstream ss;
	if ( clean ){
		command = "cd "+ path + " && make -f " + make + " EXE=" + exe + "_out clean";
		system_echo ( command, ss );
		echo << ss.str ();
		land.impress (*this );
		return;
	}
	
	command = "cd "+ path + " && make -j6 -f " + make + " EXE=" + exe + "_out";
	echo << "file path" << endl << edit.file_path << endl;
	
	ss << "comed " << edit.file_path << " " << edit.scroll_down << " " <<  edit.get_caret ()->get_position ();
	start_command = ss.str ();
	ss.seekp ( 0 );
	system_echo ( command, ss );
	string s1, s2;
	bool bmv = true;
	bool flag_ok = true;
	while ( ss.good ()){
		getline ( ss, s1 );
		if ( string::npos!= s1.find ("error" )){
			flag_ok = false;
		}
		else if ( string::npos!= s1.find ("Error" )){
			flag_ok = false;
		}
	}
	
	if ( flag_ok == false ){
		string str = ss.str ();
		echo.buffer = str;
		ears.set_texels ( str );
		string find;
		string search ("error" );
		size_t indez = 0;
		ears.regexp.find ( str, indez, 0, search, find );
		texel_caret_c* tc = ears.get_caret ();
		size_t position = position_index ( str, indez );
		cout  << "pos:" << position << endl;
		tc->move_to ( position+1 );
		ears.selector_index = position+find.size ()+2;
		ears.scroll_down = 3000;
		ears.fit_scrolls ();
		ears.scroll_down -= 10;
	}
	else {
		release_pointer ();
		exit ( wcp::restart );
	}		
	ears.valide = 0;
	land.impress (*this );
	return;
}

void lamb_c::resize_fonts ( float f )
{
	list<object_c*>::iterator it;
	it = land.llist.begin ();
	float ff;
	for (;it!= land.llist.end ();it++){
		if ((*it )->get_type () == EDITOR ){
			if ((*it )->parent == 0 || is_printing ){
				ff = f;
			}
			else {
				ff = 1;
			}
			editor_c* pe = (editor_c*)*it;
			pe->clear_pixel_vectors ();
			pe->clear_glyph_bitmap ();
//			pe->clear_texel_map ();
			pe->font.set_face ( ff );
		}
	}	
			
}

int lamb_c::command2 ( unsigned char c, string& m, string& s )
{
	return command ( s );
}

bool lamb_c::command ( string str )
{
	static editor_c* editor;
	
	stringstream ss ( str );
	stringstream nss ( str );
	string ns;
	nss >> ns;
	string s, s1 , s2 , s3;
	ss >> s >> s1 >> s2 >> s3;
	if ( s == "sc" ){
		echo << "start c:" ;
		command ( start_command );
	}
	if ( s == "cd" ){
		change_pwd ( s1 );
		char* pch = getenv ("CWD" );
		if ( pch!= 0 ){
			directory = string ( pch );
		}
		echo << directory << endl;
	}
	if ( s == "project" ){
		if ( s1!="?" ){
			project = s1;
			echo << "test:" << s1 << endl;
		}
		echo << project << endl;
		land.impress (*this );
	}
	if ( s == "comed" ){
		if ( s1!="" ){
			string ss;
			ss = "ed "+ s1+" "+ s2 + " " + s3;
			command ( ss );
		}
	}
	if ( s == "ed" ){
		auto it = land.llist.begin ();
		for (;it != land.llist.end ();it++){
			object_c* po = *it;
			if ( po->get_type () == EDITOR  && po!=&mouth ){
				editor = (editor_c*)*it;
				break;
			}	
		}
		if ( DEBUG ){
			editor->state = 1;
		}
		bool file_is_ok = true;
		if (file_is_ok ){
			editor->file_path = s1;
			if ( s1!="" ){
				editor->import_text ( s1 );
			}
			stringstream ss;	
			if ( s2!="" ){
				if ( s3 == "" ){
					stringstream ss;
					ss << s2;
					int lineNo;
					ss >> lineNo;
					editor->get_caret ()->to_row ( lineNo );
					editor->selector_index = editor->get_caret ()->get_position ();
					editor->scroll_down = lineNo-5;
					if ( editor->scroll_down<0 ){
						editor->scroll_down = 0;
					}
				}
				else{
					stringstream ss;
					ss<<s2;
					int scrollNo;
					ss>>scrollNo;
					editor->scroll_down = scrollNo;
					ss.clear ();
					ss << s3;
					int posNo;
					ss >> posNo;
					editor->get_caret ()->move_to ( posNo );
					editor->selector_index = posNo;
				}
			}
		}	
	}

	if ( s == "t1" ){
		stringstream ss;
		ss << s1 << " " << s2;
		int x, y;
		ss >> x;
		ss >> y;
		test ( x, y );
	}

	if ( s == "e" ){
		int i1, i2;
		string s;
		nss >> i1 >> i2 >> s;
		test4 ( i1, i2, s );
	}
	if ( s == "ip" ){
		is_printing =! is_printing;
		resize_fonts ( eyes.zoom );
	}
	if ( s == "base" ){
		land.show_base=!land.show_base;
	}   
	if ( s == "cle" ){
		echo.clear ();
	}
	if ( s == "ras" ){
		land.raster = land.raster == 0?10:0;
	}    
	if ( s == "bp" ){
		flag = !flag;
	}
	if ( s == "d" ){
		echo << "remove selection" << endl;
		land.remove_selection ();
	}
	if ( s == "ase" ){
		echo << "new editor" << endl;
		new_editor ();
	}
	if ( s == "ass" ){
		echo << "new spline" << endl;
		new_spline ();				
	}
	if ( s == "ws" ){
		save_file ( s1 );
		echo << s1 << endl; 
	}
	if ( s == "wl" ){
		load_file ( s1 );
	}
	if ( s == "z" ){
		stringstream ss ( s1 );
		ss >> eyes.zoom;
		resize_fonts ( eyes.zoom );
	}
	if ( s == "rf" ){
		stringstream ss ( s1 );
		float f;
		ss >> f;
		resize_fonts ( f );
	}
	if ( s == "xg" ){
		export_image2 (*this, s1, 3 );
		echo << s1 << endl; 
	}
	if ( s == "fig" ){
		string s1;
		nss >> s1;
		if ( s1  == "sp" ){
			figure = SPLINE;
		}
		else if ( s1 == "ed" ){
			figure = EDITOR;
		}
		else if ( s1 == "ca" ) {
			figure = CARTESIAN;
		}
		else if ( s1 == "im" ) {
			figure = IMAGE;
		}
	}
	if ( s == "v" ){
		if ( s1 == "" ){
			eyes.push_surface (*this );
		}
		else{
			stringstream ss;
			ss << s1;
			int i;
			ss >> i;
			eyes.change_surface (*this, i );
		}	
	}
	if ( s == "w" ){
		movement = movement<3?movement+1:0;
	}
	if ( s == "q" ){
	}
	else if ( s == "cp" ){
		copy_selection ();
	}
	else if ( s == "ho" ){
		home = s1;
		home_c (*this, s1 );
		land.impress (*this );
	}
	else if ( s == "h0" ){
		mx={{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
		motion.set_object_vector ( 1,{0, 0, 0});
		motion.set_object_vector ( 2,{1, 0, 0});
		motion.set_object_vector ( 3,{0, 1, 0});
		list<object_c*>::iterator it;
		it = land.llist.begin ();
		for (;it!= land.llist.end ();it++){
			if ((*it )->get_type () == EDITOR ){
				editor_c* pe = (editor_c*)*it;
				pe->clear_pixel_vectors ();
				pe->clear_texel_map ();
			}
		}	

		pointer = {0, 0, 0};
		local_position = {0, 0, 0};
	}
	else if ( s == "h1" ){
		mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
		pointer = {0, 0, 0};
		local_position = {-310, 190, 0};
	}

	else if ( ns == "raster" ){
		nss >> land.raster;
	}
	else if ( ns == "mouse" ){
		nss >> keyboard_mouse;
		echo << "mousx";
	}
	else if ( ns == "myt" ){
		int i;
		string s;
		nss >> i >> s;
		test2 ( i );
	}
}

void lamb_c::change_movement ( int para1, int para2 )
{
	if ( para2 == 7 ){
		keyboard_mouse = !keyboard_mouse;
	}
	else if ( para2 == 2 ){
		movement = movement<_POINT?++movement:_SHIFT;
	}
	else if ( para2 == 1 ){
		movement = movement>0?--movement:_POINT;
	}
	else if ( para2 == 5 ){
		object_c* &po = sensor;
		if ( po->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)po;
			ed->export_text ( ed->file_path );
		}
		if ( po->get_type () == EDITOR ){
			editor_c* ed = (editor_c*)po;
			ed->import_text ( ed->file_path, true );
		}
	}
	land.impress (*this );	
}

void lamb_c::to_default ( int a, string s )
{
	mx={{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});
	list<object_c*>::iterator it;
	it = land.llist.begin ();
	for (;it!= land.llist.end ();it++){
		if ((*it )->get_type () == EDITOR ){
			editor_c* pe = (editor_c*)*it;
			pe->clear_pixel_vectors ();
			pe->clear_texel_map ();
		}
	}	

	pointer = {0, 0, 0};
	local_position = {0, 0, 0};
	eyes.zoom = 1;
	resize_fonts ( eyes.zoom );
	land.impress (*this );
}

void lamb_c::init ()
{
	home_c (*this,"blue" );
	command ( start_command );
	sensor = this;
	movement = _POINT;
	load_file ( grf_file );
}

bool condition ( int category )
{
	lamb_c& lamb = *land. get_lamb ();
	if ( category == 0 ){
		return true;	
	}
	else if ( category == 1 ){
		if ( lamb.sensor == &lamb ||
		lamb.sensor == &lamb.status ||
		lamb.sensor == &lamb.mouth ||
		lamb.sensor == &lamb.eyes ||
		lamb.sensor == &lamb.ears   
		){
			return false;
		}
		else{
			return true;
		}
	}
	else if ( category == 2 ){
	if ( lamb.sensor == &lamb ||
		lamb.sensor == &lamb.status ||
		lamb.sensor == &lamb.mouth ||
		lamb.sensor == &lamb.eyes ||
		lamb.sensor == &lamb.ears  
		){
			return true;
		}
		else{
			return false;
		}
	}
}

list<object_c*>::iterator
lamb_c::next_object ( list<object_c*>& l, object_c*& sou, int category )
{
	list<object_c*>::iterator it, it_begin;
	it_begin = it = std::find ( l.begin (), l.end (), sou );
	if ( it != l.end ()){
		do{
			it++;
			if ( it == l.end ()){
				it = l.begin ();
			}
			sou = *it;
		}while ( it != it_begin && condition ( category ) == false );
	}
}

list<object_c*>::iterator 
lamb_c::previous_object ( list<object_c*>& l, object_c*& sou, int category )
{
	list<object_c*>::reverse_iterator it, it_begin;
	it_begin = it = std::find ( l.rbegin (), l.rend (), sou );
	if ( it != l.rend ()){
		do{
			it++;
			if ( it == l.rend ()){
				it = l.rbegin ();
			}
			sou = *it;
		}while ( it != it_begin && condition ( category ) == false );
	}
}

#define step 10

namespace command_table{

command_table_c< lamb_c >* pcontactil = 0;
command_table_c<lamb_c>* pmovil = 0;

void construct_ct ( lamb_c& lamb ){

pcontactil = new command_table_c<lamb_c> ({
COMMAND, XK_slash,		change_movement, 0, 7,
COMMAND, XK_comma,		change_movement, 0, 1,
COMMAND, XK_period,		change_movement, 0, 2
} ) ;

command_table_c<lamb_c>::ctl2.insert 
( pair< pair < int, lamb_c* >, command_table_c < lamb_c > * > ( { 1, &lamb }, pcontactil ) );

pmovil = new command_table_c<lamb_c>  ({
SHIFT, XK_h,		shift,&lamb.sensor,-1, 0, 0,
SHIFT, XK_l,		shift,&lamb.sensor, 1, 0, 0,
SHIFT, XK_j,		shift,&lamb.sensor, 0, 0,-1,
SHIFT, XK_k,		shift,&lamb.sensor, 0, 0, 1,
SHIFT, XK_i,		shift,&lamb.sensor, 0, 1, 0,
SHIFT, XK_m,		shift,&lamb.sensor, 0,-1, 0,

ROTATE, XK_h,	move, _ROTATE,-1, 0, 0,
ROTATE, XK_l,	move, _ROTATE, 1, 0, 0,
ROTATE, XK_j,	move, _ROTATE, 0, 0,-1,
ROTATE, XK_k,	move, _ROTATE, 0, 0, 1,
ROTATE, XK_i,	move, _ROTATE, 0, 1, 0,
ROTATE, XK_m,	move, _ROTATE, 0,-1, 0,

POINT, XK_l, 		move, POINT, step, 0, 0,
POINT, XK_j,		move, POINT, 0, 0, -step,
POINT, XK_k,		move, POINT, 0, 0,+step,
POINT, XK_i,		move, POINT, 0,+step, 0,
POINT, XK_m,		move, POINT, 0,-step, 0,
POINT, XK_h,		move, POINT,-step, 0, 0,

SHIFT_OBJECT, XK_h,		move, SHIFT_OBJECT,-step, 0, 0,
SHIFT_OBJECT, XK_l,		move, SHIFT_OBJECT, step, 0, 0,
SHIFT_OBJECT, XK_j,		move, SHIFT_OBJECT, 0, 0,-step,
SHIFT_OBJECT, XK_k,		move, SHIFT_OBJECT, 0, 0,+step,
SHIFT_OBJECT, XK_i,		move, SHIFT_OBJECT, 0,+step, 0,
SHIFT_OBJECT, XK_m,		move, SHIFT_OBJECT, 0,-step, 0,
ROTATE_OBJECT, XK_h,	move, ROTATE_OBJECT,-step, 0, 0,
ROTATE_OBJECT, XK_l,		move, ROTATE_OBJECT, step, 0, 0,
ROTATE_OBJECT, XK_j,		move, ROTATE_OBJECT, 0, 0,-step,
ROTATE_OBJECT, XK_k,		move, ROTATE_OBJECT, 0, 0,+step,
ROTATE_OBJECT, XK_i,		move, ROTATE_OBJECT, 0,+step, 0,
ROTATE_OBJECT, XK_m,	move, ROTATE_OBJECT, 0,-step, 0,

POINT, XK_y,				button_pressed, 1,
POINT, XK_u,				button_pressed, 2,
POINT, XK_n,				button_pressed, 3,
POINT, XK_slash,			change_movement, 0, 7,
SHIFT, XK_slash,			change_movement, 0, 7,
ROTATE, XK_slash,			change_movement, 0, 7,
SHIFT_OBJECT, XK_slash,	change_movement, 0, 7,
ROTATE_OBJECT, XK_slash,	change_movement, 0, 7
} );

command_table_c<lamb_c>::ctl2.insert 
( pair < pair < int, lamb_c* >, command_table_c < lamb_c> * > ( { 2, &lamb }, pmovil ) );

}

}// end namespace command_table

void lamb_c::write ( bool is_pressed, uint16_t stroke ){
	
	
	if ( sensor == 0 ){
		echo << "active object is null\n";
		sensor = this;
	}
	keyboard.on_key ( is_pressed, stroke );

	if ( is_pressed == false ){
		return;
	}
	if ( hook != nullptr ) {
		hook -> edit (*this , keyboard );
		echo << "hook" << endl;
		if ( keyboard.get_char () == string ("e" )){
			hook = nullptr;
		}
		return;
	}
	if ( stroke == XK_F5 ){
		to_default ( 0,"" );
		return;
	}
	
	unsigned long v = keyboard.get_stroke ();
	string c = keyboard.get_char ();
	unsigned long m = keyboard.get_controls ();
	if ( keyboard_mouse ){
		command_table::command_table_c<lamb_c>::
			ssfind ( 2, this, COMMAND, ul_t ( m ), int ( v ), c );

		land.impress (*this );
		return;
	}
	int shema = 2;
	if (shema == 1 ){
		if (!edit (*this, keyboard )){
			object_c* object = sensor;
			if ( motor == &mouth ){
				object = &mouth;
			}
			object->edit (*this, keyboard );
		}
	}
	else if (shema == 2 ){
		object_c* object = sensor;
		if ( motor == &mouth ){
			object = &mouth;
		}
		if ( !object->edit ( *this, keyboard )){
			if ( object != this ){
				edit (*this, keyboard );
			}
			object->invalide_visual ( 0 );
			land.invalide_all_objects ( 0 );
		}
	}
	land.impress (*this );
	return;
}

int lamb_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	uint16_t v = keyboard.get_stroke ();
	string c = keyboard.get_char ();
	uint32_t m = keyboard.get_controls ();
	bool proceeded = false;	
	if ( m == LL_KA_RC && v == XK_s ){
		next_object ( motors, motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		land.impress (*this );		
		goto PROCEEDED;
	}
	else{
		if ( motors.front () != motor ){	
			motors.remove ( motor );
			motors.push_front ( motor );
		}
	}	
	if ( m == LL_KA_RC && (v == XK_space || v == XK_Tab )){
		if ( condition ( 1 )  ) { 
			if ( v == XK_space ){
				next_object ( land.llist, sensor, 1 );
			}
	  		else {
				previous_object ( land.llist, sensor, 1 );
			}
		}
		else{
			if ( sensor == &lamb.ears ) {
				sensor = &lamb.status;
			}
			else if ( sensor == &lamb.status ) {
				sensor = &lamb.eyes;
			}
			else if ( sensor == &lamb.eyes ) {
				sensor = &lamb;
			}
			else {
				sensor = &lamb.ears;
			}
		}
		land.invalide_all_objects ( 0 );
		land.impress (*this );		
		goto PROCEEDED;
	}
	else if ( m == LL_KA_RC && v == XK_a ) {
		if ( condition ( 2 ) ) {
			sensor = land.llist.front () ;
			next_object ( land.llist, sensor, 1 );
			echo << "a" << endl;
		}
		else{
			sensor = &lamb.ears;
		}
		land.invalide_all_objects ( 0 );
		goto PROCEEDED;
	}
	else{
		if ( condition ( 1 ) ) {
			if ( land.llist.front () != sensor ){
				land.llist.remove ( sensor );
				land.llist.push_front ( sensor );
			}
		}
	}
	assert ( command_table::pcontactil );
	command_table::command_table_c<lamb_c>::
		ssfind (1, this, COMMAND, ul_t ( m ), int ( v ), c );
	land.impress ( lamb );
	return false;
	PROCEEDED: return true;
}


int lamb_c::service ( string& s, string& s1, string& s2 )
{
	make_c make;
	if ( s == "make" ){
		make.make ( s1, s2 );
	}
	if ( s == "mkproper" ){
		make.mkproper ( s1, s2 );
	}
}

void lamb_c::test4 ( int d, int par, string s )
{
	void* vp = nullptr;
	switch ( d ){
	case 3:
		ex_entry_lamb ( par, s );
		break;
	case 4:
		ex_entry_comtab ( par, s );
		break;
	case 5:
		ex_entry_editor ( par, s );
		break;
	case 6:
		ex_entry_cartesian ( par, s, vp );
		break; 
	case 7:
		ex_entry_home ( par, s, vp );
		break;
	default:
	
		O ( test: no target )		
	}
}

void lamb_c::test3 ( object_c*, int par1, int par2, int par3 )
{
}

void lamb_c::test2 ( int par1 )
{
}

void lamb_c::test ( int par, string s )
{
	echo << "lamb_c::test:par" << par << " string:" << s << endl;
}

void lamb_c::test ( int par1, int par2 )
{
	echo << "lamb_c::test par1:"<< par1 << " par2:" << par2 << endl;

	using namespace std::chrono;

	steady_clock::time_point t1 = steady_clock::now ();
	land.impress (*this );

	steady_clock::time_point t2 = steady_clock::now ();

	duration<double> time_span = duration_cast<duration<double>>(t2 - t1 );
	echo << "It took me " << time_span.count () << " seconds." << endl;
	land.impress (*this );
	return;
}


int entry ( int argc, char *argv[] ) {

	land.llist.push_front ( new lamb_c );
	lamb_c& lamb = *land.get_lamb ();
	both_c b1;
	b1.state = 0;
	if ( argc == 2 ){
		string s = "comed ";
		s= s + argv[1] + " 0 1";
		lamb.start_command = s;
		cout << lamb.start_command;
	}
	flag1 = 2;
	message_c* pm;
	pm = get_machine ();	
	if ( pm == 0 ){
		cout << "machine fail" << endl;
		return EXIT_FAILURE;
	}
	lamb.that = pm;
	pm->that = &lamb;
	
	lamb.open ();
	
	cout << "exit code:"<< lamb.return_code << endl;
	return lamb.return_code;
}

namespace ex_lamb
{
void entry ( int a, string s ){
	if ( a == 1 ){
		O ( ex_lamb );
		spline_c* p  = new spline_c;
		p->control_points.push_back ({0, 0, 0});	
		p->control_points.push_back ({0, 50, 2000});
		land.llist.push_back ( p );
		p->state = 1;
	}
	else{
		auto it = land.llist.begin ();
		for (; it != land.llist.end (); it++){
			if ( (*it )->get_type () == SPLINE ){
				
			}
		}
		land.remove_selection ();
	}
};
	
}	//end namespace ex_lamb

void ex_entry_lamb ( int a, string s )
{
	ex_lamb::entry ( a, s );
}
