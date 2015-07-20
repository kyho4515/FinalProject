#include "CircuitCmp.h"
#define MASK 0x1

bool CircuitCmp::SimCheck(bool mode,vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
  RandGenerator rand;
  int count = 0;
  while(true){
    for(int i=0; i < circuitOne -> input.size(); ++i){
      int tmp = rand.getRand();
      circuitOne -> input[i] -> curSim = tmp;
      circuitTwo -> input[i] -> curSim = tmp;
   	 }
    circuitOne -> constTrueGate -> curSim = ~0;
    circuitOne -> constFalseGate -> curSim = 0;
    circuitTwo -> constTrueGate -> curSim = ~0;
    circuitTwo -> constFalseGate -> curSim = 0;
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
      for(int i=0; i < circuitOne -> output.size(); ++i){
        if(circuitOne -> output[i] -> curSim != circuitTwo -> output[i] -> curSim && circuitOne -> output[i] -> curSim != ~(circuitTwo -> output[i] -> curSim)){
          return false;
        }
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
    if(32*count >= 10*(circuitOne -> input.size())){
      break;
    }
  }
  return true;
}

void CircuitCmp::SimFEC(vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
  for(int i=0; i < _FECpair.size(); ++i){
    for(int j=0; j < _FECpair[i] -> size(); ++j)
      _FECpair[i]->at(j)->potentialCut = false;
    delete _FECpair[i];
  }
  _FECpair.clear();
  RandGenerator rand;

  for(int i=0; i < circuitOne -> input.size(); ++i){
    int tmp = rand.getRand();
    circuitOne -> input[i] -> curSim = tmp;
    circuitTwo -> input[i] -> curSim = tmp;
  }
  for(int i=0; i < cutSet.size(); ++i){
    int tmp = rand.getRand();
    for(int j=0; j < cutSet[i] -> size(); ++j){
      if(cutSet[i] -> at(j) -> phase)
        cutSet[i] -> at(j) -> curSim = tmp;
      else
        cutSet[i] -> at(j) -> curSim = ~tmp;
    }
  }
  circuitOne -> constTrueGate -> curSim = ~0;
  circuitOne -> constFalseGate -> curSim = 0;
  circuitTwo -> constTrueGate -> curSim = ~0;
  circuitTwo -> constFalseGate -> curSim = 0;
  HashMap<IntKey, Wire*> hash(circuitOne -> wire.size()+circuitTwo -> wire.size());

  for(int i=0; i < _dfsListOne.size(); ++i){
    _dfsListOne[i] -> operate();
    if(_dfsListOne[i] -> gateType == Wir && !(((Wire*)_dfsListOne[i]) -> isCut())){
      Wire* tmp;
      if((_dfsListOne[i] -> curSim & MASK) == 0)
        tmp = hash.insert(IntKey(~(_dfsListOne[i] -> curSim)), (Wire*)_dfsListOne[i]);
      else
        tmp = hash.insert(IntKey(_dfsListOne[i] -> curSim), (Wire*)_dfsListOne[i]);
      if(tmp != NULL){
        if(tmp -> traversed){
          ((Wire*)_dfsListOne[i]) -> potentialCut = true;
          _FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListOne[i]);
        }
        else{
          tmp -> traversed = true;
          tmp -> FECNum = _FECpair.size();
          ((Wire*)_dfsListOne[i]) -> FECNum = _FECpair.size();
          _FECpair.push_back(new vector<Wire*>);
          ((Wire*)tmp) -> potentialCut = true;
          ((Wire*)_dfsListOne[i]) -> potentialCut = true;
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
      if((_dfsListTwo[i] -> curSim & MASK) == 0)
        tmp = hash.insert(IntKey(~(_dfsListTwo[i] -> curSim)), (Wire*)_dfsListTwo[i]);
      else 
        tmp = hash.insert(IntKey(_dfsListTwo[i] -> curSim), (Wire*)_dfsListTwo[i]);
      if(tmp != NULL){
        if(tmp -> traversed){
          ((Wire*)_dfsListTwo[i]) -> potentialCut = true;
          _FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListTwo[i]);
        }
        else{
          tmp -> traversed = true;
          tmp -> FECNum = _FECpair.size();
          ((Wire*)_dfsListTwo[i]) -> FECNum = _FECpair.size();
          _FECpair.push_back(new vector<Wire*>);
          ((Wire*)tmp) -> potentialCut = true;
          ((Wire*)_dfsListTwo[i]) -> potentialCut = true;
          _FECpair[tmp -> FECNum] -> push_back(tmp);
          _FECpair[tmp -> FECNum] -> push_back((Wire*)_dfsListTwo[i]);
        }
      }
    }
  }
  cout << "HASH Size:" << hash.size() << endl;

  //Check gate level
  cout << _FECpair.size() << endl;
  for(int i=0; i < _FECpair.size(); ++i){
    for(int j=0; j < _FECpair[i] -> size(); ++j){
      if(_FECpair[i] -> at(j) -> gateLevel != CurGateLevel){
        vector<Wire*>::iterator it = _FECpair[i] -> begin()+j;
        (*it) -> potentialCut = false;
        _FECpair[i] -> erase(it);
        --j;
      }
    }
    if(_FECpair[i] -> size() < 2){
      if(_FECpair[i] -> size() == 1)
        _FECpair[i] -> at(0) -> potentialCut = false;
      delete _FECpair[i];
      vector<vector<Wire*>*>::iterator it = _FECpair.begin()+i;
      _FECpair.erase(it);
      --i;
    }
  }
  cout << "After gateLevel: " << _FECpair.size() << endl;
  circuitOne -> resetTraversed();
  circuitTwo -> resetTraversed();
  
  SimFilter(1000, _dfsListOne, _dfsListTwo);
  
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
  
}

void CircuitCmp::SimFilter(int Count, vector<Gate*>& _dfsListOne, vector<Gate*>& _dfsListTwo){
  for(int i=0; i < Count; ++i){
    RandGenerator rand;
    for(int i=0; i < circuitOne -> input.size(); ++i){
      int tmp = rand.getRand();
      circuitOne -> input[i] -> curSim = tmp;
      circuitTwo -> input[i] -> curSim = tmp;
    }
    circuitOne -> constTrueGate -> curSim = ~0;
    circuitOne -> constFalseGate -> curSim = 0;
    circuitTwo -> constTrueGate -> curSim = ~0;
    circuitTwo -> constFalseGate -> curSim = 0;
    for(int i=0; i < cutSet.size(); ++i){
      int tmp = rand.getRand();
      for(int j=0; j < cutSet[i] -> size(); ++j){
        if(cutSet[i] -> at(j) -> phase)
          cutSet[i] -> at(j) -> curSim = tmp;
        else
          cutSet[i] -> at(j) -> curSim = ~tmp;
      }
    }
    for(int i=0; i < _dfsListOne.size(); ++i)
      _dfsListOne[i] -> operate();
    
    for(int i=0; i < _dfsListTwo.size(); ++i)
      _dfsListTwo[i] -> operate();
    
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
            Pair[num] -> at(0) -> potentialCut = false;
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
              Pair[j] -> at(0) -> potentialCut = false;
              delete Pair[j];
            }
          }
        }
      }
      else{
        for(int i=0; i < Pair.size(); ++i)
          delete Pair[i];
      }
    }
  }
  cout << "After filtering: " << _FECpair.size() << endl;
}
