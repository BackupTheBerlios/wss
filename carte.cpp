#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <iostream>

#include "symbol/keysym.h"

#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "global.h"

#include "message.h"
#include "object.h"
#include "mouse.h"
#include "eyes.h"
#include "keyb.h"
#include "fox/edit.h"
#include "home.h"
#include "lamb.h"
#include "line.h"
#include "spline.h"
#include "land.h"
#include "carte.h"

extern land_c land;

const int cartesian_c::type = CARTESIAN;

void cartesian_c::coocked_to_local ( object_c* o, matrix_c < FT > & cbase, matrix_c <FT >& cA, int& flag){
		matrix_service_c < FT > mxsrv;
//		matrix_c < FT > lb = mxsrv.ortho3 (vvx, vvy, 0, 0, 0 );
		matrix_c < FT > lb = { vvx, vvy, vvz };
		lb = .01*lb;

		matrix_c < FT > lA = vA;
		matrix_c < FT > b0 = motion.object_base ();
		matrix_c < FT > A0 = motion.object_vector ( 1 );
		matrix_c < FT > ob = o -> motion.object_base ();
		matrix_c < FT > oA = o -> motion.object_vector ( 1 );
		lamb_c& lamb = *land.get_lamb ();
		matrix_c < FT > bb = lamb.motion.object_base ();
		matrix_c < FT > x = { { 0, 1, 0 }, { 0, 0, 1 }, { 1, 0, 0 } };
		cbase = bb * x;
		cA = ( b0 * lb * oA ) + ( b0 * lA ) + A0;			
		flag = 1;
}

void cartesian_c::object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA ) {
	matrix_service_c < FT > mxsrv;
	lbase = mxsrv . ortho3 ( vvx, vvy, 0, 0, 0 );	
	lvA = vA;
};


void cartesian_c::dump ()
{
	stringstream ss, s2;
	matrix_c < FT > m = tx.motion.object_vector ( 1 );
	matrix_c < FT > base = tx.motion.object_base ();
	matrix_c < FT > mx = { 0, 3, 4};
	mx.out ( 3, ss );
	m.out ( 3, ss );
	base.out ( 3, ss );
	echo << ss.str ();
}

void cartesian_c::locals_to_world ( vector < matrix_c < FT >  > & l )
{
	matrix_c < FT > b0 = motion.object_base ();
	matrix_c < FT > a0 = motion.object_vector ( 1 );
	matrix_service_c < FT > mxsrv;
	matrix_c < FT > b = mxsrv.ortho3 ( vvx, vvy, 0, 0, 0 );
	b = { vvx, vvy, vvz };
	b = .01*b;
	matrix_c < FT > a = vA;
	matrix_c < FT > base = b0*b;
	
	matrix_c < FT > A = a0 + b0*a;
	auto it = l.begin ();	
	for ( ; it != l.end(); it ++ ){
		*it = base*(*it) + A;
	}
}

cartesian_c::cartesian_c () :
	right_hand ( true ),
	dimension ( 2 )
{
	tx . set_texels ( "x" );
	tx . parent = this;
	tx . simple_text = true;
	ty . set_texels ( "y" );
	ty . parent = this;
	ty. simple_text = true;
	tz . set_texels ( "z" );
	tz . parent = this;
	tz . simple_text = true;
	
	tx.frame_height = 2;
	tx.frame_width = 10;
	ty.frame_height = 2;
	ty.frame_width = 10;
	tz.frame_height = 2;
	tz.frame_width = 10;
	matrix_c < FT > vvA = { 100, 0, 0 }, vvb = { 0, 1, 0 }, vvx = { 0, 0, 1 };
	tx.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 100, 0 };
	ty.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 0, 100 };
	tz.motion.set_object ( vvA, vvb, vvx );
}

cartesian_c* cartesian_c::create ()
{
    return new cartesian_c ();
}

