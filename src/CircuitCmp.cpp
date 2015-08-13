/*
	6/28修改
	bool CircuitCmp::CheckGoodCut(Gate* x, Gate* y)		
	
	6/28新增
	bool CircuitCmp::CheckInputEqual1(Gate* x, Gate* y)
	bool CircuitCmp::CheckInputEqual2(Gate* x, Gate* y)
	void ModifyInput(Gate*);
	void Replace(Gate* gate)
*/

#include "CircuitCmp.h"

CircuitCmp::CircuitCmp(const char* file1, const char* file2){
	/***read file***/
	cout<<endl<<"read circuitone"<<endl<<endl;
   circuitOne = new CirMgr(file1, 1);
	circuitOne->FindConstWire();
	assert(0);
	cout<<endl<<"read circuittwo"<<endl<<endl;
   circuitTwo = new CirMgr(file2, 2);
	circuitTwo->FindConstWire();
      	
	CurGateLevel = 2;
   CurCutLevel = 0;
	
	/***find const wire***/
	
	
	assert(0);
}

bool CircuitCmp::CircuitEquiCheck(){
	/***check 電路是否相等***/
   if(SimCheck(1, circuitOne -> dfsList, circuitTwo -> dfsList)) 
		equivalence = true;
   else 
		equivalence = false;
	/******/

///用sat證明電路相等或不相等    	
	CheckOutputNum.clear();
	solver.initialize();
   for(int i=0; i < circuitOne->output.size(); ++i)
   	CheckOutputNum.push_back(i);
   genProofModel(circuitOne -> dfsList, circuitTwo -> dfsList);        	
	for(int i=0; i < outputXor.size(); ++i){
      solver.assumeProperty(outputXor[i], true);
        }
	/***true gate 和 false gate 加入assumption***/
   solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
  	solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
	/******/	
   bool result = solver.assumpSolve();
   assert((equivalence && !result) || (!equivalence && result));
	return equivalence;
}


void CircuitCmp::Simulation(){
   SimFEC(circuitOne -> dfsList, circuitTwo -> dfsList);
   SimFilter(500, circuitOne -> dfsList, circuitTwo -> dfsList);
}

void CircuitCmp::Simulation(int l){
	//if(!Check()) assert(0);
   SimFEC(dfsListOne, dfsListTwo);
   SimFilter(500, dfsListOne, dfsListTwo);
}


bool CircuitCmp::CheckGoodCut(Gate* x, Gate* y){
	for(int i=0; i < circuitOne -> input.size(); ++i){
   	circuitOne -> input[i] -> included = false;
   	circuitTwo -> input[i] -> included = false;
	}
   for(int i=0; i < cutSet.size(); ++i){
   	for(int j=0; j < cutSet[i] -> size(); ++j)
      	cutSet[i] -> at(j) -> included = false;
  	}

  	bool flag = false;
  	for(int i=0; i < x -> output.size(); ++i){
    	if(x -> output[i] -> gateType != Output)
      	flag = true;
  	}
  	if(!flag) return false;
  	flag = false;
  	for(int i=0; i < y -> output.size(); ++i){
    	if(y -> output[i] -> gateType != Output)
      	flag = true;
  	}
  	if(!flag) return false;
	vector<Gate*> inputX, inputY;
  	CountInput(x, inputX);
  	CountInput(y, inputY);

  	circuitOne -> resetTraversed();
  	circuitTwo -> resetTraversed();
  
  	map<string, Gate*> inputMap;
  	map<int, Gate*> cutMap;
  	for(int i=0; i < inputX.size(); ++i){
    	if(inputX[i] -> gateType == Input)
      	inputMap.insert(pair<string, Gate*>(inputX[i] -> name, inputX[i]));
    	else{
      	cutMap.insert(pair<int, Gate*>(((Wire*)inputX[i]) -> cutNum, inputX[i]));
    		}
  	}
  	for(int i=0; i < inputY.size(); ++i){
    	if(inputY[i] -> gateType == Input){
      	if(inputMap.find(inputY[i] -> name) == inputMap.end())
        	return false;
    		}
    	else{
      	if(cutMap.find(((Wire*)inputY[i]) -> cutNum) == cutMap.end())
        		return false;
    		}
  	}
  	return true;
}

void CircuitCmp::CountInput(Gate* source, vector<Gate*>& list){
  	if(source -> gateType == Input){
    	source -> included = true;
    	list.push_back(source);
  	}
  	for(int i = 0; i < source -> input.size(); ++i){
    	if(!(source -> input[i] -> traversed)){
      	if(source -> input[i] -> gateType == Wir && ((Wire*)(source -> input[i])) -> isCut()){
        		source -> input[i] -> traversed = true;
        		source -> input[i] -> included = true;
        		list.push_back(source -> input[i]);
      			}
      	else {
        		source -> input[i] -> traversed = true;
        		CountInput(source -> input[i], list);
      			}
    		}
  	}
}
void CircuitCmp::DFSearch(Gate* source, vector<Gate*>& dfsList){
  	for(int i = 0; i < source -> input.size(); ++i){
    	if(!(source -> input[i] -> traversed)){
      	if(source -> input[i] -> gateType == Wir && ((Wire*)(source -> input[i])) -> isCut()){
        		source -> input[i] -> traversed = true;
        		dfsList.push_back(source -> input[i]);
      			}
      	else {
        		source -> input[i] -> traversed = true;
        		DFSearch(source -> input[i], dfsList);
      			}
    		}
  	}
  	dfsList.push_back(source);
}


CircuitCmp::~CircuitCmp(){
	delete circuitOne;
   delete circuitTwo;
   for(int i=0; i < _FECpair.size(); ++i)
   	delete _FECpair[i];
   for(int i=0; i < cutSet.size(); ++i)
      delete cutSet[i];
}

void CircuitCmp::WriteFile(const char* file1, const char* file2){
	circuitOne->WriteOutputFile(file1);
   circuitTwo->WriteOutputFile(file2);
}
		

