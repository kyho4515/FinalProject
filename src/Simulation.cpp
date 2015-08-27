#include "CircuitCmp.h"

#define MASK 0x1


bool CircuitCmp::SimCheck(bool mode,vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
  	RandGenerator rand;
  	int count = 0;
	vector<int> unchecked_output_index;
	map<int,Gate*> input_size;
	//尋找待確認的output的id和找到input的個數
	for(int i=0;i<_dfsListOne.size();i++){
		if(_dfsListOne[i]->gateType==Output)
			unchecked_output_index.push_back(_dfsListOne[i]->idd);
		else if(_dfsListOne[i]->gateType==Input){
			map<int,Gate*>::iterator it=input_size.find(_dfsListOne[i]->idd);
			if(it==input_size.end())
				input_size.insert(pair<int,Gate*>(_dfsListOne[i]->idd,_dfsListOne[i]));
		}
	}
	for(int i=0;i<_dfsListTwo.size();i++){
		if(_dfsListTwo[i]->gateType==Output)
			unchecked_output_index.push_back(_dfsListTwo[i]->idd);
		else if(_dfsListTwo[i]->gateType==Input){
			map<int,Gate*>::iterator it=input_size.find(_dfsListTwo[i]->idd);  
			if(it==input_size.end())
				input_size.insert(pair<int,Gate*>(_dfsListTwo[i]->idd,_dfsListTwo[i]));
		}
	}
  	while(true){
    	for(int i=0; i < circuitOne -> input.size(); ++i){
      	int tmp = rand.getRand();
      	circuitOne -> input[i] -> curSim = tmp;
      	circuitTwo -> input[i] -> curSim = tmp;
   	 	}
		assert(circuitOne -> constTrueGate -> curSim==~0);
		assert(circuitOne -> constFalseGate -> curSim==0);
		assert(circuitTwo -> constTrueGate -> curSim==~0);
		assert(circuitTwo -> constFalseGate -> curSim==0);

    	if(mode){
      	for(int i=0; i < cutSet.size(); ++i){
      		int tmp = rand.getRand();
        		for(int j=0; j < cutSet[i] -> size(); ++j){
          		if(cutSet[i] -> at(j) -> phase)
            		cutSet[i] -> at(j) -> curSim = tmp;
          		else
            		cutSet[i] -> at(j) -> curSim = ~tmp;
        			}
      			}
    		}
    
   	for(int i=0; i < _dfsListOne.size(); ++i)
      	_dfsListOne[i] -> operate();
    
   	for(int i=0; i < _dfsListTwo.size(); ++i)
      	_dfsListTwo[i] -> operate();
    
		if(mode){
   		for(int i=0; i < unchecked_output_index.size(); ++i){
      		if(circuitOne -> output[unchecked_output_index[i]] -> curSim != circuitTwo -> output[unchecked_output_index[i]] -> curSim && circuitOne -> output[unchecked_output_index[i]] -> curSim != ~(circuitTwo -> output[unchecked_output_index[i]] -> curSim))
          		return false;
      			}
    		}
   	else{
    		if(_dfsListOne[_dfsListOne.size()-1] -> phase == _dfsListTwo[_dfsListTwo.size()-1] -> phase){
        		if(_dfsListOne[_dfsListOne.size()-1] -> curSim != _dfsListTwo[_dfsListTwo.size()-1] -> curSim)
          		return false;
      			}
      	else{
        		if(_dfsListOne[_dfsListOne.size()-1] -> curSim != ~(_dfsListTwo[_dfsListTwo.size()-1] -> curSim))
          		return false;
      			}
    		}
   	++count;
   	if(32*count >= 10*(input_size.size())){
      	break;
    		}
  	}
  	return true;
}