int cartesian_c::edit ( lamb_c& lamb, keyboard_c& keyb)
{
	uint16_t v = keyb.get_stroke ();
	string c = keyb.get_char ();
	if ( c == "s" ) {
		state = ! state;
	}
	if ( c== "c" ) {
		if ( dimension == 3 ) {
			land.impress ( lamb );
			matrix_c < FT > swap = vvz;
			vvz = vvx, vvx = vvy, vvy = swap;
			dimension = 2;
		}
		else {
			matrix_c < FT > swap = vvz;
			vvz = vvy, vvy = vvx, vvx = swap;
			dimension = 3;
		}
	}
	if ( c == "p" ) {
		vvx = { 100, -100, - 100 };	
		vvx = 1/sqrt ( 2 )* vvx;
	}
	if (c == "q" ) {
		vvx = { 100, 0, 0 };
	}
	if ( c == "d" ) {
		dump ();
	}
	return 0;
}

void cartesian_c::draw ( lamb_c& lamb )
{
	float pi = 3.1814;
//	echo << "draw cartesian" << endl;
	matrix_c < FT > b0 = motion.object_base ();
	matrix_c < FT > a0 = motion.vA;
	matrix_service_c < FT > mxsrv;
	matrix_c < FT > b = mxsrv.ortho3 ( vvx, vvy, 0, 0, 0 );
	b = { vvx, vvy , vvz };
	b = .01*b;
	matrix_c < FT > a = vA;
	matrix_c < FT > base = b0*b;
	
	matrix_c < FT > A = a0 + b0*a;
	base = 100*base;
		
	vector < matrix_c < FT > > l = {
	};
	for ( float x = 0; x <= 100; x += 1 ) {
		float y;
		y = sqrt ( x );
		y = 25* cos ( 4*pi*x/100.0 );
		matrix_c < FT > m;
		m = { x, y, 0 };
		l.push_back ( m );
	}
	
	locals_to_world ( l );
	g.order = 2;
	g.control_points = l;
	g.draw ( lamb );	
	
	x . color = 0xff0000;
	y . color = 0xff00;
	z . color = 0xff;
	int32_t color = 0x00;
	if ( lamb.sensor == this ) {
		color |= 0xff00;
	}
	if ( state == 1 ) {
		color |= 0xff0000;
	}
	if ( state == 1 || lamb.sensor == this ) {
		x . color = color;
		y . color = color;
		z . color = color;
	}
	
	if ( dimension == 3 ) {
		tz.set_texels ( "z" );
	}
	else {
		tz.set_texels ( "" );
	}
	tx . draw ( lamb );
	ty . draw ( lamb );
	tz . draw ( lamb );
	tx . draw ( lamb );
	ty . draw ( lamb );
	tz . draw ( lamb );

	x.control_points = { 
		{ A },
		{A + base.get_column ( 1 )}
	};
	y.control_points = { 
		{ A },
		{ A + base.get_column ( 2 )}
	};
	if ( dimension == 3 ) {
		z.control_points = { 
			{ A },
			{ A + base.get_column ( 3 )}
		};
	}
	else {
		z.control_points = { 
		};
	}
	x.draw ( lamb );
	y.draw ( lamb );
	z.draw ( lamb );
	x.draw ( lamb );
	y.draw ( lamb );
	z.draw ( lamb );
}

void cartesian_c::serialize ( fstream &file, bool bsave )
{
	if( bsave ){
		file << "CARTESIAN" << endl;
	}
	motion.serialize ( file, bsave );
	if(!bsave ){
		vA.serialize ( file, bsave );
		vvx.serialize ( file, bsave );
		vvy.serialize ( file, bsave );
		vvz.serialize ( file, bsave );
		string s;		
		file >> s;
		tx.serialize ( file, bsave );
		file >> s;
		ty.serialize ( file, bsave );
		file >> s;
		tz.serialize ( file, bsave );
		file >> dimension;
	}
	else{
		vA.serialize ( file, bsave );
		vvx.serialize ( file, bsave );
		vvy.serialize ( file, bsave );
		vvz.serialize ( file, bsave );
		tx.serialize ( file, bsave );
		ty.serialize ( file, bsave );
		tz.serialize ( file, bsave );
		file << dimension << endl;
	}
}

namespace ex_cartesian{

}// end namespace ex_cartesian

void ex_entry_cartesian ( int a, string s, void* vp )
{
	static cartesian_c cartesian;
}