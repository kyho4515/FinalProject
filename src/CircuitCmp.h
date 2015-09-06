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
	 int output_size;
	 int input_size;
	 int maxcutSet_size;
    CirMgr* circuitOne;
    CirMgr* circuitTwo;
    vector<vector<Wire*>*> _FECpair;
    vector<vector<Wire*>*> cutSet;
	 vector<int>cut_potentialCutFalseWire_index;//當cut成立時，儲存那時候的potentialCutFalseWire的長度
	 vector<Wire*> potentialCutFalseWire;//儲存potentialCut 被設成false的wire
    vector<Gate*> dfsListOne;
    vector<Gate*> dfsListTwo;
    vector<Var> outputXor;
	 vector<int> start_record;
	 vector<int> *all_output_index;
    SatSolver solver;
    bool equivalence;
    vector<vector<int>*> CheckOutputNum; // store the output that should be check
	 PotentialPair *Ppair;
	 fstream fout;
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
	 bool Cut(int, Wire*,bool=false);
    bool CheckGoodCut(Gate* x, Gate* y);
    void CountInput(Gate* source, vector<Gate*>&);
	 bool CheckInputEqual2(Gate* x, Gate* y);
	 bool CheckInputEqual1(Gate* x, Gate* y);
	 static bool sortcompare(const Wire* l,const Wire* r){  //sort的判斷式
			return l->gateLevel < r->gateLevel;		
	   }
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
              //CheckOutputNum.push_back(i);
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
	//in sat.cpp
	bool SATCheckOutput(vector<int>*,bool=false);//用sat確認所有output相等
	void genProofModel(vector<Gate*>& , vector<Gate*>& ,int =-1);//if in the same circuit mode = true
	bool proveSAT(Gate*, Gate*);
	//in CircuitCmp.cpp
	void DFSearch(Gate* source, vector<Gate*>& dfsList);
	void DFS(Gate*);
	void potentialCutReset(int);
	void resetTraversed(vector<Gate*>&);
	//in simulation.cpp
	bool SimFEC(vector<Gate*>&, vector<Gate*>&);
   bool SimCheck(bool mode,vector<Gate*>& dfsListOne, vector<Gate*>& dfsListTwo,int=1);//check circuit equivalent or not
	void SimInitialize();//input和cut給值 
	void SimFilter(int, vector<Gate*>&, vector<Gate*>&);
	void SimLeveling();
	void SimSATcheck(bool=false);
	bool SimCheckLevel(Wire* ,Wire* );//確認兩條wire是否互不包含，互不包含回傳false
	bool SimCheckOutput(vector<int>*,int=1);//用simulation確認所有output都相等
	//in strash.cpp
	bool proveDontCareSim(Wire*);
	bool proveDontCareSat(Wire*);
	void cutSetClear(int,int);//清除指定範圍內的cutSet
	//in shortcut.cpp
	void upwardCut(vector<Wire*>*);//如果找到的cut只連接到buf那就連上面的一起cut
	void findCutfromFECpair(vector<Wire*>*);//從有兩個circuit的wire的FECpair找cut
	void upwardSetFalse(Wire* );//如果確定有一個wire不能被cut且他只有連接到buf，那就連上面的buf一起設成不能被cut
	void FindWrongCut();//找到錯誤的cutSet並移除
	public:
		CircuitCmp(const char* , const char* );
    	~CircuitCmp();  	
   	bool Simulation();
    	void Strash();
    	void Sat(int=-1);
    	void Simulation(int l);
    	void WriteFile(const char* , const char* );
     	bool CircuitEquiCheck(bool=false);
		void RebuiltDFSlist();
		bool CheckOutputEqualSAT();
		void CountScore();
		void SetOutputPhase();

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
		int FECsize(){return _FECpair.size();}
};
#endif
