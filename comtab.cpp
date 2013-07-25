#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <map>
#include <complex>
#include <chrono>
#include <tuple>
#include <cmath>
#include <memory>
#include <utility>

#include "standard.h"

#include "symbol/keysym.h"

#include "library/wdslib.h"
#include "wcp.h"

#include "matrix.h"
#include "position.h"
#include "retine.h"
#include "message.h"

#include "global.h"



#include "object.h"

#include "mouse.h"

#include "eyes.h"
#include "keyb.h"
#include "home.h"
#include "lamb.h"
#include "line.h"
#include "spline.h"
#include "image.h"
#include "land.h"
#include "fox/font.h"
#include "texel.h"
#include "keyb.h"
#include "edit.h"
#include "home.h"
#include "make.h"
#include "comtab.h"

extern land_c land;
extern lamb_c lamb;

void ex_entry_comtab ( int, string );

namespace command_table{

enum {
	NEW_LINE_U,
	NEW_LINE_D,
	COPY_LINE,
	DELETE_LINE,
	CUT_LINE,
	NEXT_WORD_BEG,
	PREV_WORD_BEG,
	NEXT_WORD_END,
	LINE_BEG,
	LINE_END,
	PAGE_D,
	PAGE_U,
	LINE_D,
	LINE_U,
	HOME,
	END,
	PASTE,
	CUT,
	DELETE,
	COPY,
	CHANGE,
	CUT_WORD,
	INC_COLUMN,
	DEC_COLUMN,
	INC_LINE,
	DEC_LINE,
	INC_ROW,
	DEC_ROW,
	SEL,
	CHARACTER,
	SPACE,
	TAB,
	MARK,
	GO_TO_MARK

};

// lamb_c
void change_movement ( int, int ){};
void shift ( object_c*, int, int, int ){};
void move ( int, int, int, int ){};
void button_pressed ( int ){};

// editor_c
void inserting_overwriting ( int, int ){};
void collapsing ( int, int ){};
void marking ( int, int ){};
void moving ( int, int ){};
void change_mode ( int, int ){};
void editing ( int ){};
void deleting ( int, int ){};
void scrolling ( int, int ){};
void runing ( int, int ){};



template<>
symbol_to_identifier_map_c<lamb_c> command_base_c<lamb_c>::symbols = {
change_movement,&lamb_c::change_movement,
shift,(void ( lamb_c::*)(object_c*, int, int, int ))&lamb_c::shift,
move,&lamb_c::move,
button_pressed,&lamb_c::button_pressed
};

/*

template<>
symbol_to_identifier_map_c<editor_c> command_base_c<editor_c>::symbols = {
marking,&editor_c::marking,
inserting_overwriting,&editor_c::inserting_overwriting,
collapsing,&editor_c::collapsing,
moving,&editor_c::moving,
change_mode,&editor_c::change_mode,
editing,&editor_c::editing,
deleting,&editor_c::deleting,
scrolling,&editor_c::scrolling,
runing,&editor_c::runing
};
*/
/*
  #define step 10
command_table_c<lamb_c> ctlamb{
COMMAND, ul_t ( 0 ), XK_period, change_movement, 0, 2,
SHIFT, XK_h, shift,&lamb.sensor,-1, 0, 0,
POINT, XK_i, move, POINT, 0,+step, 0
};
*/
/*
command_table_c<editor_c> cteditor{
COMMAND, ul_t ( 0 ),"0", moving, LINE_BEG, false,
VISUAL, ul_t ( 0 ),"0", moving, LINE_BEG, true,
};
*/

void  entry ( int, string )
{
	O(com entry )
/*
	ctlamb.find(&lamb, COMMAND, ul_t ( 0 ), XK_period,"" );
	ctlamb.find(&lamb, SHIFT, ul_t ( 0 ), XK_h,"" );
	ctlamb.find(&lamb, POINT, ul_t ( 0 ), XK_i,"" );
	object_c* op = (object_c*)land.llist.front ();
	lamb.next_object ( land.llist, op, 1 );
*/	
//	cteditor.find((editor_c*)op, COMMAND, ul_t ( 0 ), XK_0,"" );		
}


} //end namespace command_table


void ex_entry_comtab ( int i, string s )
{
	command_table::entry ( i, s );
}


