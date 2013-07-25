#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "symbol/keysym.h"

#include "library/wdslib.h"
#include "wcp.h"
#include "matrix.h"
#include "position.h"

#include "retine.h"
#include "message.h"

#include "global.h"
#include "object.h"
#include "land.h"
#include "keyb.h"
#include "retine.h"
#include "mouse.h"
#include "eyes.h"
#include "edit.h"
#include "home.h"
#include "lamb.h"
#include "font.h"
#include "texel.h"
#include "edit.h"

extern int flag1;

extern land_c land;
int iniok = 0;

using namespace std;

FT_Library library = 0;
FT_Face face;

unsigned long font_c::type = 2;

float z;

Glyph::Glyph ()
{
	isEmpty = true;
	bitmapGlyph = 0;
}

unsigned int Glyph::get_char ()
{
//	return character;
}

void  Glyph::createCaret ()
{
	bitmapGlyph = new FT_BitmapGlyphRec;
	int r = bitmapGlyph->bitmap.rows = 4;
	int w = bitmapGlyph->bitmap.width = 14;
	bitmapGlyph->left = -5;
	bitmapGlyph->top = -1;
	bitmapGlyph->bitmap.buffer = new unsigned char [r*w];
	memset ( bitmapGlyph->bitmap.buffer, 0xff, r*w );
}

Glyph::Glyph ( FT_Face face, unsigned char c ):
	isEmpty ( true ){
	if ( c=='\t'){
		bitmapGlyph = new FT_BitmapGlyphRec;
		bitmapGlyph->bitmap.buffer = 0;
		bitmapGlyph->bitmap.rows = 0;
		bitmapGlyph->bitmap.width = 0;
		bitmapGlyph->root.advance.x = 50;
		bitmapGlyph->root.advance.y = 0;		
	}
}

Glyph::Glyph ( FT_Face face, FT_UInt glyph_index ):
	isEmpty ( true )
{
	int error = FT_Load_Glyph ( face, glyph_index, FT_LOAD_DEFAULT );
	if ( error ){
		cout << "free type: err load glyph" << std::endl;
		return;
	}
	FT_GlyphSlot slot = face->glyph;
	error = FT_Render_Glyph ( slot, FT_RENDER_MODE_NORMAL );
	if ( error ){
		cout << "free type: err render" << std::endl;
		return;
	}
	FT_Glyph ftglyph;
	FT_Get_Glyph ( slot,&ftglyph );
	FT_Glyph_To_Bitmap (&ftglyph, FT_RENDER_MODE_NORMAL, 0, 1 );
	bitmapGlyph=(FT_BitmapGlyph ) ftglyph;
	bitmapGlyph->root.advance.x = slot->advance.x;
	bitmapGlyph->root.advance.y = slot->advance.y;
}

Glyph::~Glyph ()
{
}



void font_c::set_face ( float f )
{
	int error;	
	string fontpath = get_lambs_path ();
	fontpath+="/";
//	fontpath+="../fonts/luximr.ttf";
//	fontpath+="../fonts/VeraMono.ttf";
	fontpath+="./fonts/Vera.ttf";
//	fontpath = "/usr/share/fonts/truetype/LiberationSans-Regular.ttf";
//	fontpath+="../fonts/DejaVuSans.ttf";

	error = FT_New_Face ( library, fontpath.c_str (), 0, &face );
	if ( error ) {
//		echo << "error new face: " << error << endl;
//		echo << fontpath << endl;
		return;
	}
	else{
//		echo << "ok new face: " << endl;
	}
	//float zz = lamb.eyes.dpm*lamb.eyes.zoom;
//	error = FT_Set_Char_Size ( face, 0, 15*64, 0, 92 );
	error = FT_Set_Char_Size ( face, 0, 13*64*f, 0, 92 );
//	error = FT_Set_Char_Size ( face, 0, 12*64*f, 0, 92 );
//	error = FT_Set_Pixel_Sizes ( face, 0, 10 );
	if ( error ){
	    cout << "free type: err size" << endl;
	}
}

font_c* font_c::create ()
{
    return new font_c;
}

int font_c::getY ( string c )
{
	return 20;
}

Glyph& font_c::get_glyph ( FT_UInt glyph_index )
{
	Glyph glyph;
	return glyph;
}

matrix_c<FT> font_c::get_text_size ( string str )
{
	matrix_c<FT> size ( 3, 1 );
	map<FT_UInt, Glyph>::iterator it;
	int num_chars = str.size ();
	for (int n = 0;n <num_chars;n++){
		if ( str[n]=='\t'){
			size[1][1]+= 40*z;
			continue;
		}
		FT_UInt glyph_index;
		glyph_index = FT_Get_Char_Index ( face, str[n]);
		it = glyphMap.find ( glyph_index );
		if ( it == glyphMap.end ()){
			glyphMap[glyph_index]= Glyph ( face, glyph_index );
			it = glyphMap.find ( glyph_index );
		}			
		Glyph& glyph = it->second;
		
		FT_BitmapGlyph bitmapGlyph;
		bitmapGlyph = glyph.bitmapGlyph;
		size[1][1]+= bitmapGlyph->root.advance.x >> 6;		
	}
	return size;
}

