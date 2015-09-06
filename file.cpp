#include <fstream>
#include <iostream>
#include <string>
using namespace std;
int main(){
	fstream fin, fout;
	string input;
	int count=0;
	fin.open("error.txt",ios::in);
	fout.open("error.csv",ios::out);
	while(getline(fin,input)){
		fout<<count<<","<<input<<endl;
		count++;
	}
	fin.close();
	fout.close();
}
