#include "CircuitCmp.h"

bool CircuitCmp::Cut(Gate* x, Gate* y){
  	bool isCut = false;
  	Wire* ptr;
  	if(x -> output[0] -> gateType != Output){
   	if(((Wire*)x -> output[0]) -> isCut()){
      	isCut = true;
      	ptr = (Wire*)(x -> output[0]);
    		}
    	else
      	isCut = false;
  	}
  	if(!isCut){
    	if(y -> output[0] -> gateType != Output){
      	if(((Wire*)y -> output[0]) -> isCut()){
        		isCut = true;
        		ptr = (Wire*)(y -> output[0]);
      			}
      	else
        		isCut = false;
    		}
  	}
  int Cutnum;
  if(!isCut){
    Cutnum = cutSet.size();
    cutSet.push_back(new vector<Wire*>);
    ((Wire*)x -> output[0]) -> Cut(); 
    ((Wire*)x -> output[0]) -> cutNum = Cutnum;
    cutSet[Cutnum] -> push_back((Wire*)x -> output[0]);
    x -> output[0] -> phase = true;
  }
  else Cutnum = ptr -> cutNum;
  ((Wire*)y -> output[0]) -> Cut(); 
  ((Wire*)y -> output[0]) -> cutNum = Cutnum;
  cutSet[Cutnum] -> push_back((Wire*)y -> output[0]);
  if(y -> output[0] -> curSim == x -> output[0] -> curSim)
    y -> output[0] -> phase = x -> output[0] -> phase;
  else y -> output[0] -> phase = !(x -> output[0] -> phase);
  
  //Check output right after cutting
  if(equivalence){
    if(!Check((Wire*)x, (Wire*)y)){
      if(!isCut){
        delete cutSet[Cutnum];
        cutSet.pop_back();
        ((Wire*)x -> output[0]) -> CutReset();
      }
      else
        cutSet[Cutnum] -> pop_back();
      ((Wire*)y -> output[0]) -> CutReset();
      dfsListOne.clear();
      dfsListTwo.clear();
      for(int i=0; i < circuitOne->output.size(); ++i){
        DFSearch(circuitOne->output[i], dfsListOne);
        DFSearch(circuitTwo->output[i], dfsListTwo);
      }
      circuitOne->resetTraversed();
      circuitTwo->resetTraversed();
      assert(SimCheck(1,dfsListOne,dfsListTwo));
      solver.initialize();
      genProofModel(dfsListOne,dfsListTwo);
      return false;
    }
  }
  
  return true;
}


bool CircuitCmp::Cut(int set_index,Wire* uncheckWire,bool check){
	assert(cutSet[set_index]->size()!=0 && !(uncheckWire->isCut()));
	//put uncheckWire into cutSet	
	cutSet[set_index]->push_back(uncheckWire);
	for(int i=0;i<cutSet[set_index]->size();i++){
		cutSet[set_index]->at(i)->Cut();
		cutSet[set_index]->at(i)->cutNum=cutSet[set_index]->at(0)->cutNum;
	}
	if(check)
		cout<<"start find output_id"<<endl;
	//find output_id of those outputs that should be check
	vector<int>*unckeckOutput_id=new vector<int>;
	int CheckOutputNumSize=CheckOutputNum[set_index]->size();
	int uncheckWire_outputSize=uncheckWire->final_output.size();
	for(int i=0,j=0;i<CheckOutputNumSize || j<uncheckWire_outputSize;){
		if(i==CheckOutputNumSize || (j!=uncheckWire_outputSize && CheckOutputNum[set_index]->at(i) > uncheckWire->final_output[j]->idd)){
			unckeckOutput_id->push_back(uncheckWire->final_output[j]->idd);
			j++;
		}
		else if (j==uncheckWire->final_output.size() || CheckOutputNum[set_index]->at(i) < uncheckWire->final_output[j]->idd){
			unckeckOutput_id->push_back(CheckOutputNum[set_index]->at(i));
			i++;
		}
		else{
			assert(CheckOutputNum[set_index]->at(i) == uncheckWire->final_output[j]->idd);
			unckeckOutput_id->push_back(CheckOutputNum[set_index]->at(i));
			i++;j++;
		}
	} 
	if(check)
		cout<<"start Sim"<<endl;
	//use simulation to prove outputs equavilence
	vector<Gate*>one,two;
	bool result;
	for(int i=0;i<unckeckOutput_id->size();i++){
		DFSearch(circuitOne->output[unckeckOutput_id->at(i)],one);
		DFSearch(circuitTwo->output[unckeckOutput_id->at(i)],two);
	} 
	resetTraversed(one);
	resetTraversed(two);

	result=SimCheck(true,one,two);
	for(int i=0;i<cutSet[set_index]->size();i++)
		cutSet[set_index]->at(i)->CutReset();
	cutSet[set_index]->pop_back();

	if(result){	
		delete CheckOutputNum[set_index]; 
		CheckOutputNum[set_index]=unckeckOutput_id;
		if(check){
			cout<<"true"<<endl;
			cout<<"Sim end"<<endl;
		}			
	}
	else{
		if(check){
			cout<<"false"<<endl;
			cout<<"Sim end"<<endl;
		}				
	}

	return result;
	//use SAT to prove outputs equavilence
	/*vector<Gate*> one,two;	
	bool result;
	for(int i=0;i<unckeckOutput_id->size();i++){
		assert(one.size()==0 && two.size()==0);
		solver.initialize();
		DFSearch(circuitOne->output[unckeckOutput_id->at(i)],one);
		DFSearch(circuitTwo->output[unckeckOutput_id->at(i)],two);	
		genProofModel(false,one,two,check?3:0);
		resetTraversed(one);
		resetTraversed(two);
		one.clear();
		two.clear();
		Var fin=solver.newVar();
		if(circuitOne -> output[unckeckOutput_id->at(i)] -> curSim == circuitTwo -> output[unckeckOutput_id->at(i)] -> curSim)
			solver.addXorCNF(fin,circuitOne->output[unckeckOutput_id->at(i)]->getVar(),false,circuitTwo->output[unckeckOutput_id->at(i)]->getVar(),false);
		else
			solver.addXorCNF(fin,circuitOne->output[unckeckOutput_id->at(i)]->getVar(),true,circuitTwo->output[unckeckOutput_id->at(i)]->getVar(),false);
		solver.assumeProperty(fin,true);
		solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
  		solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
		solver.assumeProperty(circuitTwo -> constTrueGate -> getVar(), true);
  		solver.assumeProperty(circuitTwo -> constFalseGate -> getVar(), false);
		
		if(solver.assumpSolve()){
			cutSet[set_index]->pop_back();
			uncheckWire->CutReset();
			if(cutSet[set_index]->size()==1){
				assert(cutSet[set_index]->at(0)->isCut());
				cutSet[set_index]->at(0)->CutReset();
			}
			if(check)
				cout<<"false"<<endl;
			return false;
		}
	}
	if(check)
		cout<<"SAT end"<<endl;
	delete CheckOutputNum[set_index]; 
	CheckOutputNum[set_index]=unckeckOutput_id;
	if(check)
		cout<<"true"<<endl;	
	return true;*/
}


