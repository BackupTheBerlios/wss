#ifndef COMTAB_H
#define COMTAB_H


struct ul_t{
	ul_t (){};
	ul_t ( const unsigned long&  _ul ){ul = _ul;}
	operator unsigned long (){ return (static_cast<unsigned long>(ul ));}
	unsigned long ul;
};

class con_c{
	public:
	con_c (){}
	con_c ( const con_c& _con ){ i = _con.i;}
	con_c ( int _i ){ i = _i;}
	operator int (){return i;}
	int i;	
};

const con_c POINT = _POINT;
const con_c SHIFT = _SHIFT;
const con_c ROTATE = _ROTATE;
const con_c SHIFT_OBJECT = _SHIFT_OBJECT;
const con_c ROTATE_OBJECT = _ROTATE_OBJECT;
const con_c VISUAL = _VISUAL;
const con_c COMMAND = _COMMAND;
const con_c INSERT = _INSERT;

namespace command_table{

//editor_c
void marking ( int, int );
void inserting_overwriting ( int, int );
void collapsing ( int, int );
void moving ( int, int );
void change_mode ( int, int );
void editing ( int );
void deleting ( int, int );
void scrolling ( int, int );
void runing ( int, int );

//lamb_c
void change_movement ( int, int );
void shift ( object_c*, int, int, int );
void move ( int, int, int, int );
void button_pressed ( int );



template<typename T>
class symbol_to_identifier_map_c
{
	public:

	template<typename... Targs>
	symbol_to_identifier_map_c ( Targs... Fargs )
	{
		load ( Fargs...);
	}
	
	void load ()
	{
		return;
	}
	
	template<typename T1, typename T2, typename... Targs>
	void load ( T1 val1, T2 val2, Targs... Fargs )
	{
		void * vp = new T2 ( val2 );
		symbol_map.insert ( pair<void*, void*>((void *)val1, vp ));
		load ( Fargs ...);
		return;
	}
	void* at ( void* key ){
		return symbol_map.at ( key );
	}
	map<void*, void*> symbol_map;
};

template <typename T>
class command_c; 

template<typename T1, typename T2>
void shof1 ( T1* o, typename list<command_c<T1>>::iterator& it );

template<typename T1, typename T2>
void shof2 ( T1* o, typename list<command_c<T1>>::iterator& it );

template<typename T1, typename T2>
void shof3 ( T1* o, typename list<command_c<T1>>::iterator& it );

template<typename T1, typename T2>
void shof4 ( T1* o, typename list<command_c<T1>>::iterator& it );

template<typename  T>
class command_base_c
{	
	static symbol_to_identifier_map_c<T> symbols;

	using shoty = void(*)(T*, typename list<command_c<T>>::iterator&);
	using tutype = tuple<const type_info*, void*, int, shoty, void*>;

	public:
	operator const type_info* (){return (type_info*)get<0>(tu );} 

	command_base_c (){}
	
	command_base_c ( con_c con ){sets ( con );}
	command_base_c ( ul_t ult ){sets ( ult );}
	command_base_c ( string s ){sets ( s );}
	command_base_c ( char* cp ){sets ( string ( cp ));}
	command_base_c ( int i ){sets ( i );}
	command_base_c ( unsigned long ul ){sets ( ul );}
	command_base_c ( object_c** opp ){sets ( opp );}
	
	tutype tu;
	
	template<typename T0>
	void sets ( T0 t ){
		get<0>(tu ) = &typeid ( T0 );
		get<1>(tu ) = new T0 ( t );
	}
	void erase (){delete get<1>(tu );}	
	int get_count (){return get<2>(tu );}

	operator unsigned long (){return *(unsigned long*)get<1>(tu );}
	operator int (){return *(int*)get<1>(tu );}
	operator string (){ return *(string*)get<1>(tu );}
	operator object_c* (){ return **(object_c***)get<1>(tu );}

