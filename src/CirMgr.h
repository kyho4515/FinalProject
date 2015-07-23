#ifndef CIRMGR
#define CIRMGR

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "Gate.h"
using namespace std;
typedef map<string, Gate*> gmap;

class CircuitCmp;

class CirMgr{
  friend class Gate;
  friend CircuitCmp;
  private:
	//variable
    vector<Gate*> input;
    vector<Gate*> output;
    vector<Wire*> wire;
    vector<Gate*> gate;
    vector<Gate*> dfsList;
    gmap inputMap;
    gmap outputMap;
    gmap wireMap;
    gmap gateMap;
    Gate* constTrueGate;
    Gate* constFalseGate;

	//function
	void resetTraversed();
  public:
   CirMgr(const char* file, int num){ReadVerilog(file, num);}
   ~CirMgr(){
     for(int i=0; i < input.size(); ++i)
       delete input[i];
     for(int i=0; i < output.size(); ++i)
       delete output[i];
     for(int i=0; i < wire.size(); ++i)
       delete wire[i];
     for(int i=0; i < gate.size(); ++i)
       delete gate[i];
     delete constTrueGate;
     delete constFalseGate;
   }
   bool ReadVerilog(string inputFile, int num);
   void DFSearch(Gate*);
   void WriteOutputFile(const char* );
	 vector<vector<Gate*>* > Getdep_output();
	 vector<vector<string>* >Getdep_input();	
	 Gate* Getoutput(int);
	 int GetoutputSize(); 
};

#endif
