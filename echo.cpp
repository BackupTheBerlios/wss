#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <cmath>

#include "library/wdslib.h"

#include "symbol/keysym.h"
#include "wcp.h"
#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"
#include "object.h"
#include "keyb.h"
#include "eyes.h"
#include "mouse.h"
#include "fox/edit.h"
#include "land.h"
#include "retine.h"
#include "mouse.h"
#include "eyes.h"
#include "home.h"

#include "lamb.h"

#include "fox/edit.h"
#include "comtab.h"

#include "echo.h"

extern lamb_c lamb;

void echo_c::flush (){
//	set_texels ( get_texels () + buffer );
	buffer = "";
}

echo_c& echo_c::operator << (StandardEndLine manip )
{
	buffer += "\n";
	return *this;
}

echo_c& echo_c::operator << (int& i )
{
	stringstream ss;
	ss << i;
	buffer += ss.str ();
	return *this;
}

echo_c& echo_c::endl ( echo_c& stream )
{
	// do other stuff with the stream
	// cout, for example, will flush the stream
	return stream;
}

echo_c& echo_c::operator << (echo_manipulator manip )
{
	// call the function, and return it's value
	return manip (*this );
}

void echo_c::clear ()
{
	buffer = "";
}

echo_c::echo_c ():roll ( true )
{
}

echo_c::~echo_c ()
{
}
