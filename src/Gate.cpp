#include "Gate.h"

void AndGate::operate(){
  int tmp = input[0] -> curSim;
  int tmp2 = tmp;
  for(int i=1; i < input.size(); ++i){
    tmp2 = tmp & input[i] -> curSim;
    tmp = tmp2;
  }
  if(phase == false)
    curSim = ~tmp;
  else curSim = tmp;
}

void AndGate::constructSat(SatSolver& s, Var& Const){
  Gate* Input1 = (input[0] -> gateType == Wir && !(((Wire*)input[0]) -> isCut())) ? input[0] -> input[0] : input[0];
  Gate* Input2 = (input[1] -> gateType == Wir && !(((Wire*)input[1]) -> isCut())) ? input[1] -> input[0] : input[1];

  if(input.size() > 2){
    Var v = s.newVar();
    s.addAigCNF(v, Input1 -> getVar(), !(Input1 -> phase), Input2 -> getVar(), !(Input2 -> phase));
    for(int i=2; i < input.size()-1; ++i){
      Var tmp = s.newVar();
      Input1 = (input[i] -> gateType == Wir && !(((Wire*)input[i]) -> isCut())) ? input[i] -> input[0] : input[i];
      s.addAigCNF(tmp, Input1 -> getVar(), !(Input1 -> phase), v, false);
      v = tmp;
    }
    Input1 = (input[input.size()-1] -> gateType == Wir && !(((Wire*)input[input.size()-1]) -> isCut())) ? input[input.size()-1] -> input[0] : input[input.size()-1];
    s.addAigCNF(getVar(), Input1 -> getVar(), !(Input1 -> phase), v, false);
  }
  else s.addAigCNF(getVar(), Input1->getVar(), !(Input1 -> phase), Input2->getVar(), !(Input2 -> phase));
}

void OrGate::operate(){
  int tmp = input[0] -> curSim;
  int tmp2 = tmp;
  for(int i=1; i < input.size(); ++i){
    tmp = tmp2 | input[i] -> curSim;
    tmp2 = tmp;
  }
  if(phase == false)
    curSim = ~tmp;
  else curSim = tmp;
}

void OrGate::constructSat(SatSolver& s, Var& Const){
  Var v = s.newVar();
  Gate* Input1 = (input[0] -> gateType == Wir && !(((Wire*)input[0]) -> isCut())) ? input[0] -> input[0] : input[0];
  Gate* Input2 = (input[1] -> gateType == Wir && !(((Wire*)input[1]) -> isCut())) ? input[1] -> input[0] : input[1];
  s.addAigCNF(v, Input1 -> getVar(), Input1 -> phase, Input2 -> getVar(), Input2 -> phase);
  for(int i=2; i < input.size()-1; ++i){
    Var tmp = s.newVar();
    Input1 = (input[i] -> gateType == Wir && !(((Wire*)input[i]) -> isCut())) ? input[i] -> input[0] : input[i];
    s.addAigCNF(tmp, Input1 -> getVar(), Input1 -> phase, v, false);
    v = tmp;
  }
  Var tmp = v;
  if(input.size() > 2){
    tmp = s.newVar();
    Input1 = (input[input.size()-1] -> gateType == Wir && !(((Wire*)input[input.size()-1]) -> isCut())) ? input[input.size()-1] -> input[0] : input[input.size()-1];
    s.addAigCNF(tmp, Input1 -> getVar(), Input1 -> phase, v, false);
  }
  s.addAigCNF(getVar(), tmp, true, Const, false);
}

void XorGate::operate(){
  int tmp = input[0] -> curSim;
  int tmp2 = tmp;
  for(int i=1; i < input.size(); ++i){
    tmp2 = (tmp & ~input[i] -> curSim) | (~tmp & input[i] -> curSim);
    tmp = tmp2;
  }
  if(phase == false)
    curSim = ~tmp2;
  else curSim = tmp2;
}

void XorGate::constructSat(SatSolver& s, Var& Const){
  Gate* Input1 = (input[0] -> gateType == Wir && !(((Wire*)input[0]) -> isCut())) ? input[0] -> input[0] : input[0];
  Gate* Input2 = (input[1] -> gateType == Wir && !(((Wire*)input[1]) -> isCut())) ? input[1] -> input[0] : input[1];
  if(input.size() > 2){
    Var v = s.newVar();
    s.addXorCNF(v, Input1 -> getVar(), !(Input1 -> phase), Input2 -> getVar(), !(Input2 -> phase));
    for(int i=2; i < input.size()-1; ++i){
      Var tmp = s.newVar();
      Input1 = (input[i] -> gateType == Wir && !(((Wire*)input[i]) -> isCut())) ? input[i] -> input[0] : input[i];
      s.addXorCNF(tmp, Input1 -> getVar(), !(Input1 -> phase), v, false);
      v = tmp;
    }
    Input1 = (input[input.size()-1] -> gateType == Wir && !(((Wire*)input[input.size()-1]) -> isCut())) ? input[input.size()-1] -> input[0] : input[input.size()-1];
    s.addXorCNF(getVar(), Input1 -> getVar(), !(Input1 -> phase), v, false);
  }
  else s.addXorCNF(getVar(), Input1->getVar(), !(Input1 -> phase), Input2->getVar(), !(Input2 -> phase));
}

void BufGate::operate(){
  if(phase == false)
    curSim = ~(input[0] -> curSim);
  else curSim = input[0] -> curSim;
}

void Wire::operate(){
  if(!isCut())
    curSim = input[0] -> curSim;
}

void OutputGate::operate(){
  curSim = input[0] -> curSim;
}

size_t GateKey::transform(string str){
  size_t count = 0;
  for(int i=0; i < str.size(); ++i)
    count += size_t(str[i]);
  return count;
}

GateKey::GateKey(Gate* gate){
  size_t tmp = 1;
  for(int i=0; i < gate -> input.size(); ++i)
    tmp *= transform(gate -> input[i] -> name);
  key = tmp * gate -> gateType;
}
