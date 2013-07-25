#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

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
#include "mouse.h"
#include "eyes.h"
#include "fox/edit.h"
#include "home.h"
#include "lamb.h"
#include "font.h"
#include "texel.h"
#include "edit.h"

extern land_c land;

texel_caret_c::texel_caret_c ()
{
	is_silent = false;
	column_memory = 1;
	character='\a';
	row_flag = 0;
}

unsigned int texel_caret_c::get_char ()
{
	return character;
}

size_t texel_caret_c::next_word ( size_t pos )
{
	auto it = get_iterator ();	
	size_t pos_c = pos;
	bool flag = false;
	for (;it!= texels->end ();it++){
		texel_c* tex=*it;
		unsigned long ch = tex->get_char ();
		if ( ch!='\a' && ch<0x80 ){	
			if  ( !isalnum ( ch )){
				flag = true;
			}
			else if ( flag == true ){
				return pos_c;
			}
		}
		if ( ch!='\a'){
			pos_c++;
		}	
	}
	return pos;
}

size_t texel_caret_c::next_word_end ( size_t pos )
{
	auto it = get_iterator ();
	size_t pos_c = pos;
	bool flag = false;
	for (;it != texels->end ();it++){
		texel_c* tex=*it;
		unsigned long ch = tex->get_char ();
		if ( ch!='\a'){
			if ( ch >=  0x80 || isalnum ( ch )){
				flag = true;
			}
			else if ( flag == true ){
				return pos_c;
			}
			pos_c++;
		}
	}
	return --pos_c;
}

size_t texel_caret_c::previous_word ( size_t pos )
{
	auto it = get_iterator ();
	size_t pos_c = pos;
	bool flag = false;
	for (;it!=--texels->begin ();it--){
		texel_c* tex=*it;
		unsigned long ch = tex->get_char ();
		if ( ch!='\a' && ch<0x80 ){	
			if  ( !isalnum ( ch )){
				flag = true;
			}
			else if ( flag == true ){
				return pos_c;
			}
		}
		if ( ch!='\a'){
			pos_c--;
		}	
	}
	return pos;
}

size_t texel_caret_c::get_position_row_column_size ( size_t& ro, size_t& co, size_t& si )
{
	size_t pos1;
	row = get_row ( 0,&pos1,&si );
	size_t pos2 = get_position ();
	co = pos2-pos1+1;
	return pos2;
}

size_t texel_caret_c::move_to ( size_t position )
{
	size_t pos = get_position ();
	int delta= position-pos;
	offset ( delta );
}

size_t texel_caret_c::offset ( int delta )
{
	vector<texel_c*>::iterator it, it2;
	it2 = it = get_iterator ();
	if ( delta<0 ){
		for (;it!= texels->begin ();it--){
			if ( delta == 0 ){
				break;
			}
			delta++;
		}
	}
	else{
		for (;it!=--texels->end ();it++){
			if ( delta == 0 ){
				break;
			}
			delta--;
		}
	}
	texel_c* texel_c=*it2;
	texels->erase ( it2 );
	texels->insert ( it, texel_c );
}

size_t texel_caret_c::size ()
{
}

void texel_caret_c::to_row ( size_t row )
{
	size_t row_size;
	size_t position;
	get_row ( row, &position, &row_size );
	if ( position == 0 ){
		return ;
	}
	size_t o = std::min ( row_size, column_memory );
	move_to ( position + o - 1 );
}

size_t texel_caret_c::get_row ( int No, size_t* pos, size_t* size )
{
	bool b = is_silent;
	is_silent = true;
	if ( No == 0 ){
		row_flag = 0;
	}
	else{
		row_flag = 1;
		row = No;
	}
	editor -> draw ( *land.get_lamb () );
	if ( pos!= 0 ){
		*pos = row_position;
	}
	if ( size!= 0 ){
		*size = row_size;
	}
	is_silent = b;
	return row;
}

void texel_caret_c::offset_row ( int delta )
{
	size_t row, column, position;
	row = get_row ( 0, 0, 0 );
	size_t d = row + delta;
	to_row ( d );
}

string texel_caret_c::get_string ( int number )
{
	string s;
	auto it = get_iterator_of_line ( number );
	for (;it!= texels->end ();it++){
		uint32_t cc= (*it )->get_char ();
		string ss = utf32_to_utf8 ( cc );
		char ch = cc;
		if ( ch=='\n'){
			break;
		}
		if ( ch=='\a'){
			continue;
		}
		if ( ch=='\t'){
			ch=' ';
		}
		s+= ss;
	}
	return s;
}

size_t texel_caret_c::get_line ( int No, size_t* pos, size_t* si )
{
	auto it = texels->begin ();
	size_t count = 1;
	size_t position = 1;
	size_t  line_position;
	size_t size = 0;
	bool last_line = false;
	if ( No == 0 ){
		for (;it!= texels->end ();it++){
			unsigned long chr=(*it )->get_char ();
			if ( chr=='\a'){
				last_line = true;
				continue;
			}
			else if ( chr=='\n'){
				if ( last_line == true ){
					break;
				}
				size = 0;
				line_position = position;
				count++;
			}
			size++;
			position++;
		}
		if ( last_line == false ){
			line_position = 0;
			count = 0;
			size = 0;
		}
		if ( pos!= 0 ){
			*pos = line_position;
		}
		if ( si!= 0 ){
			*si = size;
		}
		return count;
	}
}

