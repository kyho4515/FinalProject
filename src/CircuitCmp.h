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
#include "pair.h"
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
    vector<int> CheckOutputNum; // store the output that should be check
	 PotentialPair *Ppair;
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
	bool Check(Wire* a, Wire* b){
      dfsListOne.clear();
      dfsListTwo.clear();
      if(equivalence){
        vector<string> tmp;
        int i=0;
        int j=0;
        while(true){
          if(i >= a -> final_output.size() && j < b -> final_output.size()){
            tmp.push_back(b -> final_output[j] -> name);
            ++j;
          }
          else if(i < a -> final_output.size() && j >= b -> final_output.size()){
            tmp.push_back(a -> final_output[i] -> name);
            ++i;
          }
          else if(i >= a -> final_output.size() && j >= b -> final_output.size())
            break;
          else if(a -> final_output[i] -> name > b -> final_output[j] -> name){
            tmp.push_back(b -> final_output[j] -> name);
            ++j;
          }
          else if(a -> final_output[i] -> name < b -> final_output[j] ->name){
            tmp.push_back(a -> final_output[i] -> name);
            ++i;
          }
          else{
            tmp.push_back(b -> final_output[j] -> name);
            ++j;
            ++i;
          }
        }
        CheckOutputNum.clear();
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
		CircuitCmp(const char* , const char* );
    	~CircuitCmp();  	
   	void Simulation();
    	void Strash();
    	void Sat();
    	void Simulation(int l);
    	void WriteFile(const char* , const char* );
     	bool CircuitEquiCheck();

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
};
#endif
