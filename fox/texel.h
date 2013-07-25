#ifndef TEXEL_H
#define TEXEL_H

class texel_c
{
	public:
	static texel_c* construct ( unsigned int ui, std::map<unsigned int, texel_c*>&);
	virtual unsigned int get_char ();
	virtual Glyph& get_glyph (){};
	texel_c ();
	~texel_c ();
};


class texel_caret_c:public texel_c
{
	public:
	texel_caret_c ();
	virtual unsigned int get_char ();
	unsigned int character;
	virtual Glyph& get_glyph (){return glyph;};
	Glyph glyph;
	void inc_column ();
	void dec_column ();
	void inc_line ();
	void dec_line ();
	void to_row ( size_t row );
	void offset_row ( int delta );
	void insert ( texel_c* tex );
	void erase ();
	int get_leading_tabs ();
	size_t next_word_end ( size_t pos );
	size_t next_word ( size_t pos );
	size_t previous_word ( size_t pos );
	size_t move_to ( size_t index );
	size_t offset ( int delta );
	size_t size ();
	size_t get_position ();
	size_t get_position ( std::vector<texel_c*>::iterator );
	std::vector<texel_c*>::iterator get_iterator ();
	std::vector<texel_c*>::iterator get_iterator_of_line ( int number );
	size_t get_position_row_column_size ( size_t&, size_t&, size_t&);
	size_t get_last_row ();
	size_t get_last_line ();
	size_t get_last_position ();
	
	size_t get_row ( int, size_t* pos, size_t* size );	
	size_t get_line_position ();
	std::string get_string ( int number );
	size_t get_line ( int number, size_t* pos, size_t* size );
	
	editor_c* editor;	
	std::vector<texel_c*>* texels;
	size_t column_memory;	

	size_t column;
	bool is_silent;	
	int row_flag; // -1: inactive, 0: caret, 1: row
	size_t row_size;
	size_t row_position;
	size_t row;
	
	void set_column_memory ();
};

class texel_collapse_c: public texel_c
{
	public:
	texel_collapse_c ();
	virtual unsigned int get_char ();
	int character;
};


class texel_tab_c: public texel_c
{
	public:
	texel_tab_c ();
	virtual unsigned int get_char ();
	int character;
};

class texel_newline_c: public texel_c
{
	public:
	texel_newline_c ();
	virtual unsigned int get_char ();
	unsigned int character;
};

class texel_color_c: public texel_c
{
public:
	texel_color_c ();
	unsigned int character;	
	virtual unsigned int get_char ();
	int count;	
	uint32_t color;
};

class texel_char_c: public texel_c
{
	public:
	texel_char_c ( const texel_char_c&);
	texel_char_c ();
	texel_char_c ( unsigned int );
	virtual Glyph& get_glyph (){return glyph;};
	virtual unsigned int get_char ();
	unsigned int character;
	Glyph glyph;	
};

#endif \\TEXEL_H
