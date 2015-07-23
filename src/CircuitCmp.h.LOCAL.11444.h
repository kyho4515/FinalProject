/*
	6/28新增
	bool CountInputMethod2(Gate* x, Gate* y);
	void ModifyInput(Gate*);
	void Replace(Gate* gate);
*/

#ifndef CIRCUIT
#define CIRCUIT

#include <vector>
#include "util.h"
#include "CirMgr.h"
#include "Gate.h"
#include "sat/sat.h"
#include "DepList.h"
using namespace std;

class CircuitCmp{
  private:
	//variable
    int CurCutLevel;
    int CurGateLevel;
    CirMgr* circuitOne;
    CirMgr* circuitTwo;
    vector<vector<Wire*>*> _FECpair;
    vector<vector<Wire*>*> cutSet;
    vector<Gate*> dfsListOne;
    vector<Gate*> dfsListTwo;
    vector<Var> outputXor;
    SatSolver solver;
    bool equivalence;
	 DepList* dep_list;
	
	//function
    bool HashKeyCmp(Gate* one, Gate* two){
      if(one -> input.size() == two -> input.size() && one -> gateType == two -> gateType){
        for(int i=0; i < one -> input.size(); ++i){
          bool flag = false;
          for(int j=0; j < two -> input.size(); ++j){
            if(one -> input[i] -> name == two -> input[j] -> name){
              flag = true;
              break;
            }
          }
          if(!flag) return false;
        }
        return true;
      }
      else return false;
    }
    bool Cut(Gate* x, Gate* y);
    bool CheckGoodCut(Gate* x, Gate* y);
    void CountInput(Gate* source, vector<Gate*>&);
	 bool CheckInputEqual2(Gate* x, Gate* y);
	 bool CheckInputEqual1(Gate* x, Gate* y);
    void SimFEC(vector<Gate*>&, vector<Gate*>&);
    bool SimCheck(bool mode,vector<Gate*>& dfsListOne, vector<Gate*>& dfsListTwo);//check circuit equivalent or not
    void DFSearch(Gate* source, vector<Gate*>& dfsList);
    void genProofModel(vector<Gate*>& dfslistOne, vector<Gate*>& dfslistTwo);
    void SimFilter(int, vector<Gate*>&, vector<Gate*>&);
    bool CheckReachability();
    bool CheckReach(Gate* source);
    bool Check(){
      dfsListOne.clear();
      dfsListTwo.clear();
      for(int i=0; i < circuitOne -> output.size(); ++i){
        DFSearch(circuitOne -> output[i],dfsListOne);
        DFSearch(circuitTwo -> output[i],dfsListTwo);
      }
      circuitOne -> resetTraversed();
      circuitTwo -> resetTraversed();
      if(equivalence){
          //cout << "still equivalent" << endl;
        if(SimCheck(1,dfsListOne, dfsListTwo)){
          solver.initialize();
          genProofModel(dfsListOne, dfsListTwo);
          for(int i=0; i < outputXor.size(); ++i){
            solver.assumeProperty(outputXor[i], true);
          }
          solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
          solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
          if(!solver.assumpSolve())
            return true;
          else 
            return false;
        }
        else 
          return false;
        //else assert(0);
      }
      return true;
    }
    bool proveSAT(Wire*, Wire*);
	 void ModifyInput(Gate*);
	 void Replace(Gate* gate);


