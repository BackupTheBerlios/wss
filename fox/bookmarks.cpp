#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "standard.h"

#include "bookmarks.h"

using namespace std;

bookmarks_c::bookmarks_c()
{
}

void bookmarks_c::add_to_cash(string& url,stringstream& page)
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	int max_nr=10;
	string cash=lynxs_path+"/cash";
	string path=lynxs_path+"/pages";
	string nr_s;	
	int nr;
	fstream f;
	f.open(cash,ios::in);
	if(!f.is_open()){
		cout << "not good" << endl;
		f.close();
		f.open(cash,ios::out);
		nr =1;
		f << nr << endl;
		f << url;
	}
	else{
		cout << "good" << endl;
		stringstream ss1;
		ss1 << f.rdbuf();
		f.close();		
		f.open(cash,ios::out);
		getline(ss1,nr_s);
//!!!		nr=stoi(nr_s);
		if(nr < max_nr){
			nr++;
		}
		else{
			nr=1;
		}
		f << nr << endl;
		cout << nr << endl;
		for(int c=1;c <= max_nr;c++){
			string s1;
			getline(ss1,s1);
			if(c!=nr){
				f << s1 << endl;
			}
			else{
				f << url << endl;
			}
			if(ss1.eof() && nr<=c){
				break;
			}
		}
	}
	ofstream f_page;
	f_page.open(path+ "/" + sc11::to_string(nr));
	f_page << page.str();
}

bool bookmarks_c::look_in_cash(string& url, string& name)
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	string line;
	ifstream f(lynxs_path + "/cash");
	if(!f.good()){
		return false;
	}
	getline(f,line);
	for(int page=1;!f.eof();page++){
		getline(f,line);
		if(line==url){
			name=lynxs_path+"/pages/"+sc11::to_string(page);
			return true;
		}			
	}
	return false;	
}