void CircuitCmp::SimFEC(vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
	for(int i=0; i < _FECpair.size(); ++i){
		/*for(int j=0; j < _FECpair[i] -> size(); ++j)
      	_FECpair[i]->at(j)->potentialCut = false;*/
    	delete _FECpair[i];
  	}
   _FECpair.clear();

   SimInitialize();

	assert(circuitOne -> constTrueGate -> curSim==~0);
	assert(circuitOne -> constFalseGate -> curSim==0);
	assert(circuitTwo -> constTrueGate -> curSim==~0);
	assert(circuitTwo -> constFalseGate -> curSim==0);
	
  	HashMap<IntKey, Wire*> hash(circuitOne -> wire.size()+circuitTwo -> wire.size());
	
	for(int i=0; i < _dfsListOne.size(); ++i){
		_dfsListOne[i] -> operate();
		if(_dfsListOne[i] -> gateType == Wir && !(((Wire*)_dfsListOne[i]) -> isCut()) && ((Wire*)_dfsListOne[i])->potentialCut){			
			Wire* tmp;
			if((_dfsListOne[i] -> curSim & MASK) == 0){
				_dfsListOne[i]->phase=false;
				tmp = hash.insert(IntKey(~(_dfsListOne[i] -> curSim)), (Wire*)_dfsListOne[i]);
			}
			else{
				_dfsListOne[i]->phase=true;
				tmp = hash.insert(IntKey(_dfsListOne[i] -> curSim), (Wire*)_dfsListOne[i]);
			}
			if(tmp != NULL){
				if(tmp -> traversed){				
					//((Wire*)_dfsListOne[i]) -> potentialCut = true;
					_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListOne[i]);
				}
				else{
      			tmp -> traversed = true;
      	   	tmp -> FECNum = _FECpair.size();
      	 	 	((Wire*)_dfsListOne[i]) -> FECNum = _FECpair.size();
      		   _FECpair.push_back(new vector<Wire*>);
      	   	//((Wire*)tmp) -> potentialCut = true;
      	 		//((Wire*)_dfsListOne[i]) -> potentialCut = true;
      	   	_FECpair[tmp -> FECNum] -> push_back(tmp);
      	   	_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListOne[i]);
				}
				
			}
		}	
	}
	
	for(int i=0; i < _dfsListTwo.size(); ++i){
		_dfsListTwo[i] -> operate();
		if(_dfsListTwo[i] -> gateType == Wir && !(((Wire*)_dfsListTwo[i]) -> isCut())){
			Wire* tmp;
			if((_dfsListTwo[i] -> curSim & MASK) == 0){
				tmp = hash.insert(IntKey(~(_dfsListTwo[i] -> curSim)), (Wire*)_dfsListTwo[i]);
				_dfsListTwo[i]->phase=false;
			}
			else{
				tmp = hash.insert(IntKey(_dfsListTwo[i] -> curSim), (Wire*)_dfsListTwo[i]);
				_dfsListTwo[i]->phase=true;
			}
			if(tmp != NULL){
				if(tmp -> traversed){
					//((Wire*)_dfsListTwo[i]) -> potentialCut = true;
					_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListTwo[i]);
				 }
				else{
					tmp -> traversed = true;
					tmp -> FECNum = _FECpair.size();
					((Wire*)_dfsListTwo[i]) -> FECNum = _FECpair.size();
					_FECpair.push_back(new vector<Wire*>);
					//((Wire*)tmp) -> potentialCut = true;
					//((Wire*)_dfsListTwo[i]) -> potentialCut = true;
					_FECpair[tmp -> FECNum] -> push_back(tmp);
					_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListTwo[i]);
				}
			}
		}
	}
	assert(circuitOne -> constTrueGate -> curSim==~0);
	assert(circuitOne -> constFalseGate -> curSim==0);
	assert(circuitTwo -> constTrueGate -> curSim==~0);
	assert(circuitTwo -> constFalseGate -> curSim==0);
	cout << "HASH Size:" << hash.size() << endl;
	cout << _FECpair.size() << endl;

   resetTraversed(_dfsListOne);
   resetTraversed(_dfsListTwo);
  
   SimFilter(500, _dfsListOne, _dfsListTwo);
	
	SimLeveling();
	
	SimSATcheck();
 /*
   for(int i=_FECpair.size()-1; i >= 0; --i){
   	vector<vector<Wire*>*> Pair;
   	for(int j=0; j < _FECpair[i] -> size(); ++j){
      	Pair.push_back(new vector<Wire*>);
      	Pair[Pair.size()-1]->push_back(_FECpair[i]->at(j));
      	for(int k=_FECpair[i]->size()-1; k > j; --k){
      		if(CheckGoodCut(_FECpair[i]->at(j),_FECpair[i]->at(k))){
          		Pair[Pair.size()-1]->push_back(_FECpair[i]->at(k));
          		vector<Wire*>::iterator it = _FECpair[i]->begin()+k;
          		_FECpair[i]->erase(it);
        			}
      			}
    		}
   	delete _FECpair[i];
    	int num = 0;
    	for(; num < Pair.size(); ++num){
      	if(Pair[num] -> size() > 1)break;
      	else {
        		Pair[num] -> at(0) -> potentialCut = false;
        		delete Pair[num];
      			}
    		}
    	if(num == Pair.size()){
      	vector<vector<Wire*>*>::iterator it = _FECpair.begin()+i;
      	_FECpair.erase(it);
    		}	 
    	else{
      	_FECpair[i] = Pair[num];
      	for(int k=0; k < Pair[num] -> size(); ++k)
        		Pair[num] -> at(k) -> FECNum = i;
      	for(int j=num+1; j < Pair.size(); ++j){
        		if(Pair[j] -> size() > 1){
          		for(int k=0; k < Pair[j] -> size(); ++k){
            		Pair[j] -> at(k) -> FECNum = _FECpair.size();
          				}
          		_FECpair.push_back(Pair[j]);
        			}
        		else{
         		Pair[j] -> at(0) -> potentialCut = false;
          		delete Pair[j];
        			}
      			}
    		}
  	}
*/
	/*vector<vector<Wire*>*> _Largepair;
	int count=0;
	for(int i=0;i<_FECpair.size();i++)
		count+=_FECpair[i]->size();
	for(int i=0;i<_FECpair.size();i++)
		if(_FECpair[i]->size() > 200){
			_Largepair.push_back(new vector<Wire*>);
			_Largepair[_Largepair.size()-1]=_FECpair[i];
			vector<vector<Wire*>*>::iterator it = _FECpair.begin()+i;
      	_FECpair.erase(it);
      	--i;
		}
	assert(0);	
	Ppair=new PotentialPair(_FECpair);
	Ppair->Out();
	cout<<"original FECpair size: "<<_FECpair.size()<<endl;	
	cout<<"original wire size: "<<count<<endl;
	cout<<"number of large pair : "<<_Largepair.size()<<endl;*/
