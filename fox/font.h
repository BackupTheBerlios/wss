#ifndef FONT_H
#define FONT_H

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class pixel_c;
class editor_c;
class texel_c;
class texel_caret_c;
class xline;

struct glyph_environment_c
{
	glyph_environment_c ()
	{editor = 0;};
	matrix_c<FT> vA;
	matrix_c<FT> vx;
	matrix_c<FT> vy;
	int no;
	int caret;
	texel_caret_c* tc;
	int selectionA;
	int selectionB;
	int x;
	int y;
	int penX;
	int penY;
	vector<texel_c*>* texels;
	editor_c* editor;
	bool is_checked;
	bool is_selected;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	uint32_t color;
};

class Glyph
{
	public:
	void createCaret ();
	Glyph ();
	Glyph ( FT_Face, unsigned char );
	Glyph ( FT_Face, FT_UInt );
	~Glyph ();
	std::vector<pixel_c> color_pixels;
	FT_BitmapGlyph bitmapGlyph;
	unsigned int get_char ();
	bool isEmpty;
};

class font_c 
{
	public :
	xline* line;
	font_c ();
	virtual ~font_c ();
	static int init ();
	static int done ();
	static unsigned long type;
	static font_c* create ();

	virtual void engrave ( lamb_c& lamb, glyph_environment_c& glyph_env );
	virtual unsigned long get_type (){return type;};
	matrix_c<FT> get_text_size ( std::string s );
	bool if_word_to_its_end ( std::vector<texel_c*>&, size_t&);
	int get_text_size ( std::vector<texel_c*>&, size_t, size_t );
	void set_face ( float f );
	std::map<FT_UInt, Glyph> glyphMap;
	Glyph caretGlyph;
	Glyph tabGlyph;
	int my_draw ( lamb_c& lamb, Glyph& glyph, glyph_environment_c& glyph_env );
	int getY ( std::string c );
	int cursorPosition;
	Glyph& get_glyph ( FT_UInt );
	int tab_width;
	int cell;
};

#endif
