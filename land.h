#ifndef LAND_H
#define LAND_H

using namespace std;

class lamb_c;
class line_c;
class land_c 
{
	public:
	land_c ();
	virtual ~land_c ();
	void impress ( lamb_c& lamb );
	void invalide_all_objects ( int level );
	list<object_c*> llist;
	list<object_c*> lclip;
	bool show_base;
	float raster;
	matrix_c<FT> rasterize ( matrix_c<FT> pt );
	void remove_selection ();
	void remove_object ( object_c*);
	void move_cursor ( lamb_c& lamb );	
	lamb_c* get_lamb ();
};
#endif
