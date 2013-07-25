#ifndef BOOKMARKS_H
#define BOOKMARKS_H

using namespace std;

class bookmarks_c
{
	public:
	bookmarks_c();
	bool look_in_cash(string&,string&);
	void add_to_cash(string&,stringstream&);
};

#endif