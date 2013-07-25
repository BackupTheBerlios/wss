#ifndef ECHO_H
#define ECHO_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <cmath>

#include "library/wdslib.h"
#include "symbol/keysymdef.h"
#include "wcp.h"
#include "retine.h"
#include "message.h"
#include "matrix.h"
#include "position.h"
#include "object.h"

#include "keyb.h"
#include "retine.h"
#include "eyes.h"
#include "fox/edit.h"
#include "fox/texel.h"

using namespace std;

class echo_c 
{
	public:
	echo_c ();
	~echo_c ();
	string buffer;

	template <typename T>
	echo_c& operator<<(const T& x );
	
	echo_c& operator<<(int& i );
	
	// function that takes a custom stream, and returns it
	typedef echo_c& (*echo_manipulator )(echo_c&);

	// take in a function with the custom signature
	echo_c& operator<<(echo_manipulator manip );

	// define the custom endl for this stream.
	// note how it matches the `echo_manipulator`
	// function signature
	static echo_c& endl ( echo_c& stream );
	
	static echo_c& hex ( echo_c& stream );
	
	// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine )(CoutType&);

	// define an operator<< to take in std::endl
	echo_c& operator<<(StandardEndLine manip );
	virtual void clear ();
	void flush ();
	bool roll;
};

template <typename T>
echo_c& echo_c::operator<<(const T& x )
{
	stringstream ss;
	ss << x;
	buffer += ss.str ();
	return *this;		
}

#endif


