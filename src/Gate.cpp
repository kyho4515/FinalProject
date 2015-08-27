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
	assert(input.size()>1);
	Var fanin1,fanin2;
	fanin1=input[0]->getVar();
	for(int i=1;i<input.size();i++){
		Var tem=s.newVar();
		fanin2=input[i]->getVar();
		s.addAigCNF(tem, fanin1, false, fanin2, false);
		fanin1=tem;
	}
	s.addAigCNF(output[0]->getVar(), fanin1, !phase, Const, false);
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
	assert(input.size()>1);
	Var fan=s.newVar();
	s.addAigCNF(fan, input[0]->getVar(), true, input[1]->getVar(), true);
	for(int i=2;i<input.size();i++){
		Var tem=s.newVar();
		s.addAigCNF(tem, fan, false, input[i]->getVar(), true);
		fan=tem;
	}
	s.addAigCNF(output[0]->getVar(), fan, phase, Const, false);
}

void XorGate::operate(){
  	int tmp = input[0] -> curSim;
  	int tmp2 = tmp;
  	for(int i=1; i < input.size(); ++i){
    	tmp2 = (tmp & ~input[i] -> curSim) | (~tmp & input[i] -> curSim);
    	tmp = tmp2;
  	}

  	if(phase == false)
    	curSim = ~tmp;
  	else curSim = tmp;
}

void XorGate::constructSat(SatSolver& s, Var& Const){
	assert(input.size()>1);
	Var fanin1,fanin2;
	fanin1=input[0]->getVar();
	for(int i=1;i<input.size();i++){
		Var tem=s.newVar();
		fanin2=input[i]->getVar();
		s.addXorCNF(tem, fanin1, false, fanin2, false);
		fanin1=tem;
	}
	s.addAigCNF(output[0]->getVar(), fanin1, !phase, Const, false);	
}

void BufGate::operate(){
  	if(phase == false)
    	curSim = ~(input[0] -> curSim);
  	else 
		curSim = input[0] -> curSim;
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
