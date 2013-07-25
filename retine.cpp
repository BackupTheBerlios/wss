#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iostream>

#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"

memory_c::memory_c ()
{
	valide = 0;
}

void memory_c::disapear ()
{
	for ( int c = 0; c < l.size (); c++ ){
		zlist_c* zl = l [ c ].first;
		list < pair < float, uint32_t > >::iterator zi = l [ c ].second;
		zl -> zpix.erase ( zi );
		*( zl -> frame_xy ) = zl -> zpix.begin () -> second;		
	}
	l.resize ( 0 );
}

memory_map_c::memory_map_c ()
{
	active_mem = 0;
	active_surface = 0;
}

memory_map_c::~memory_map_c ()
{
	for ( auto memory : m ){
		memory.second.disapear ();
	}
	for ( auto surfaces : surfaces_list ) {
		surfaces -> memories_list.remove ( this );
	}
}

void memory_map_c::erase_mem ( surface_c& surface )
{
	auto it = m.find ( &surface );
	if ( it != m.end () ){
		it -> second.disapear ();		
	}	
}

void memory_map_c::remove ( surface_c* surface )
{
	auto it = m.find ( surface );
	if ( it != m.end () ){
		m.erase ( it );
	}
	active_surface = 0;
	active_mem = 0;
}

memory_c* memory_map_c::get_mem ( surface_c& surface )
{
	if ( active_surface == &surface ){
		return active_mem;
	}
	auto it = m.find ( &surface );
	if ( it != m.end () ){
		active_surface = &surface;
		active_mem = &it -> second;
	}
	else{
		auto res = m.insert ( pair < surface_c*, memory_c > ( &surface, memory_c () ) );
		active_mem = &res.first -> second;
		active_surface = &surface;
		if  ( surface.parent == 0 ){
			cout << "zero parent " << endl;
		}
		else {
			surface.parent -> register_memory ( this );
			auto it = find ( surfaces_list.begin (), surfaces_list.end (), surface.parent );
			if ( it == surfaces_list.end () ){
				surfaces_list.push_back ( surface.parent );
			}
		}
	}
	return active_mem;
}

surface_c::surface_c ( surface_list_c* psl ):
frame ( 0 ),
parent ( psl )
{
}

void surface_c::initialize ( int x, int y, uint32_t color )
{
	assert ( frame == 0 );
	frame = new uint32_t [ x*y ];
	uint32_t* frame_xy = frame;
	v.resize ( y );
	for ( int cy = 0 ; cy < y ; cy++ ){
		v [ cy ].resize ( x );
		for (int cx = 0; cx < x ; cx++ ){
			v [ cy ] [ cx ].zpix.push_front ( {10000, color} );
			v [ cy ] [ cx ].frame_xy = frame_xy;
			*frame_xy = color;
			frame_xy++;
		}
	}
}

surface_c::~surface_c ()
{
	assert ( parent != 0);
	for ( auto memories : parent -> memories_list ){
		memories -> remove ( this );
	}
	delete frame;
}

surface_list_c::~surface_list_c ()
{
	l.clear ();
}

void surface_list_c::register_memory (memory_map_c* mmp)
{
	if ( memories_list.end () == find ( memories_list.begin (), memories_list.end (), mmp )) {
		memories_list.push_back ( mmp );
	}
}

retine_c::retine_c () 
{
}
