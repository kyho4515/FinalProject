#ifndef CIRMGR
#define CIRMGR

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "Gate.h"
#include "ConstWire.h"
using namespace std;
typedef map<string, Gate*> gmap;

class CircuitCmp;

class CirMgr{
  friend class Gate;
  friend CircuitCmp;
  private:
//variable
   vector<Gate*> input;//紀錄gateType是input的gate
   vector<Gate*> output;//紀錄gateType是output的gate
   vector<Wire*> wire;//紀錄gateType是Wir的gate
   vector<Gate*> gate;//紀錄gateType是其他的gate
   vector<Gate*> dfsList;
   gmap inputMap;
   gmap outputMap;
   gmap wireMap;
   gmap gateMap;
   Gate* constTrueGate;//值為1的input
   Gate* constFalseGate;//值為0的input
	ConstWire *const_wire;//是constant的gate(1 or 0)
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
	void FindConstWire();
};

#endif