vector<texel_c*>::iterator texel_caret_c::get_iterator_of_line ( int number )
{
	auto it = texels->begin ();
	int count = 1;
	for (;it!= texels->end ();it++){
		if ( count == number ){
			break;
		}
		char ch=(*it )->get_char ();
		if ( ch=='\n'){
			count++;
		}
	}
	return it;
}

vector<texel_c*>::iterator texel_caret_c::get_iterator ()
{
	auto it = texels->begin ();
	for (;it!= texels->end ();it++){
		if ((*it )->get_char () =='\a'){
			return it;
		}
	}
}

size_t texel_caret_c::get_position ( vector<texel_c*>::iterator it2 )
{
	auto it = texels->begin ();
	size_t c = 1;
	for (;it!= texels->end ();it++){
		if ( it2 == it ){
			return c;
		}
		if ((*it )->get_char ()!='\a'){
			c++;
		}
	}
}

size_t texel_caret_c::get_position ()
{
	size_t c = 1;
	for ( auto t : *texels ){
		if ( t->get_char () =='\a'){
			return c;
		}
		c++;
	}
}

void texel_caret_c::insert ( texel_c* tex )
{
	auto it = get_iterator ();
	texels->insert ( it, tex );
}

size_t texel_caret_c::get_line_position ()
{
	size_t position = get_position ();
	auto it = get_iterator ();
	for (;it!= texels->begin ();){
		it--;
		if ((*it )->get_char () =='\n'){
			break;
		}
		position--;
	}
	return position;
}

int texel_caret_c::get_leading_tabs ()
{
	int c = 0;
	size_t pos1 = get_position ();
	size_t pos2 = get_line_position ();
	move_to ( pos2 );
	std::vector<texel_c*>::iterator it;
	it = get_iterator ();
	it++;
	for (;it!= texels->end ();it++){
		if ((*it )->get_char () =='\t'){
			c++;
		}
		else{
			break;
		}
	}
	move_to ( pos1 );
	return c;
}

void texel_caret_c::set_column_memory ()
{
	size_t row, column, position;
	get_row ( 0,&position, 0 );
	column = get_position ()-position+1;
	column_memory = column;
}

void texel_caret_c::erase ()
{
	auto it = get_iterator ();
	if ( it!= texels->begin ()){
		it--;
		texels->erase ( it );
	}
}

size_t texel_caret_c::get_last_position ()
{
	size_t numerator = 1;
	auto it = texels->begin ();
	for (;it!= texels->end ();it++){
		char ch=(*it )->get_char ();
		if ( ch!='\a'){
			numerator++;
		}
	}
	return numerator;
}

size_t texel_caret_c::get_last_line ()
{
	size_t numerator = 1;
	auto it = texels->begin ();
	for (; it!= texels->end (); it++){
		char ch=(*it )->get_char ();
		if ( ch=='\n'){
			numerator++;	
		}
	}	
	return numerator;
}

size_t texel_caret_c::get_last_row ()
{
	size_t pos = get_position ();
	move_to ( 100000 );
	size_t row, column, position;
	row = get_row ( 0, 0, 0 );
	move_to ( pos );
	return row;
}

void texel_caret_c::inc_line ()
{
	offset_row ( 1 );
}

void texel_caret_c::dec_line ()
{
	offset_row (-1 );
}

void texel_caret_c::inc_column ()
{
	offset ( 1 );
}

void texel_caret_c::dec_column ()
{
	offset (-1 );
}

texel_collapse_c::texel_collapse_c ()
{
	character = 1;	
}

unsigned int texel_collapse_c::get_char ()
{
	return character;
}

texel_color_c::texel_color_c ()
{
	character='\b';
	count = 2;
	color = 0xff0000;
}

unsigned int texel_color_c::get_char ()
{
	return character;
}

texel_tab_c::texel_tab_c ()
{
	character='\t';
}

unsigned int texel_tab_c::get_char ()
{
	return character;
}

texel_newline_c::texel_newline_c ()
{
	character='\n';
}

unsigned int texel_newline_c::get_char ()
{
	return character;
}

texel_char_c::texel_char_c ( const texel_char_c& tl )
{
	glyph = tl.glyph;
	character = tl.character;
}

texel_char_c::texel_char_c ( unsigned int ui )
{
	character = ui;
}

unsigned int texel_char_c::get_char ()
{
	return character;
}

texel_c* texel_c::construct ( unsigned int ui, map<unsigned int, texel_c*>& texel_map )
{
	map<unsigned int, texel_c*>::iterator it;
	it = texel_map.find ( ui );
	int c;
	if ( it == texel_map.end ()){
		switch ( ui ){
		case '\a':
			texel_map[ui]= new texel_caret_c ();
			break;
		case 1:
			texel_map[ui]= new texel_collapse_c ();
			break;
		default :
			texel_map[ui]= new texel_char_c ( ui );
		}
	}
	return texel_map[ui];		
}

unsigned int texel_c::get_char ()
{
}

texel_c::texel_c ()
{
}

texel_c::~texel_c ()
{
}
