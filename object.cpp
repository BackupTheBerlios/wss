#include <stdio.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"

#include "matrix.h"
#include "retine.h"
#include "position.h"

#include "object.h"
#include "line.h"
#include "keyb.h"
#include "fox/edit.h"
#include "spline.h"
#include "carte.h"

using namespace std;

object_c::object_c ()
{
	state = 0;
	parent = 0;
}

object_c::~object_c ()
{
}

void object_c::shift ( matrix_c<FT>&)
{
}

object_c* object_c::duplicate ()
{
	return 0;
}

void object_c::object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA )
{
	lbase = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
	lvA = { 0, 0, 0 };
}

void object_c:: coocked_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA, int& flag )
{
	lbase = motion.object_base ();
	lvA = motion.vA;
}

matrix_c<FT> object_c::to_object ( object_c& object, matrix_c<FT>& pt )
{
	matrix_c<FT> point, base1, base2, vA1, vA2;
	point = pt;
	base1 = motion.object_base ();
	vA1 = motion.object_vector ( 1 );
	base2 = object.motion.object_base ();
	vA2 = object.motion.object_vector ( 1 );
	point = base1*point;
	point = point+vA1;
	point = point-vA2;
	point=~base2*point;
	return point;	
}

matrix_c<FT> object_c::from_object ( object_c& object, matrix_c<FT>& pt )
{
	matrix_c<FT> point, base1, base2, vA1, vA2;
	point = pt;
	base1 = motion.object_base ();
	vA1 = motion.object_vector ( 1 );
	base2 = object.motion.object_base ();
	vA2 = object.motion.object_vector ( 1 );
	point = base2*point;
	point = point+vA2;
	point = point-vA1;
	point=~base1*point;
	return point;	
}

bool object_c::find ( lamb_c &lamb, matrix_c<FT> pt )
{
	return false;
}

object_c* object_c::dyncreate ( unsigned long type )
{
	if ( type == line_c::type ){
		return line_c::create ();
	}
	else if ( type == editor_c::type ){
		 return editor_c::create ();
	}
	else if ( type == spline_c::type ){
		return spline_c::create ();
	}
	else if ( type == cartesian_c::type ){
		return cartesian_c::create ();
	}
	switch ( type ){
	}
	return 0;
}

void object_c::serialize ( fstream &pfile, bool bsave )
{
}
