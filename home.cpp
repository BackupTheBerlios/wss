#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <complex>
#include <cmath>
#include <regex>

#include "symbol/keysym.h"


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
#include "line.h"
#include "image.h"
#include "land.h"
#include "fox/font.h"
#include "texel.h"
#include "keyb.h"
#include "edit.h"

#include "home.h"

extern land_c land;

int parrot = 0;

window_manager_c::window_manager_c ()
{
}

void window_manager_c::transform ( int sx, int sy,  rectangle_c& r, int gravity , int revert  = NONE )
{
	rectangle_c s;
	if ( gravity == SOUTH_WEST ) {
		s = r;
	}
	else if ( gravity == EAST_SOUTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, WEST_NORTH, false );
		}
		else {
			s.top = sx - r.left;
			s.bottom = sx - r.right;
			s.left = r.bottom;
			s.right = r.top;		
		}
	}
	else if ( gravity == WEST_NORTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, EAST_SOUTH, false );
		}
		else {
			s.bottom = r.left;
			s.top = r.right;
			s.right = sy - r.bottom;
			s.left = sy - r.top;
		}
	}
	r = s;
	return;
}

void window_manager_c::manage ( lamb_c& lamb, rectangle_c& rectangle , int gravity )
{
	rectangle_c r = rectangle;
	int sx = lamb.eyes.width;
	int sy = lamb.eyes.height;
	rectangle_c rr ;
	if ( get_free_place (r, gravity, lamb.eyes.width, lamb.eyes.height )) {
		rr.left = r.left - sx/2;
		rr.right = r.right - sx/2;
		rr.bottom = r.bottom - sy/2;
		rr.top = r.top - sy/2;
		rectangles.push_back ( r );
	}
	rectangle = rr;
}

pair < int, int> mysort ( list < pair < int, int > >& sl , int max )
{
	sl.sort ( [] ( pair < int, int > p1, pair < int, int > p2 ) {
		if ( p1.first < p2.first ) return true; else return false; 
	} );
	int a = 0;
	for ( auto x : sl ) {
		if ( a < x.first ) {
			return { a, x.first };
		}
		else {
			a = x.second;
		}
	}
	if ( a < max ) {
		return { a, max };
	}
	return { 0, 0 };
}


bool window_manager_c::get_free_place ( rectangle_c& s, int gravity , int sx , int sy ){
	list< pair < int, int> > sl;
	pair < int, int > p;
	list < rectangle_c > nrs;
	rectangle_c ns = s;
	transform ( sx, sy, ns, gravity, false ); 
	ns.right = ns.right - ns.left;
	ns.left =  0;
	ns.top = ns.top - ns.bottom;
	ns.bottom = 0; 
	for ( auto x : rectangles ) {
		transform ( sx, sy, x, gravity, false );
		nrs.push_back ( x );
	}		
	rectangle_c rsxy = { 0, sx, 0, sy };
	transform ( sx, sy, rsxy, gravity, false );
	for ( auto x : nrs ){
		if ( x.bottom == ns.bottom ) {
			sl.push_back ( { x.left, x.right } );
		}
	}
	p = mysort ( sl , rsxy.right ); 
	if ( ns.right - ns.left == 0 ) {
	}
	else {
		ns.left = p.first ;
		ns.right = min ( p.second, ns.left + ns.right );
		ns.bottom = 0;
		ns.top = ns.top;
	}
	transform ( rsxy.right, rsxy.top, ns, gravity, true );
	s = ns;
	return true;
}

void window_manager_c::set ( lamb_c& lamb, editor_c& o, rectangle_c r, int gravity )
{
		manage ( lamb, r , gravity );
		o.frame_width =  r.width ()/20;
		o.frame_height = r.height ()/20 - 1;
		lamb.local ( o, { r.left, r.top, 0}, {1, 0, 0}, {0, 1, 0});
} 

home_c::home_c ( lamb_c& lamb, string tapestry )
{
	list<object_c*> l;
	home ( tapestry, lamb, l );
}

void home_c::home ( string tapestry, lamb_c& lamb, list<object_c*> l )
{
	editor_c* editor = 0;

	list < object_c* >::iterator it = land.llist.begin ();
	for (; it != land.llist.end (); it++ ){
		object_c* po = *it;
		if ( po->get_type () == EDITOR && editor == 0 ){
			editor = ( editor_c* ) *it;
			editor->parent = &lamb;
		}
	}
	if ( editor == 0 ){
		editor = new editor_c;
		land.llist.push_back ( editor );
	}

	window_manager_c& wm = lamb.window_manager;
	wm.rectangles.clear ();

	if ( tapestry == "blue" || tapestry == "" ){
		wm.set ( lamb, lamb.mouth, { 40*20, 3*20}, EAST_SOUTH );		
		wm.set ( lamb, *editor, { 40*20, 48*20 }, EAST_SOUTH );
		wm.set ( lamb, lamb.ears, { 30*20, 24*20 }, SOUTH_WEST );
		wm.set ( lamb, lamb.status, { 32*20, 24*20 }, SOUTH_WEST );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
		lamb.sensor = editor;
	}
	else if ( tapestry == "red" ){
		wm.set ( lamb, lamb.mouth, { 40*20, 3*20}, EAST_SOUTH );		
		wm.set ( lamb, *editor, { 40*20, 48*20 }, EAST_SOUTH );
		lamb.sensor = editor;
	}
	else {
		wm.set ( lamb, lamb.mouth, { 40*20, 3*20}, EAST_SOUTH );		
		wm.set ( lamb, *editor, { 40*20, 48*20 }, EAST_SOUTH );
		wm.set ( lamb, lamb.status, { 32*20, 24*20 }, SOUTH_WEST );
		lamb.sensor = editor;
	}

}

namespace exhome{
void entry ( int a, string s, void* pv )
{
	window_manager_c wm;
	rectangle_c u = { 0,  300,  0,  50 };
	rectangle_c r; 
	parrot = 1;
	r = u;
	wm.get_free_place ( r, EAST_SOUTH, 800, 400 );
	echo << ".." << r.left << ":" << r.right << ":" << r.bottom << ":" << r.top <<endl;
	parrot = 0;
	r = u;
	wm.get_free_place ( r, EAST_SOUTH, 800, 400 );
	echo << "..." << r.left << ":" << r.right << ":" << r.bottom << ":" << r.top <<endl;


}

} //end namespace exhome

void ex_entry_home (int a, string s, void* pv )
{
	exhome::entry ( a, s, pv );	
}