/*
	for(int i=0;i<_FECpair.size();i++){
		cout<<i<<" FECpair"<<endl;
		for(int j=0;j<_FECpair[i]->size();j++){
			cout<<_FECpair[i]->at(j)->circuitNum<<" ";
		}
		cout<<endl;

	}
	assert(0);*/
}

void CircuitCmp::SimFilter(int Count, vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
	cout << "filtering..." << endl;
	for(int i=0; i < Count; ++i){
		//initialize		
		SimInitialize();
	
		assert(circuitOne -> constTrueGate -> curSim==~0);
		assert(circuitOne -> constFalseGate -> curSim==0);
		assert(circuitTwo -> constTrueGate -> curSim==~0);
		assert(circuitTwo -> constFalseGate -> curSim==0);
		////////

		//operate//
      for(int i=0; i < _dfsListOne.size(); ++i)
      	_dfsListOne[i] -> operate();
    
    	for(int i=0; i < _dfsListTwo.size(); ++i)
      	_dfsListTwo[i] -> operate();
    		//////

   	int size = _FECpair.size();
      for(int i=0; i < size; ++i){
      	map<int, Wire*> FECmap;
     		vector<vector<Wire*>*> Pair;
      	for(int j=0; j < _FECpair[i] -> size(); ++j){
        		int curSim;
        		if((_FECpair[i] -> at(j) -> curSim & MASK) == 0){
          		curSim = ~(_FECpair[i] -> at(j) -> curSim);
        			}
        		else {
          		curSim = _FECpair[i] -> at(j) -> curSim;
       				 }

        		map<int, Wire*>::iterator it = FECmap.find(curSim);
        		if(it == FECmap.end()){
          		FECmap.insert(pair<int,Wire*>(curSim, _FECpair[i] -> at(j)));
          		_FECpair[i] -> at(j) -> FECNum = Pair.size();
          		Pair.push_back(new vector<Wire*>);
         		Pair[Pair.size()-1] -> push_back(_FECpair[i] -> at(j));
       				 }
        		else{
          		Pair[it -> second -> FECNum] -> push_back(_FECpair[i] -> at(j));
        			}
      			}
      	if(Pair.size() != 1){
        		delete _FECpair[i];
        		int num = 0;
        		for(; num < Pair.size(); ++num){
          		if(Pair[num] -> size() > 1)break;
          		else {
            		//Pair[num] -> at(0) -> potentialCut = false;
            		delete Pair[num];
          				}
        			}
        		if(num == Pair.size()){
          		vector<vector<Wire*>*>::iterator it = _FECpair.begin()+i;
          		_FECpair.erase(it);
          		--i;
          		--size;
        			}
        		else{
         		_FECpair[i] = Pair[num];
          		for(int k=0; k < Pair[num] -> size(); ++k)
            		Pair[num] -> at(k) -> FECNum = i;
          		for(int j=num+1; j < Pair.size(); ++j){
            		if(Pair[j] -> size() > 1){
              			for(int k=0; k < Pair[j] -> size(); ++k){
                		Pair[j] -> at(k) -> FECNum = _FECpair.size();
             						 }
              			_FECpair.push_back(Pair[j]);
           					 }
            		else{
              			//Pair[j] -> at(0) -> potentialCut = false;
              			delete Pair[j];
            					}
          				}
        			}
      			}
      	else{
      	  	for(int i=0; i < Pair.size(); ++i){
					Pair[i]->at(0)->potentialCut = false;	  	
					delete Pair[i];
				}
      			}	
    		}
  	}
   cout << "After filtering: " << _FECpair.size() << endl;
}
void CircuitCmp::SimSATcheck(bool test){
	vector<vector<Wire*>*>pair;
	int start;
	bool result;
	for(int FEC_num=0;FEC_num < _FECpair.size();FEC_num++){
		start=pair.size();		
		for(int i=0;i<_FECpair[FEC_num]->size();i++){	
			result=false;
			if(test)
			 	cout<<"A"<<endl;
			for(int j=start;j<pair.size();j++){
				result=proveSAT(pair[j]->at(0),_FECpair[FEC_num]->at(i));
				if(result){
					pair[j]->push_back(_FECpair[FEC_num]->at(i));
					break;
				}		
			}
			if(!result){
				pair.push_back(new vector<Wire*>);
				pair[pair.size()-1]->push_back(_FECpair[FEC_num]->at(i));
			}
		}
		delete _FECpair[FEC_num];
	}
	_FECpair.clear();
	for(int i=0;i<pair.size();i++)
		if(pair[i]->size()>1){
			_FECpair.push_back(pair[i]);
		}		
		else{
			assert(pair[i]->size()==1);
			pair[i]->at(0)->potentialCut = false;
		}
	cout<<"After SimSAT:"<<_FECpair.size()<<endl;
}
void CircuitCmp::SimLeveling(){
	cout << "Leveling... " << endl;
	vector<Gate*> store;
	bool check;
	for(int FEC_num=0;FEC_num < _FECpair.size();FEC_num++){	
		vector<Wire*>* tmp=new vector<Wire*>;
		///先依照gateLevel做sort，小的在前面
		sort(_FECpair[FEC_num]->begin(),_FECpair[FEC_num]->end(),sortcompare);
		///確認任兩條wire不會有上下關係
		//cout<<_FECpair.size()-FEC_num-1<<endl;
		tmp->push_back(_FECpair[FEC_num]->at(0));
		for(int i=1;i<_FECpair[FEC_num]->size();i++){
			//cout<<store.size()<<":"<<tmp->size()<<endl;
			assert(store.size()==0 && tmp->size()>0);
			DFSearch(_FECpair[FEC_num]->at(i),store);
			check=true;
			for(int j=0;j<tmp->size();j++)
				if(tmp->at(j)->traversed){
					check=false;
					break;
				}
			if(check){
				tmp->push_back(_FECpair[FEC_num]->at(i));
			}
			resetTraversed(store);
			store.clear();
		}		
		delete _FECpair[FEC_num];
		_FECpair[FEC_num]=tmp;
		/***清除只剩一條wire的_FECpair***/
		assert(_FECpair[FEC_num]->size()>0);
		if(_FECpair[FEC_num]->size()==1){
			_FECpair[FEC_num]->at(0)->potentialCut = false;
      	_FECpair[FEC_num]->clear();
			delete _FECpair[FEC_num];
			vector<vector<Wire*>*>::iterator it = _FECpair.begin()+FEC_num;
      	_FECpair.erase(it);
			--FEC_num;
		}
		else{
			assert(_FECpair[FEC_num]->size()>1);
			for(int i=0;i < _FECpair[FEC_num]->size();i++)
				_FECpair[FEC_num]->at(i)->mark=true;
		}	
	}
	//assert(0);
	cout<<_FECpair.size()<<endl;
	for(int FEC_num=0;FEC_num < _FECpair.size();FEC_num++){	
		for(int i=0;i<_FECpair[FEC_num]->size();i++){
			assert(store.size()==0);
			DFSearch(_FECpair[FEC_num]->at(i),store);
			for(int j=0;j<store.size()-1;j++)
				if(store[j]->gateType==Wir && ((Wire*)store[j])->mark){
					//_FECpair[FEC_num]->at(i)->potentialCut = false;
					vector<Wire*>::iterator it = _FECpair[FEC_num]->begin()+i;
      			_FECpair[FEC_num]->erase(it);
					--i;
					break;
				}
			resetTraversed(store);
			store.clear();
		}
		if(_FECpair[FEC_num]->size()<=1){
			if(_FECpair[FEC_num]->size()==1){
				//_FECpair[FEC_num]->at(0)->potentialCut = false;
      		_FECpair[FEC_num]->clear();
			}
			delete _FECpair[FEC_num];
			vector<vector<Wire*>*>::iterator it = _FECpair.begin()+FEC_num;
      	_FECpair.erase(it);
			--FEC_num;
		}
	}
	cout << "After Leveling: " << _FECpair.size() << endl;
	//assert(0);
	//reset mark	
	for(int i=0;i<circuitOne -> dfsList.size();i++)
		if(circuitOne -> dfsList[i]->gateType==Wir)
			((Wire*)circuitOne -> dfsList[i])->mark=false;
	
	for(int i=0;i<circuitTwo -> dfsList.size();i++)
		if(circuitTwo -> dfsList[i]->gateType==Wir)
			((Wire*)circuitTwo -> dfsList[i])->mark=false;
}

bool CircuitCmp::SimCheckLevel(Wire* a,Wire* b){
	vector<Gate*> store;
	bool result=false;
	DFSearch((a->gateLevel > b->gateLevel? a:b),store);
	if(a->traversed && b->traversed)
		result=true;
	resetTraversed(store);

	return result;


}

void CircuitCmp::SimInitialize(){
	RandGenerator rand;
	//input給值
   for(int i=0; i < circuitOne -> input.size(); ++i){
     int tmp = rand.getRand();
     circuitOne -> input[i] -> curSim = tmp;
     circuitTwo -> input[i] -> curSim = tmp;
      	}
	//cut給值
  	for(int i=0; i < cutSet.size(); ++i){
    	int tmp = rand.getRand();
    	for(int j=0; j < cutSet[i] -> size(); ++j){
    	  if(cutSet[i] -> at(j) -> phase)
     	    cutSet[i] -> at(j) -> curSim = tmp;
     	  else
          cutSet[i] -> at(j) -> curSim = ~tmp;
       		}
  	}

}
