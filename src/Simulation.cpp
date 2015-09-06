#include "CircuitCmp.h"

#define MASK 0x1


bool CircuitCmp::SimCheck(bool mode,vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo,int time){
  	int count = 0;
	vector<int> unchecked_output_index;
	map<int,Gate*>unchecked_output_index_map;
	//尋找待確認的output的id
	for(int i=0;i<_dfsListOne.size();i++){
		if(_dfsListOne[i]->gateType==Output){
			map<int,Gate*>::iterator it=unchecked_output_index_map.find(_dfsListOne[i]->idd);
			if(it==unchecked_output_index_map.end()){
				unchecked_output_index_map.insert(pair<int,Gate*>(_dfsListOne[i]->idd,_dfsListOne[i]));
				unchecked_output_index.push_back(_dfsListOne[i]->idd);
			}
		}
	}
	for(int i=0;i<_dfsListTwo.size();i++){
		if(_dfsListTwo[i]->gateType==Output){
			map<int,Gate*>::iterator it=unchecked_output_index_map.find(_dfsListTwo[i]->idd);
			if(it==unchecked_output_index_map.end()){
				unchecked_output_index_map.insert(pair<int,Gate*>(_dfsListTwo[i]->idd,_dfsListTwo[i]));
				unchecked_output_index.push_back(_dfsListTwo[i]->idd);
			}
		}
	}
	//cout<<"output size:"<<unchecked_output_index.size()<<endl;
  	while(true){
    	SimInitialize();
		//operate
   	for(int i=0; i < _dfsListOne.size(); i++)
      	_dfsListOne[i] -> operate();
   	for(int i=0; i < _dfsListTwo.size(); i++)
      	_dfsListTwo[i] -> operate();
    
   	for(int i=0; i < unchecked_output_index.size(); ++i){
			int index=unchecked_output_index[i];
			int cirOneSim=circuitOne -> output[index] -> curSim ;
			int cirTwoSim=circuitTwo -> output[index] -> curSim ;
      	if(cirOneSim != cirTwoSim && cirOneSim != ~cirTwoSim)
          	return false;
      		}
   	++count;
   	if(32*count >= time*input_size){
      	break;
    		}
  	}
  	return true;
}

bool CircuitCmp::SimFEC(vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
	for(int i=0; i < _FECpair.size(); ++i)
    	delete _FECpair[i];
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
					_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListOne[i]);
					((Wire*)_dfsListOne[i])->mark=true;
				}
				else{
      			tmp -> traversed = true;
      	   	tmp -> FECNum = _FECpair.size();
					tmp -> mark = true;
      	 	 	((Wire*)_dfsListOne[i]) -> FECNum = _FECpair.size();
					((Wire*)_dfsListOne[i]) -> mark = true;
      		   _FECpair.push_back(new vector<Wire*>);
      	   	_FECpair[tmp -> FECNum] -> push_back(tmp);
      	   	_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListOne[i]);
				}
				
			}
		}	
	}
	
	for(int i=0; i < _dfsListTwo.size(); ++i){
		_dfsListTwo[i] -> operate();
		if(_dfsListTwo[i] -> gateType == Wir && !(((Wire*)_dfsListTwo[i]) -> isCut()) && ((Wire*)_dfsListTwo[i])->potentialCut){
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
					_FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListTwo[i]);
					((Wire*)_dfsListTwo[i]) -> mark = true;
				 }
				else{
					tmp -> traversed = true;
					tmp -> FECNum = _FECpair.size();
					tmp -> mark = true;
					((Wire*)_dfsListTwo[i]) -> FECNum = _FECpair.size();
					((Wire*)_dfsListTwo[i]) -> mark = true;
					_FECpair.push_back(new vector<Wire*>);
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
  
   SimFilter(100, _dfsListOne, _dfsListTwo);
	if(_FECpair.size()==0)
		return false;	

	SimLeveling();

	//assert(0);
	return true;
 
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
	vector<Gate*> list;
	vector<vector<Wire*>*> Pair;
	int start;
	for(int c=0; c < Count; ++c){
		if(c%100==0){
			list.clear();
			for(int FEC_num=0;FEC_num<_FECpair.size();FEC_num++)
				for(int FEC_index=0;FEC_index<_FECpair[FEC_num]->size();FEC_index++)
					DFSearch(_FECpair[FEC_num]->at(FEC_index),list);
			resetTraversed(list);
		}
		//initialize		
		SimInitialize();

		//operate
		for(int j=0;j<list.size();j++)
			list[j]->operate();

      for(int i=0; i < _FECpair.size(); ++i){
      	map<int, Wire*> FECmap;
			start=Pair.size();
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
			delete _FECpair[i];
			//remove Pair whose size is one
			for(int j=start;j<Pair.size();j++){
				if(Pair[j]->size()==1){
					Pair[j]->at(0)->potentialCut = false;
					potentialCutFalseWire.push_back(Pair[j]->at(0));
					upwardSetFalse(Pair[j]->at(0));
					Pair[j]->at(0)->mark=false;
					delete Pair[j];
					vector<vector<Wire*>*>::iterator it=Pair.begin()+j;
					Pair.erase(it);
					j--;
				}
				else if(Pair[j]->size()==0){
					cout<<"size can't be zero"<<endl;
					assert(0);
				}
			}
		}
		
		_FECpair.clear();
		for(int j=0;j<Pair.size();j++)
			_FECpair.push_back(Pair[j]);	
		Pair.clear();
  	}
   cout << "After filtering: " << _FECpair.size() << endl;
	int kk=0;
	for(int j=0;j<_FECpair.size();j++)
		kk+=_FECpair[j]->size();
	cout<<"uncheck wire : "<<kk<<endl;
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
			potentialCutFalseWire.push_back(pair[i]->at(0));
			upwardSetFalse(pair[i]->at(0));
		}
	cout<<"After SimSAT:"<<_FECpair.size()<<endl;
	int kk=0;
	for(int j=0;j<_FECpair.size();j++)
		kk+=_FECpair[j]->size();
	cout<<"uncheck wire : "<<kk<<endl;
}