int font_c::my_draw ( lamb_c& lamb, Glyph& glyph, glyph_environment_c& glyph_env )
{
	static int count = 0;	
	int ix = glyph.bitmapGlyph->left;
	int iy = glyph.bitmapGlyph->top;
	if ( z != 1 ){
		ix /= z;
		iy /= z;
	}
	int x = glyph_env.x + glyph_env.penX + ix;
	int y = glyph_env.y - glyph_env.penY + iy;
	matrix_c<FT> v ( 3, 1 );
	if ( glyph.isEmpty ){
		FT_Bitmap& bitmap = glyph.bitmapGlyph->bitmap;
		int i, j, p, q;
		int x_max = x+glyph.bitmapGlyph->bitmap.width;
		int y_min = y-glyph.bitmapGlyph->bitmap.rows;

		for ( j = y, q = 0; j > y_min; j--, q++){
			for ( i = x, p = 0; i < x_max; i++, p++){
				unsigned char col = bitmap.buffer[q*bitmap.width+p];
				//if ( col>127 ){
				if ( col > 1 ){
					v = p*glyph_env.vx + -q*glyph_env.vy ;
					unsigned char icol =~ col;
					uint32_t rgb;
					rgb = icol;
					rgb = rgb << 8;
					rgb |= icol;
					rgb = rgb << 8;
					rgb |= icol;
					glyph.color_pixels.push_back ( pixel_c ((int )v[1],(int )v[2], rgb ));
				}
			}
		}
		glyph.isEmpty = false;
	}

	if ( false ){
		v[1] = x + glyph_env.vA[2];
		v[2] = y + glyph_env.vA[3];

	/*
		v[1] = x + vA[2];
		v[2] = y + vA[3];
	*/
	}
	else{
		v = x*glyph_env.vx + y*glyph_env.vy + glyph_env.vA;
		v = lamb.eyes.zoom*v;
	}
	
	int colo = 0;
	if ( glyph_env.is_checked ){
		colo = 0xff0000;
	}
	else if ( glyph_env.is_selected ){
		colo = 0xff;
	}	
	
	auto itc = glyph.color_pixels.begin ();
	int ccx = v[1];
	int ccy = v[2];
	for (; itc != glyph.color_pixels.end (); itc++){
		pixel_c cpix = *itc;
		cpix.color |= colo;

		lamb.eyes.set_pixel ( glyph_env.editor, cpix.x + ccx, cpix.y + ccy , 0, cpix.color );
	}
}

int font_c::done () 
{
    FT_Done_FreeType ( library );
}

int font_c::init ()
{
	int error = FT_Init_FreeType (&library );
	if ( error ) {
		cout << "free type :err init " << endl;
		return 1;
	}
	else {
		cout << "initialize freetype font library " << endl;
	}
	iniok = 1;
	return 0;
}

bool font_c::if_word_to_its_end ( vector<texel_c*>& texels, size_t& inz ){
	assert ( 0 <= inz && inz <= texels.size ());
	bool word = false;
	bool cf = false;
	for (; inz < texels.size (); inz++ ){
		texel_c* tex = texels.at ( inz );
		unsigned long ch = tex->get_char ();
		if ( ch < 0x80 ){	
			if ( !isalnum ( ch ) && ch != '\a'  ){
				if ( word ){
					if ( cf ) {
						-- inz;
					}
					--inz;
					return true;
				}
				return false;
			}
			if ( ch != '\a' ) {
				word = true;
				cf = false;
			}
			else {
				cf = true;
			}
		}	
	}
	--inz;
	return true;
}

