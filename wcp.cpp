#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

#include "wcp.h"

using namespace std;

string path;
string  lamb0; // main program 
string  lambb; // the backup for recovery
string lambo;  // the output of the build 
string  exit_cfg;
string make;
string exe;

int main (int argc, char* arg[])
{
	char* pe;
	pe=getenv ("LL_PATH");
	if (pe==0){
		cout << "environment variable LL_PATH not set" << endl;
		return 0;
	}
	string path (pe);
	
	string s;
	string a0,a1,a2;
	if (argc>=2){
		a1=arg[1];
	}
	if (a1== "-s"){
		if (argc <3 ){
			return 0;
		}
		ofstream f;
		f.open (path+ "/config/environment");
		a2=arg[2];
		if (a2 == "linux"){
			f << "makelinux" << endl;
			f << "linux" << endl;
		}
		else if (a2=="sdl"){
			f << "makesdl" << endl;
			f << "sdl" << endl;
		}
		else if (a2=="screen"){
			f << "makescreen" << endl;
			f << "screen" << endl;
		}
		else if (a2=="x11"){
			f << "makex11" << endl;
			f << "x11" << endl;
		}
		else if (a2=="win"){
			f << "makewin" << endl;
			f << "win" << endl;
		}
		return 0;
	}
	ifstream f (path+"/config/environment");
	if (!f.good ()){
		cout << "error: no environment file" << endl;
		return 0;
	}
	f >> make;
	f >> lamb0;
	f.close ();
	cout << path << endl;
	cout << make << endl;
	cout << lamb0 << endl;

	lamb0=path + "/" + lamb0;
	lambo=lamb0 + "_out"; 
	lambb=lamb0+"_save";
	exit_cfg=path + "/config/exitcfg";
	int result;	
	if (a1=="-r"){
		ofstream f (exit_cfg);
		f << wcp::restore << endl;
		f.close ();
	}
	else if (a1=="-save"){
		s="cp " + lamb0 + " " + lambb;
		system (s.c_str ());
		return 0;
	}
	else {	
		ofstream f (exit_cfg);
		f << wcp::start << endl;
		f.close ();
	}
	while (true){
		ifstream f;
		f.open (exit_cfg.c_str ());	
		if (f.good ()){
			f >> result;
		}
		else{
			result=0;
		}
		f.close ();
		if ( a1 == "-e" ) {
			s =  path + "/sdl";
			system (s.c_str ());
			a1 = "";
			continue;
		}

		//restore
		if (result==wcp::restore){   
			s="cp " + lambb + " " + lamb0;
			system (s.c_str ());
			result=system (lamb0.c_str ());
			continue;
		}
		//quit
		if (result==wcp::quit){
			break;
		}
		//save
		if (result==wcp::save){
			s="cp " + lamb0 + " " + lambb;	
			system (s.c_str ());
			s=lamb0;
			result=system (s.c_str ());
			continue;
			
		}
		//start
		if (result==wcp::start){
			s=lamb0 + " " + a1;
			system (s.c_str ());
			continue;
		}
		//restart
		if (result==wcp::restart){
			s="rm " + exit_cfg;
			system (s.c_str ());
			cout << "launch" << endl;
			s="cp "+ lambo + " " + lamb0;
			system (s.c_str ());
			system (lamb0.c_str ());
			continue;
		}
	}	
	cout << "launcher exit"<< endl;
	return 0;
}
