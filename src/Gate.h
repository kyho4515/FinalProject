/*  6/28新增項目
	in class Gate
	vector<string> total_input_name;
	vector<int> total_cut_name;
	6/28修改項目
	in class InputGate
	InputGate(string n, int a);
	
	7/1修正
	in class Gate
	vector<vector<int>> check_equivalent;

"
*/

#ifndef __GATE__
#define __GATE__

#include <iostream>
#include <stdio.h>
#include <vector>
#include "sat/sat.h"
class CirMgr;
using namespace std;

enum GateType{
  Input = 0,
  Output = 1,
  And = 2,
  Or = 3,
  Xor = 4,
  Buf = 5,
  Wir = 6,
  Const = 7
};

class Gate{
  friend class CirMgr;
  friend class GateKey;
  friend class CircuitCmp;
public:
	//variables
  int gateLevel;
  string name;
  int idd;
  GateType gateType;
  Var var;
  bool phase;
  int curSim;
  bool included;
  bool traversed;
  vector<Gate*> input;//fanin
  vector<Gate*> output;//fanout
  vector<Gate*> final_output;//最後連接到的output 用class FinalOutput實作
  vector<Gate*> total_input;//trace到最前面的input 用class FinalOutput實作
  vector<Gate*> total_cut;//cut後當作input的wire 用class FinalOutput實作

	//functions
  Gate(string n, int a, GateType i):name(n),idd(a), gateType(i), traversed(false){curSim = 0;}
  Gate(string n, int a, GateType i, bool p):name(n),idd(a), gateType(i), traversed(false), phase(p){curSim = 0;}
  Gate(string n, int a, GateType i, bool p, int value):name(n),idd(a), gateType(i), traversed(false), phase(p),curSim(value){}
  virtual void operate() = 0;
  virtual void constructSat(SatSolver& s,Var&) = 0;
  void setVar(Var& v){var = v;}
  Var getVar() const {return var;}
};

class InputGate:public Gate{
  public:
	bool repeat;    
	InputGate(string n, int a): Gate(n,a,Input, true){included = false;total_input.push_back(this);gateLevel=0;}
    void operate(){}
    void constructSat(SatSolver& s, Var&){}
	
  private:
};

class ConstGate:public Gate{
  public:
    ConstGate(const string n, int a, bool p, int value): Gate(n, a, Const, p, value){gateLevel=0;}
    void operate(){}
    void constructSat(SatSolver& s, Var& Const){}
  private:
	
};

class OutputGate:public Gate{
  public:
    OutputGate(string n, int a): Gate(n,a,Output){groupNum=-1;final_output.push_back(this);}
	 int groupNum; 
 	 void operate();
    void constructSat(SatSolver& s, Var& Const){}
  private:

};
class AndGate:public Gate{
  public:
    AndGate(string n, int a,bool p): Gate(n,a,And,p){}
    void operate();
    void constructSat(SatSolver& s, Var&);
  private:
};

class OrGate:public Gate{
  public:
    OrGate(string n, int a,bool p): Gate(n,a,Or,p){}
    void operate();
    void constructSat(SatSolver& s, Var&);
  private:
};

class XorGate:public Gate{
  public:
    XorGate(string n, int a, bool p): Gate(n,a,Xor,p){}
    void operate();
    void constructSat(SatSolver& s, Var&);
  private:
};

class Wire:public Gate{
  public:
    Wire(string n, int a, int i): Gate(n,a,Wir){cut = false;circuitNum = i;included = false;potentialCut = false; cutLevel = INT_MAX;reachable = 0;mark=false;}
    void operate();
    bool isCut(){return cut;}
    void Cut(){cut = true;}
    void CutReset(){cut = false;}
    void constructSat(SatSolver& s, Var&){};
    int cutNum;
    int FECNum;
    int circuitNum;
    int cutLevel;
    bool potentialCut;
    bool reachable;
	 vector<Wire*> backwire;
	 bool mark;//將已經確認在FEC_pair中沒有任何上下關係的wire mark
  private:
    bool cut;
};

class BufGate:public Gate{
	public:
  		BufGate(string n, int a, bool p): Gate(n,a,Buf,p){}
    	void operate();
    	void constructSat(SatSolver& s, Var& Const){
      	assert(input.size() == 1);
			/*cout<<"output name:"<<output[0]->name<<endl;
			cout<<output[0]->getVar()<<endl;
			cout<<"input name:"<<input[0]->name<<endl;
			cout<<input[0]->getVar()<<endl;*/
      	s.addAigCNF(output[0]->getVar(), input[0] -> getVar(), !phase, Const, false);
    		}
  private:
};

class GateKey{
  public:
    GateKey(Gate* gate);
    bool operator == (GateKey k){
      if(k.key == key) return true;
      else return false;
    }
    int operator()(){
      return key;
    }
  private:
    size_t key;
    size_t transform(string);
};
#endif