	bool Check(Wire* a, Wire* b){
      dfsListOne.clear();
      dfsListTwo.clear();
      if(equivalence){
        vector<string> tmp;
        while(true){
          int i=0;
          int j=0;
          if(a -> final_output[i] -> name > b -> final_output[j] -> name || i >= a -> final_output.size()){
            tmp.push_back(b -> final_output[j] -> name);
            ++j;
          }
          else if(a -> final_output[i] -> name < b -> final_output[j] ->name || j >= b -> final_output.size()){
            tmp.push_back(a -> final_output[i] -> name);
            ++i;
          }
          else if(i >= a -> final_output.size() && j >= b -> final_output.size())
            break;
          else{
            tmp.push_back(b -> final_output[j] -> name);
            ++j;
            ++i;
          }
        }
        vector<int> CheckOutputNum; // store the output that should be check
        for(int i=0; i < circuitOne -> output.size(); ++i){
          for(int j=0; j < tmp.size(); ++j){
            if(circuitOne -> output[i] -> name == tmp[j]){
              DFSearch(circuitOne -> output[i], dfsListOne);
              DFSearch(circuitTwo -> output[i], dfsListTwo);
              CheckOutputNum.push_back(i);
              break;
            }
          }
        }
        circuitOne -> resetTraversed();
        circuitTwo -> resetTraversed();
        if(SimCheck(1,dfsListOne, dfsListTwo)){
          solver.initialize();
          genProofModel(dfsListOne, dfsListTwo);
          for(int i=0; i < CheckOutputNum.size(); ++i){
            solver.assumeProperty(outputXor[i], true);
          }
          solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
          solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
          if(!solver.assumpSolve())
            return true;
          else 
            return false;
        }
        else 
          return false;
        //else assert(0);
      }
      else{
        for(int i=0; i < circuitOne -> output.size(); ++i){
          DFSearch(circuitOne -> output[i],dfsListOne);
          DFSearch(circuitTwo -> output[i],dfsListTwo);
        }
        circuitOne -> resetTraversed();
        circuitTwo -> resetTraversed();
        return true;
      }
    }
  public:
      CircuitCmp(const char* file1, const char* file2){
			cout<<endl<<"read circuitone"<<endl<<endl;
      	circuitOne = new CirMgr(file1, 1);
			cout<<endl<<"read circuittwo"<<endl<<endl;
      	circuitTwo = new CirMgr(file2, 2);
      	CurGateLevel = 2;
      	CurCutLevel = 0;
      	if(SimCheck(1, circuitOne -> dfsList, circuitTwo -> dfsList)) 
				equivalence = true;
      	else 
				equivalence = false;
      	solver.initialize();
      	genProofModel(circuitOne -> dfsList, circuitTwo -> dfsList);
      	for(int i=0; i < outputXor.size(); ++i){
        		solver.assumeProperty(outputXor[i], true);
      			}
      	solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
      	solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
      	bool result = solver.assumpSolve();
      	assert((equivalence && !result) || (!equivalence && result));
			//dep_list=new DepList(circuitOne->output,circuitTwo->output);
			//dep_list->Out();
   		 }

    ~CircuitCmp(){
      delete circuitOne;
      delete circuitTwo;
      for(int i=0; i < _FECpair.size(); ++i)
        delete _FECpair[i];
      for(int i=0; i < cutSet.size(); ++i)
        delete cutSet[i];
      delete dep_list;
    }
    bool Simulation(){
      cout << "Original circuitOne gate size :" << circuitOne -> dfsList.size() << endl
           << "Original circuitTwo gate size :" << circuitTwo -> dfsList.size() << endl;
      SimFEC(circuitOne -> dfsList, circuitTwo -> dfsList);
      SimFilter(500, circuitOne -> dfsList, circuitTwo -> dfsList);
      return equivalence;
	}

    void Strash();
    void Sat();
    void Simulation(int l){
      //if(!Check()) assert(0);
      SimFEC(dfsListOne, dfsListTwo);
      SimFilter(500, dfsListOne, dfsListTwo);
    }
    void WriteFile(const char* file1, const char* file2){
      circuitOne->WriteOutputFile(file1);
      circuitTwo->WriteOutputFile(file2);
    }

    void CheckResult(){
      vector<Gate*> dfsOne,dfsTwo;
      for(int i=0; i < circuitOne-> output.size(); ++i){
        dfsOne.clear();
        DFSearch(circuitOne -> output[i], dfsOne);
        int count = 0;
        for(int j=0; j < dfsOne.size(); ++j){
          if(dfsOne[j] -> gateType == Wir && ((Wire*)dfsOne[j])->isCut())
            ++count;
          else if(dfsOne[j] -> gateType != Wir && dfsOne[j] -> gateType != Output && dfsOne[j] -> gateType != Input)
            ++count;
        }
        cout << count << endl;
      }
      for(int i=0; i < circuitTwo-> output.size(); ++i){
        dfsTwo.clear();
        DFSearch(circuitTwo -> output[i], dfsTwo);
        int count = 0;
        for(int j=0; j < dfsTwo.size(); ++j){
          if(dfsTwo[j] -> gateType == Wir && ((Wire*)dfsTwo[j])->isCut())
            ++count;
          else if(dfsTwo[j] -> gateType != Wir && dfsTwo[j] -> gateType != Output && dfsTwo[j] -> gateType != Input)
            ++count;
        }
        cout << count << endl;
      }
    }
	//in My_Simulation.cpp
	bool My_Simulation();
	void CheckDivision();
};
#endif