int font_c::get_text_size ( vector<texel_c*>& texels, size_t inz, size_t inz_e ){
	int x = 0;
	FT_UInt glyph_index;
	for (; inz < inz_e; inz++){
		texel_c& tex = *texels.at ( inz );
		unsigned long chr = tex.get_char ();
		Glyph& glyph = tex.get_glyph ();
		char ch = chr;
		if ( glyph.bitmapGlyph == 0 ){
			glyph_index = FT_Get_Char_Index ( face, chr );
			tex.get_glyph () = Glyph ( face, glyph_index );
		}
		if ( ch == '\t'){
//			glyph_env.penX += 40 - (glyph_env.penX%40 );
			x += 40*z;
		}
		else if (ch != '\a' && ch != 1 ){
			x += glyph.bitmapGlyph->root.advance.x >> 6;
		}
		else if ( ch == '\a' ) {
			if ( inz + 1 == inz_e  &&  inz + 1 < texels.size () ) {
				texel_c& tex = *texels.at ( inz + 1 );
				Glyph& glyph = tex.get_glyph ();
				ch = tex.get_char ();
				if ( glyph.bitmapGlyph == 0 ){
					glyph_index = FT_Get_Char_Index ( face, ch );
					tex.get_glyph () = Glyph ( face, glyph_index );
				}
				if ( ch == '\t'){
		//			glyph_env.penX += 40 - (glyph_env.penX%40 );
					x += 40*z;
				}
				else if (ch != '\a' && ch != 1 ){
					x += glyph.bitmapGlyph->root.advance.x >> 6;
				}
			}
		}
	}
	return x;
}



void font_c::engrave ( lamb_c& lamb, glyph_environment_c& glyph_env )
{
	glyph_environment_c& ge = glyph_env;
	z = lamb.eyes.zoom;
	size_t inz_b = 0, inz_e = 0;
	vector<texel_c*>& texels = *ge.texels;
	bool is_selected = ge.is_selected;
	size_t c_row = 1;
	size_t c_column = 0;
	size_t c_caret = 0;
	size_t c_row_position = 1;
	unsigned int chr = 0;
	ge.color = 0x00;
	ge.is_checked = false;	
	
	int line_width = (ge.editor->frame_width )*cell - 2;
	
	for (int n  = 0; n < texels.size (); n++){
		if ( ge.penY >= ge.max_y ){
			if ( ge.tc->is_silent == false ){
				break;
			}
		}
		texel_c& tex = *texels[n];
		chr = tex.get_char ();
		
		Glyph& glyph = tex.get_glyph ();
		if ( glyph.bitmapGlyph == 0 ){
			FT_UInt glyph_index = FT_Get_Char_Index ( face, chr );
			tex.get_glyph () = Glyph ( face, glyph_index );
		}
		
		bool w = true;
		int ws = 0;

		if ( inz_b == n ){
			if_word_to_its_end ( texels, inz_e );
			inz_e++;
			ws = get_text_size ( texels,  inz_b, inz_e );
			if ( ws + ge.penX > line_width*z ){
				w = false;
				n--;
			}
			inz_b = inz_e;
		}	
		FT_BitmapGlyph bitmapGlyph;
		bitmapGlyph = glyph.bitmapGlyph;
		if ( w ){
			if ( ge.no == ge.caret ){
				if ( caretGlyph.bitmapGlyph == 0 ){
					caretGlyph.createCaret ();
				}		
				int i = (bitmapGlyph->root.advance.x >> 6 )/2;
				ge.penX += i;	
				if ( ge.penY >= ge.min_y  
					&& ( glyph_env.editor -> simple_text == false) 
					 ){															my_draw ( lamb, caretGlyph, ge );
				}
				ge.penX -= i;
			}
			if ( ge.no >= ge.selectionA && 
			ge.no < ge.selectionB ){				
				ge.is_selected = true;
			}
			else{
				ge.is_selected = false;
			}
			if ( chr != '\n' && chr != '\t' && chr != '\a'){
				if ( ge.penY >= ge.min_y ){															if ( ge.tc->is_silent == false ){
						my_draw ( lamb, glyph, ge );
					}			
				}
			}
			else if ( chr == '\t' ){
				ge.penX += 40 - (ge.penX )%40;
			}
			if ( chr != '\a'  ){			
				if ( chr != '\t'){
					ge.penX += (bitmapGlyph->root.advance.x >> 6 )/z;
					ge.penY += (bitmapGlyph->root.advance.y >> 6 )/z;
				}
				c_column++;
			}
			else{
				c_caret = 1;
			}
			ge.no++;
		}
		if ( (  !w && c_column != 0 )
			|| chr == '\n' || ge.penX >= line_width || n == texels.size () - 1 ){
			if ( ge.tc->row_flag != -1 ){
				if ( ge.tc->row_flag == 0 && c_caret == 1 ){
					ge.tc->row = c_row;
					ge.tc->row_size = c_column;
					ge.tc->row_position = c_row_position;
					}
				else if ( ge.tc->row_flag == 1 && c_row == ge.tc->row ){
					ge.tc->row_size = c_column;
					ge.tc->row_position = c_row_position;
				}
			}
			c_caret = 0;
			c_row++;
			c_row_position += c_column;
			c_column = 0;
			ge.penX = 0;
			ge.penY += cell;
		}
	}
	return;
}

font_c::font_c ()
{
	set_face ( 1 );
}

font_c::~font_c () 
{
	//FT_Done_Face ( face );
}