bool CircuitCmp::CheckReachability(){
  for(int i=0; i < circuitOne -> output.size(); ++i){
    if(!CheckReach(circuitOne -> output[i])) return false;
    if(!CheckReach(circuitTwo -> output[i])) return false;
  }
  return true;
}

bool CircuitCmp::CheckReach(Gate* source){
  for(int i=0; i < source -> input.size(); ++i){
    if(source -> input[i] -> gateType == Wir && ((Wire*)source -> input[i]) -> potentialCut == true)
      ((Wire*)source -> input[i]) -> reachable = true;
    else if(source -> input[i] -> gateType == Input)
      return false;
    else if(source -> input[i] -> gateType == Wir && CurCutLevel >= ((Wire*)source -> input[i]) -> cutLevel)
      return false;
    else if(!CheckReach(source -> input[i])) return false;
  }
  return true;
}

bool CircuitCmp::proveDontCareSim(Wire* target){
	RandGenerator rand;
	int tmp,output_id;
	vector<Gate*> dfslist;
	vector<int>output_idd;
	assert(!(target->isCut()));
	target->Cut();
	for(int i=0;i<target->final_output.size();i++){
		output_id=target->final_output[i]->idd;
		output_idd.push_back(output_id);
		DFSearch(circuitOne->output[output_id],dfslist);
		DFSearch(circuitTwo->output[output_id],dfslist);
	}	
	circuitOne->resetTraversed();
	circuitTwo->resetTraversed();
	for(int i=0;i<1000;i++){
		SimInitialize();
		target->curSim=rand.getRand();
		for(int j=0;j<dfslist.size();j++)
			dfslist[j]->operate();
		for(int j=0;j<output_idd.size();j++)
			if(circuitOne->output[output_idd[j]]->curSim != circuitTwo->output[output_idd[j]]->curSim){
				target->CutReset();
				return false;
			}
	}
	target->CutReset();
	return true;
}



void CircuitCmp::Strash(){
  int count = 0;
  HashMap<GateKey, Gate*> hash(circuitOne -> gate.size() + circuitTwo -> gate.size());
  for(int i=0; i < circuitOne -> gate.size(); ++i){
    hash.insert(GateKey(circuitOne -> gate[i]), circuitOne -> gate[i]);
  }
  for(int i=0; i < circuitTwo -> gate.size(); ++i){
    Gate* tmp = hash.insert(GateKey(circuitTwo -> gate[i]), circuitTwo -> gate[i]);
    if(tmp != NULL){
      if(HashKeyCmp(tmp, circuitTwo -> gate[i])){
        if(CheckGoodCut(circuitTwo -> gate[i], tmp)){
          ++count;
          Cut(tmp, circuitTwo -> gate[i]);
        }
      }
    }
  }
  cout << "Hash size: " << hash.size() << endl;
  cout << "Cut number: " << count << endl;

  cout << "Cut set num: " << cutSet.size() << endl;

  vector<Gate*> dfslistOne,dfslistTwo;
  for(int i=0; i < circuitOne -> output.size(); ++i){
    DFSearch(circuitOne -> output[i],dfslistOne);
    DFSearch(circuitTwo -> output[i],dfslistTwo);
  }
  if(SimCheck(1, dfslistOne,dfslistTwo)) cout << "still equivalent" << endl;
  else assert(0);
}

void CircuitCmp::cutSetClear(int begin,int end){
	assert(end < cutSet.size());
	for(int i=end;i>=begin;i--){
		assert(cutSet.size()==CheckOutputNum.size());
		for(int j=0;j<cutSet[i]->size();j++)
			cutSet[i]->at(j)->CutReset();
		delete cutSet[i];
		delete CheckOutputNum[i];
		vector<vector<Wire*>*>::iterator it=cutSet.begin()+i;
		cutSet.erase(it);
		vector<vector<int>*>::iterator he=CheckOutputNum.begin()+i;
		CheckOutputNum.erase(he);
	}
}
