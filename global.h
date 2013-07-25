#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include "echo.h"

#define IF(b, s ) if((b ))echo<<(#s )<<endl;
#define IFE(b, s1, s2 ) if((b ))echo<<(#s1 )<<endl;else echo<<(#s2 )<<endl;
#define O(s ) echo<<#s<<endl;
#define I(v ) echo<<v<< endl;
#define OI(s, v ) echo<<(#s )<<":"<<(v )<<endl;
#define O2(b, x ) if((b ))cout<<(#x )<<endl;
#define O1(x ) cout<<#x<<endl;

#define OO(c, s, v ) static int i##__LINE__ = (c );\
 if( i##__LINE__  > 0 )\
{ echo << (#s ) << ":"<< v << endl; i##__LINE__--;};

#define Oo(c, s, v ) static int i##__LINE__ = (c );\
if( i##__LINE__  > 0 )\
{ cout << (#s ) << ":" <<  v << endl; i##__LINE__--;};

extern echo_c echo;

#endif