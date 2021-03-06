#ifndef REGEXP_H
#define REGEXP_H

class reg_exp_c 
{
	public:
	reg_exp_c ();
	reg_exp_c ( const reg_exp_c& rx );
	bool find_section ( string& s, size_t& position, string& found );
	bool find_section ( 
		string& s, size_t& position, 
		bool is_sb_raw, bool is_sb_excluded, string &search_begin,
		bool is_se_raw, bool is_se_excluded, string &search_end,
		string& find 
	);
	bool find ( string& s, size_t& position, bool is_raw, string& found );
	bool find ( 
		string&s, size_t& position, 
		bool is_raw, string& search, 
		string& find 
	);
	void dump (stringstream& ss );
	void command (stringstream& ss );
	string search;
	string search_end;
	string replace;
	bool is_search_raw;
	bool is_search_excluded;
	bool is_search_end_raw;
	bool is_search_end_excluded;
	int mode;
	int look_before;
	int look_after;
}; 

#endif