	command_base_c ( void(*f )(string )){
		using t = void ( T::*)(string );
		setnm ( 1, shof1<T, t>,(t*)symbols.at((void*)f ));
	}
	
	command_base_c ( void(*f )(int ))
	{
		using t = void ( T::*)(int );
		setnm ( 1, shof1<T, t>,*(t*)symbols.at((void*)f ));
	}
	
	command_base_c ( void(*f )(int, int ))
	{
		using t = void ( T::*)(int, int );
		setnm ( 2, shof2<T, t>,*(t*)symbols.at((void*)f ));
	}
	
	command_base_c ( void(*f )(int, string ))
	{
		using t = void ( T::*)(int, string );
		setnm ( 2, shof2<T, t>,*(t*)symbols.at((void*)f ));
	}
	
	command_base_c ( void(*f )(int, int, int ))
	{
		using t = void ( T::*)(int, int, int );
		setnm ( 3, shof3<T, t>,*(t*)symbols.at((void*)f ));
	}

	command_base_c ( void(*f )(int, int, int, int ))
	{
		using t = void ( T::*)(int, int, int , int );
		setnm ( 4, shof4<T, t>,*(t*)symbols.at((void*)f ));
	}
	
	command_base_c ( void(*f )(object_c*, int, int, int ))
	{
		using t = void ( T::*)(object_c*, int, int, int );
		setnm ( 4, shof4<T, t>,*(t*)symbols.at((void*)f ));
	}
	
	template<typename T0>
	void setnm ( int c, shoty sho, T0 t )
	{
		get<0>(tu ) = &typeid ( T0 );
		get<1>(tu ) = new T0 ( t );
		get<2>(tu ) = c;
		get<3>(tu ) = sho;
	}

	//static symbol_to_identifier_map_c<T> symbols;
};

template <typename T>
class command_c : public command_base_c<T>
{
	public:
	command_c (){}
	command_c ( con_c con ):command_base_c<T>(con ){}
	command_c ( ul_t ult ):command_base_c<T>(ult ){}
	command_c ( string s ):command_base_c<T>(s ){}
	command_c ( char* cp ):command_base_c<T>(cp ){}
	command_c ( int i ):command_base_c<T>(i ){}
	command_c ( object_c** opp ):command_base_c<T>(opp ){}
	command_c ( unsigned long ul ):command_base_c<T>(ul ){}
	command_c ( void(*f )(string )):command_base_c<T>(f ){}
	command_c ( void(*f )(int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, string )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int, int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(object_c*, int, int, int )):command_base_c<T>(f ){}

}; 

template<>
class command_c <lamb_c>: public command_base_c<lamb_c>
{
	public:
	using T = lamb_c;
	command_c (){}
	command_c ( con_c con ):command_base_c<T>(con ){}
	command_c ( ul_t ult ):command_base_c<T>(ult ){}
	command_c ( string s ):command_base_c<T>(s ){}
	command_c ( char* cp ):command_base_c<T>(cp ){}
	command_c ( int i ):command_base_c<T>(i ){}
	command_c ( object_c** opp ):command_base_c<T>(opp ){}
	command_c ( unsigned long ul ):command_base_c<T>(ul ){}
	command_c ( void(*f )(string )):command_base_c<T>(f ){}
	command_c ( void(*f )(int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, string )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(int, int, int, int )):command_base_c<T>(f ){}
	command_c ( void(*f )(object_c*, int, int, int )):command_base_c<T>(f ){}
}; 

template<typename T1, typename T2>
void shof1 ( T1* o, typename list<command_c<T1>>::iterator& it )
{
	T2 m = *(T2*)get<1>(it->tu );
	(o->*m )(*next ( it, 1 ));
	int c = get<2>(it->tu );
	advance ( it, c+1 );
}

template<typename T1, typename T2>
void shof2 ( T1* o, typename list<command_c<T1>>::iterator& it )
{
	T2 m = *(T2*)get<1>(it->tu );
	(o->*m )(*next ( it, 1 ),*next ( it, 2 ));
	int c = get<2>(it->tu );
	advance ( it, c+1 );
}

template<typename T1, typename T2>
void shof3 ( T1* o, typename list<command_c<T1>>::iterator& it )
{
	T2 m = *(T2*)get<1>(it->tu );
	(o->*m )(*next ( it, 1 ),*next ( it, 2 ),*next ( it, 3 ));
	int c = get<2>(it->tu );
	advance ( it, c+1 );
}

template<typename T1, typename T2>
void shof4 ( T1* o, typename list<command_c<T1>>::iterator& it )
{
	T2 m = *(T2*)get<1>(it->tu );
	(o->*m )(*next ( it, 1 ),*next ( it, 2 ),*next ( it, 3 ),*next ( it, 4 ));
	int c = get<2>(it->tu );
	advance ( it, c+1 );
}

template<typename T1>
class command_table_c  
{
	using T2 = command_c<T1>;
	public:
	command_table_c (){};
	command_table_c ( initializer_list<T2> il ){
		for ( auto x : il ) {
			lmyo.push_back ( x );
		}
	}
	~command_table_c (){
		for( auto x: lmyo ){
			x.erase ();
		}
	}
	list<T2> lmyo;
	
