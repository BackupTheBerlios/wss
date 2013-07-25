#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>



using namespace std;

#include "wdslib.h"


string get_lambs_path()
{
	char* pe=getenv("LL_PATH");
	if(pe==0){
		cout << "environment error" << endl;
		return string();
	}
	else {
		return string(pe);
	}
}


string utf32_to_utf8(uint32_t chr)
{
	std::string s;
	unsigned char ch;
	if(chr <= 0x7F){
		s+=chr;
		return s;
	}
	if(chr <= 0x7FF){
		ch=chr >> 6;
		ch=ch | 0xC0; 
		s+=ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+=ch;
		return s;
	}
	if(chr <= 0xFFFF){
		ch=chr >> 12;
		ch=ch | 0xE0;
		s+=ch;
		ch= chr >> 6;
		ch= ch & 0x3F;
		ch = ch | 0x80;
		s+=ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+=ch;
		return s;		
	}
	if(chr <= 0x1FFFFF){
	}
		return s;
}



