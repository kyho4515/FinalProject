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
    if(!Check(x, y)){
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