	virtual bool find ( T1* object, con_c, unsigned long control, unsigned long stroke, string chr );	string characters;
	
	static map < pair< int, T1* >, command_table_c < T1 > * > ctl2;
	static bool ssfind ( int i, T1* object, con_c, unsigned long control, unsigned long stroke, string chr );
	
};

template<typename T1>
bool command_table_c<T1>::ssfind ( int i, T1* object, con_c mode, unsigned long control, unsigned long stroke, string chr )
{
	pair< int, T1* > p ( i, object );
	auto pct = ctl2.find ( p );
//	if (pct != map<T1*, command_table_c<T1>*>::end ){
		pct -> second->find (object, mode, control, stroke, chr );
//	}
}

template<typename T1>
map < pair< int, T1* >, command_table_c<T1>*> command_table_c<T1>::ctl2;

template<typename T1>
bool command_table_c<T1>::find ( T1* object, con_c mode, unsigned long control, unsigned long stroke, string chr )
{
	if ( stroke == XK_Shift_R ) {
//		echo << "kars3 " << endl;
		return true;
	};
	unsigned long ul;
	string s;
	int i;
	auto it = lmyo.begin (), ite = lmyo.end ();
	for ( ; it != ite; it++ ) {
		const type_info* ty = *it;
		bool flag = true;
//		O ( find0 )	
		if ( *ty != typeid ( con_c )){
			continue;
		}
		i = *it;
//		OI ( mode, mode )
		if (i != mode ){
			flag = false;
		}
		ty = *++it;
		if (*ty == typeid ( ul_t )){
			ul = *it;
//			OI ( control, control )
			if ( ul != control ){
				flag = false;
			}
			ty = *++it;
		}
		if ( *ty == typeid ( int )){
			ul = *it;
//			OI ( stroke, stroke )
			if ( ul != stroke ){
				flag = false;
			}
			ty = *++it;			
		}
		if ( *ty == typeid ( string )){
			s = string ( *it );
			if ( flag == true ){
				if ( s.substr ( 0, characters.size () + 1 ) == characters + chr ){								characters = characters + chr;
					if ( characters.size () != s.size ()){
						return true;
					}
				}
				else {
					flag = false;
				}
			}
			ty = *++it;
		}
		if ( flag == true ){
			void(*f )(T1*, typename list<T2>::iterator&) = (void(*)(T1*, typename list<T2>::iterator&))get<3>(it->tu );
			
			f ( object, it );
			
			characters = "";
			return true;
		}
		advance ( it, it->get_count ());
	}
	characters = "";
	return false;
}

} //end command_table namespace

#endif