void CircuitCmp::SimLeveling(){
	cout << "Leveling... " << endl;
	vector<Gate*> store;
	vector<vector<Wire*>*> Pair;
	bool check;
	for(int FEC_num=0;FEC_num < _FECpair.size();FEC_num++){	
		vector<Wire*>* tmp=new vector<Wire*>;
		///先依照gateLevel做sort，小的在前面
		sort(_FECpair[FEC_num]->begin(),_FECpair[FEC_num]->end(),sortcompare);
		///取得在最下層的Wire
		for(int i=0;i<_FECpair[FEC_num]->size();i++){
			assert(store.size()==0);
			DFSearch(_FECpair[FEC_num]->at(i),store);
			resetTraversed(store);
			check=true;
			for(int j=0;j<store.size()-1;j++)
				if(store[j]->gateType==Wir && ((Wire*)store[j])->mark){
					check=false;
					break;
				}
			if(check)
				tmp->push_back(_FECpair[FEC_num]->at(i));
			else
				_FECpair[FEC_num]->at(i)->mark=false;

			store.clear();
		}
		delete _FECpair[FEC_num];
		/***清除只剩一條wire的tmp***/
		assert(tmp->size()>=0);
		if(tmp->size() < 2){
			if(tmp->size()==1){
				tmp->at(0)->potentialCut = false;
				potentialCutFalseWire.push_back(tmp->at(0));
				tmp->at(0)->mark=false;
				upwardSetFalse(tmp->at(0));					
			}
			delete tmp;
		}
		else{
			assert(tmp->size()>1);
			Pair.push_back(tmp);
		}	
	}
	//把Pair至換到—FECpair
	_FECpair.clear();
	for(int i=0;i<Pair.size();i++)
		_FECpair.push_back(Pair[i]);
	Pair.clear();

	//output message
	cout << "After Leveling: " << _FECpair.size() << endl;
	int kk=0;
	for(int j=0;j<_FECpair.size();j++)
		kk+=_FECpair[j]->size();
	cout<<"uncheck wire : "<<kk<<endl;
	//reset mark	
	for(int i=0;i<circuitOne -> dfsList.size();i++)
		if(circuitOne -> dfsList[i]->gateType==Wir)
			((Wire*)circuitOne -> dfsList[i])->mark=false;
	
	for(int i=0;i<circuitTwo -> dfsList.size();i++)
		if(circuitTwo -> dfsList[i]->gateType==Wir)
			((Wire*)circuitTwo -> dfsList[i])->mark=false;

	for(int i=0;i<_FECpair.size();i++)
		for(int j=0;j<_FECpair[i]->size();j++)
			_FECpair[i]->at(j)->FECNum=i;
	
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
   for(int i=0; i < input_size; ++i){
     int tmp = rand.getRand();
     circuitOne -> input[i] -> curSim = tmp;
     circuitTwo -> input[i] -> curSim = tmp;
      	}
	//cut給值
  	for(int i=0; i < cutSet.size(); ++i){
    	int tmp = rand.getRand();
    	for(int j=0; j < cutSet[i] -> size(); ++j){
			assert(cutSet[i] -> at(j)->isCut());
    	  	if(cutSet[i] -> at(j) -> phase)
     	    	cutSet[i] -> at(j) -> curSim = tmp;
     	  	else
          	cutSet[i] -> at(j) -> curSim = ~tmp;
       		}
  	}
	assert(circuitOne -> constTrueGate -> curSim==~0);
	assert(circuitOne -> constFalseGate -> curSim==0);
	assert(circuitTwo -> constTrueGate -> curSim==~0);
	assert(circuitTwo -> constFalseGate -> curSim==0);
}

bool CircuitCmp::SimCheckOutput(vector<int> *number,int time){
	vector<Gate*> one,two;
	for(int i=0;i<number->size();i++){
		int index=number->at(i);
		DFSearch(circuitOne->output[index],one);
		DFSearch(circuitTwo->output[index],two);
	}
	resetTraversed(one);
	resetTraversed(two);
	return SimCheck(true,one,two,time);
}
