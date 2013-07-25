#ifndef RETINE_H
#define RETINE_H

using namespace std;

#include <map>

class zlist_c
{
	public:
	list < pair < float, uint32_t >  >  zpix;
	uint32_t *frame_xy;
};

class surface_list_c;
class memory_map_c;

class surface_c
{
	public:
	surface_c ( surface_list_c* psl );
	void initialize ( int x, int y, uint32_t color );
	vector < vector < zlist_c > > v;
	uint32_t *frame;
	surface_list_c* parent;
	
	int width;
	int height;
	
	float zoom;
	matrix_c < FT> mx;
	matrix_c < FT> pointer;
	matrix_c < FT> local_position;
	motion_3D_c < FT> motion;
	~surface_c ();
};

class surface_list_c
{
	public:
	list < memory_map_c* > memories_list;
	void register_memory ( memory_map_c* ); 
	list < surface_c > l;
	~surface_list_c ();
};

class memory_c
{
	public:
	memory_c ();
	vector < pair < zlist_c*, list < pair < float, uint32_t > >::iterator> > l;
	int valide;
	void disapear ();
};

class memory_map_c
{
	public:
	memory_map_c ();
	~memory_map_c ();
	memory_c* active_mem;
	surface_c* active_surface;
	memory_c* get_mem ( surface_c& );
//	memory_c* get_mem ( surface_list_c&, surface_c& );
	void erase_mem ( surface_c& );
	void remove ( surface_c* );
	map < surface_c*, memory_c > m;
	list < surface_list_c* > surfaces_list;
};

class retine_c 
{
	public:
	retine_c ();
	surface_list_c surfaces;
	memory_map_c mems;
};

#endif
