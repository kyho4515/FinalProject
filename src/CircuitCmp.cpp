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
	cout<<endl<<"read circuittwo"<<endl<<endl;
   circuitTwo = new CirMgr(file2, 2);
	CurGateLevel = 2;
   CurCutLevel = 0;
	assert(dfsListOne.size()==0 && dfsListTwo.size()==0);
	output_size=circuitOne->output.size();
	input_size=circuitOne->input.size();
	all_output_index=new vector<int>;
	for(int i=0;i<output_size;i++)
		all_output_index->push_back(i);
	dfsListOne=circuitOne->dfsList;
	dfsListTwo=circuitTwo->dfsList;
	assert(&dfsListOne != &(circuitOne->dfsList));
	fout.open("error.txt",ios::out);
	maxcutSet_size=0;
}

bool CircuitCmp::CircuitEquiCheck(bool check){
	/***check 電路是否相等***/
	/*for(int i=0;i<3;i++){
   	if(!SimCheck(true, circuitOne -> dfsList, circuitTwo -> dfsList)){
			equivalence = false;
			return false;
		}
	}*/
	equivalence = true;
	/******/
///用sat證明電路相等或不相等   
	equivalence=CheckOutputEqualSAT();
	return equivalence;
}


bool CircuitCmp::Simulation(){
   return SimFEC(dfsListOne, dfsListTwo);
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
void CircuitCmp::DFSearch(Gate* source, vector<Gate*>& List){
	source->traversed=true;
	if(source->gateType!=Wir || !(((Wire*)source)->isCut())){
		for(int i=0;i<source->input.size();i++){
			if(!(source->input[i]->traversed))
				DFSearch(source->input[i],List);
		}
	}
  	List.push_back(source);
}

void CircuitCmp::resetTraversed(vector<Gate*>& List){
  for(int i=0; i < List.size(); ++i)
    List[i] -> traversed = false;
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
		
void CircuitCmp::RebuiltDFSlist(){
	int one=0,two=0;
	dfsListOne.clear();
	dfsListTwo.clear();
	circuitOne->resetTraversed();
	circuitTwo->resetTraversed();
	for(int i=0;i<circuitOne->output.size();i++){
		DFSearch(circuitOne->output[i],dfsListOne);
		DFSearch(circuitTwo->output[i],dfsListTwo);
	}
	for(int i=0;i<dfsListOne.size();i++)
		if(dfsListOne[i]->gateType==Wir && !(((Wire*)dfsListOne[i])->isCut()) && ((Wire*)dfsListOne[i])->potentialCut)
			one++;
	for(int i=0;i<dfsListTwo.size();i++)
		if(dfsListTwo[i]->gateType==Wir && !(((Wire*)dfsListTwo[i])->isCut()) && ((Wire*)dfsListTwo[i])->potentialCut)
			two++;
	cout<<one<<":"<<two<<endl;
	resetTraversed(dfsListOne);
	resetTraversed(dfsListTwo);
}
bool CircuitCmp::CheckOutputEqualSAT(){

	bool result=SimCheckOutput(all_output_index);
	if(result)
		cout<<"euqivalence"<<endl;
	else{
		cout<<"non-euqivalence"<<endl;
		assert(0);
	}
	//if(equivalence && result){
		for(int i=0;i<circuitOne->output.size();i++){
			if(!proveSAT(circuitOne->output[i],circuitTwo->output[i])){
				return false;
			}
		}
		return true;
/*	}
	else
		return false;
*/	
}

struct intsort{
	bool operator()(const int i1,const int i2){
		return i1>i2;
	}

};
void CircuitCmp::CountScore(){
	vector<int>score;
	int tmp;
	vector<Gate*> store;
	cout<<cutSet.size()<<endl;
	for(int i=0;i<cutSet.size();i++){
		for(int j=0;j<cutSet[i]->size();j++){
			cout<<cutSet[i]->at(j)->name<<endl;
			DFSearch(cutSet[i]->at(j)->input[0],store);
			tmp=0;			
			for(int k=0;k<store.size();k++){
				if(store[k]->gateType!=Input  && store[k]->gateType!=Wir)
					tmp++;
			}
			tmp++;
			if(tmp==1){
				cout<<"tmp=1"<<endl;
				assert(0);
			}
			score.push_back(tmp);
			store.clear();
		}
	}
	for(int i=0;i<circuitOne->output.size();i++){
		DFSearch(circuitOne->output[i]->input[0],store);
		resetTraversed(store);
		tmp=0;
		for(int j=0;j<store.size();j++){
			if(store[j]->gateType!=Input  && store[j]->gateType!=Wir)
				tmp++;		
		}
		score.push_back(tmp);
		store.clear();
		DFSearch(circuitTwo->output[i]->input[0],store);
		resetTraversed(store);
		tmp=0;
		for(int j=0;j<store.size();j++){
			if(store[j]->gateType!=Input && store[j]->gateType!=Wir)
				tmp++;		
		}
		score.push_back(tmp);
		store.clear();
	}
	intsort g;
	sort(score.begin(),score.end(),g);
	for(int i=0;i<score.size()-1;i++)
		cout<<score[i]<<",";
	cout<<score[score.size()-1]<<endl;
}


void CircuitCmp::potentialCutReset(int index){
	for(int i=potentialCutFalseWire.size()-1;i>=index;i--){
		assert(potentialCutFalseWire[i]->gateType==Wir);
		potentialCutFalseWire[i]->potentialCut=true;
		potentialCutFalseWire.pop_back();
	}
}

void CircuitCmp::SetOutputPhase(){
	for(int i=0;i<output_size;i++)
		if(circuitOne->output[i]->curSim==circuitTwo->output[i]->curSim){
			circuitOne->output[i]->phase=true;
			circuitTwo->output[i]->phase=true;
		}	
		else if(circuitOne->output[i]->curSim==~(circuitTwo->output[i]->curSim)){
			circuitOne->output[i]->phase=true;
			circuitTwo->output[i]->phase=false;
		}
		else{
			cout<<"not equivalent"<<endl;
			assert(0);
		}



}
