#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>


#include "symbol/keysym.h"
#include "wcp.h"

#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"
#include "object.h"

#include "land.h"
#include "matrix.h"
#include "keyb.h"
#include "retine.h"
#include "mouse.h"
#include "eyes.h"
#include "fox/edit.h"
#include "home.h"
#include "lamb.h"

#include "fox/font.h"
#include "line.h"
//#include "paragram.h"
#include "spline.h"
#include "image.h"

#include "global.h"

using namespace std;


extern int parrot1;

extern long flag;

extern line_c linex;
extern line_c liney;
extern line_c linez;


land_c land;
//lamb_c  lamb;
lamb_c* plamb = nullptr;

class cursor_c
{
	public:
	void draw ( lamb_c& lamb );
};

void cursor_c::draw ( lamb_c& lamb )
{
	matrix_c <FT> ptv = lamb.pointer;
	ptv = land.rasterize ( ptv );
	
	int x = ptv[1];
	int y = ptv[2];
	for ( int c=-10; c <= 10 ; c++){
		lamb.eyes.set_pixel ( 0, x+c, y, 0, 0x00, true );
		if ( c == 0 ){
			continue;
		}
		lamb.eyes.set_pixel ( 0, x, y+c, 0, 0x00, true );
	}	
}

cursor_c cursor;

matrix_c<FT> land_c::rasterize ( matrix_c<FT> pt ){
	if ( raster == 0 ) {
		return pt;
	}
	int n;
	float f;
	f = pt[1];
	if ( f<0 ){
		f-= raster/2;
	}
	else {
		f+= raster/2;
	}
	n = f/raster;
	pt[1]= n*raster;
	f = pt[2];
	if ( f<0 ){
		f-= raster/2;
	}
	else{
		f+= raster/2;
	}
	n = f/raster;
	pt[2]= n*raster;
	f = pt[3];
	if ( f<0 ){
		f-= raster/2;
	}
	else{
		f+= raster/2;
	}
	n = f/raster;
	pt[3]= n*raster;
	return pt;
}

lamb_c* land_c::get_lamb ()
{
	for ( auto x : llist ) {
		if ( x -> get_type ()  == LAMB ) {
			return ( lamb_c * ) x;
		}
	}	
	return 0;
}

void land_c::invalide_all_objects ( int level )
{
	for ( auto x : llist ) {
		x->invalide_visual ( level );
	}
}

void land_c::impress ( lamb_c& lamb )
{
	static int count = 0;
	count++;
	lamb.eyes.valide = false;
	
	if ( ! lamb.is_idle () ) {
		return;
	}
	
	lamb.write_status ();
	
	lamb.eyes.retine.mems.erase_mem ( lamb.eyes.retine.surfaces.l.front () );
	
	
	for ( auto x : llist ) {
		if ( ! lamb.is_idle () ) {
			return;
		}
		x->draw ( lamb );	
	}

	
	if ( show_base ) {	
		linex.color = 0xff0000;
		linex.vb.l[0]= 30;
		linex.vx.l[1]= 4;
		liney.color = 0x00ff00;
		liney.vb.l[1]= 30;	
		liney.vx.l[2]= 4;	
		linez.color = 0x0000ff;
		linez.vb.l[2]= 30;	
		linez.vx.l[0]= 4;	
		linex.draw ( lamb );
		liney.draw ( lamb );
		linez.draw ( lamb );
	}
	cursor.draw ( lamb );
	lamb.expose_image ();
	lamb.eyes.valide = true;
}

land_c::land_c ()
{
	raster = 10;
	show_base = false;
}

void land_c::remove_object ( object_c* object_pointer )
{
	auto it = llist.begin ();
	for (; it != llist.end (); it++){
		if (*it == object_pointer ){
			llist.erase ( it );
		}
	}
}

void land_c::remove_selection ()
{
	auto it = llist.begin ();
	object_c* pobj;
	for (; it != llist.end ();){
		pobj = *it;
		if ( pobj == 0 ){
			echo << "object" << endl;
			it++;
		}
		else{
			if ( pobj->state == 1 ){
				echo << "erase" << endl;
				it = llist.erase ( it );
				delete pobj;
			}	
			else{
				it++;
			}
		}
	}
}

land_c::~land_c ()
{
	for ( auto x : llist ){
		delete x;
	}
}

