#ifndef HOME_H
#define HOME_H

using namespace std;

struct rectangle_c
{
	rectangle_c (const rectangle_c& rect ) {
		left = rect.left, right = rect.right, bottom = rect.bottom, top = rect.top; }
	rectangle_c () : left ( 0 ), right ( 0 ), bottom ( 0 ), top ( 0 ) {}
	rectangle_c ( int l, int r, int b, int t ) : left ( l ), right ( r ), bottom ( b ), top ( t ) {}
	rectangle_c ( int r, int t ) : left ( 0 ), right ( r ), bottom ( 0 ), top ( t ) {}
	int width () { return right - left; }
	int height () { return top - bottom; }
	int top;
	int bottom;
	int left;
	int right;
};

class window_manager_c
{
public:
	window_manager_c ();
	int tree;
	void manage (lamb_c&, rectangle_c& t, int direction );
	list < rectangle_c> rectangles;
	bool get_free_place (rectangle_c&, int gravity, int x, int y );
	void  transform ( int sx, int sy, rectangle_c& r, int gravity, int revert );
	void set ( lamb_c&, editor_c&, rectangle_c r, int gravity );
};

class home_c
{
public:
	home_c ();
	home_c ( lamb_c&, string );
	void home ( string color, lamb_c&, list<object_c*>);
};

#endif