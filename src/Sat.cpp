/*
	6/29修改項目
		void CircuitCmp::Sat()
		
*/
#include "CircuitCmp.h"

void CircuitCmp::Sat(){
 /* 
  vector<Wire*> cutWire;
  cout << "Current gate level: " << CurGateLevel << endl
       << "Current cut level: " << CurCutLevel << endl;
  bool flag = true;
  if(CheckReachability()){
    for(int i=0; i < _FECpair.size();++i){
      for(int j=0; j < _FECpair[i] -> size(); ++j){
        if(_FECpair[i] -> at(j) -> reachable == true && !(_FECpair[i]->at(j)->isCut())){
          _FECpair[i]->at(j)->reachable = false;
          for(int k=j+1; k < _FECpair[i] -> size(); ++k){
            flag = false;
            if(_FECpair[i] -> at(k) -> reachable == true && !(_FECpair[i]->at(k)->isCut())){
              _FECpair[i]->at(k)->reachable = false;
              if(proveSAT(_FECpair[i] -> at(j), _FECpair[i] -> at(k)) && Cut(_FECpair[i]->at(j)->input[0], _FECpair[i]->at(k)->input[0])){
                cutWire.push_back(_FECpair[i]-> at(j));
                cutWire.push_back(_FECpair[i]-> at(k));
                flag = true;
                ++j;
                break;
              }
              else{
                if(CheckReach(_FECpair[i] -> at(j)) && CheckReach(_FECpair[i]->at(k))){
                  i = 0;
                  j = 0;
                  break;
                }
                else{
                  flag = false;
                  break;
                }
              }
            }
          }
          if(!flag){
            bool isContinue;
            for(int k=j+1; k < _FECpair[i]->size(); ++k){
              isContinue = false;
              if(proveSAT(_FECpair[i]->at(j), _FECpair[i]->at(k))){
                isContinue = true;
                if(_FECpair[i]->at(k)->gateLevel > CurGateLevel){
                  if(CheckReach(_FECpair[i]->at(j))){
                    i=0;
                    j=0;
                    flag = true;
                    break;
                  }
                  else {
                    flag = false;
                    break;
                  }
                }
                else{
                  _FECpair[i]->at(j)->reachable = true;
                  flag = true;
                  break;
                }
              }
            }
            if(!isContinue){
              if(CheckReach(_FECpair[i]->at(j))){
                i=0;
                j=0;
              }
              else break;
            }
          }
        }
      }
      if(!flag) break;
    }
  }
  if(!flag || !Check()){
    for(int i=0; i < cutWire.size(); ++i){
      if(cutSet[cutWire[i]->cutNum] -> size() == 1){
        cutSet[cutWire[i]->cutNum]->pop_back();
      }
      else{
        vector<Wire*>::iterator it = cutSet[cutWire[i]->cutNum]->begin();
        for(; it != cutSet[cutWire[i]->cutNum]->end(); ++it){
          if((*it) == cutWire[i]){
            cutSet[cutWire[i]->cutNum]->erase(it);
            break;
          }
        }
      }
      cutWire[i]->CutReset();
    }
    for(int i=0; i < cutSet.size(); ++i){
      if(cutSet[i] -> size() == 0){
        delete cutSet[i];
        vector<vector<Wire*>*>::iterator it = cutSet.begin()+i;
        cutSet.erase(it);
        --i;
      }
      else 
        assert(cutSet[i] -> size() != 1) ;
    }
    dfsListOne.clear();
    dfsListTwo.clear();
    for(int i=0; i < circuitOne -> output.size(); ++i){
      DFSearch(circuitOne->output[i],dfsListOne);
      DFSearch(circuitTwo->output[i],dfsListTwo);
    }
    circuitOne -> resetTraversed();
    circuitTwo -> resetTraversed();
    SimCheck(1,dfsListOne, dfsListTwo);
    solver.initialize();
    genProofModel(dfsListOne, dfsListTwo);
  }
  else if(cutWire.size() != 0){
    ++CurCutLevel;
    for(int i=0; i < cutWire.size(); ++i)
      cutWire[i]->cutLevel = CurCutLevel;
  }*/
	cout<<"start SAT..."<<endl;
  CurGateLevel +=2;
 
  for(int i=0; i < _FECpair.size();++i){
//    if(_FECpair[i] -> size() < 10){
		cout<<"i="<<i<<endl;
      for(int j=0; j < _FECpair[i] -> size(); ++j){
        for(int k=_FECpair[i] -> size()-1; k > j; --k){	
          if(proveSAT(_FECpair[i] -> at(j), _FECpair[i] -> at(k))){
            if(Cut(_FECpair[i]->at(j)->input[0], _FECpair[i]->at(k)->input[0])){
///////////////////////////////////6/29
					/*cout<<_FECpair[i]->at(j)->name<<":"<<_FECpair[i]->at(k)->name<<endl;
					if(_FECpair[i]->at(j)->total_cut_name.size()==0 || _FECpair[i]->at(j)->total_cut_name[0]!=_FECpair[i]->at(j)->cutNum){
						_FECpair[i]->at(j)->total_input_name.clear();
						_FECpair[i]->at(j)->total_cut_name.clear();
						_FECpair[i]->at(j)->total_cut_name.push_back(_FECpair[i]->at(j)->cutNum);
						ModifyInput(_FECpair[i]->at(j));
					}
					if(_FECpair[i]->at(k)->total_cut_name.size()==0 ||	_FECpair[i]->at(k)->total_cut_name[0]!=_FECpair[i]->at(k)->cutNum){
						_FECpair[i]->at(k)->total_input_name.clear();
						_FECpair[i]->at(k)->total_cut_name.clear();
						_FECpair[i]->at(k)->total_cut_name.push_back(_FECpair[i]->at(k)->cutNum);										
						ModifyInput(_FECpair[i]->at(k));
					}					
					circuitOne->resetTraversed();
					circuitTwo->resetTraversed();*/
					
////////////////////////////////////////////////////////////////
              vector<Wire*>::iterator it = _FECpair[i] -> begin()+k;
              _FECpair[i] -> erase(it);
      //        cout << "SAT" << endl;
            }
          }
        }
      }
//    }
  }
  
  cout << "total cut: " << cutSet.size() << endl;
  int count=0;
  for(int i=0; i < circuitOne -> wire.size(); ++i){
    if(circuitOne -> wire[i] -> isCut()) ++count;
  }
  for(int i=0; i < circuitTwo -> wire.size(); ++i){
    if(circuitTwo -> wire[i] -> isCut()) ++count;
  }
  cout << "cutting wire: " << count << endl;
}

bool CircuitCmp::proveSAT(Wire* one, Wire* two){
  if(one -> circuitNum != two -> circuitNum){
   if(CheckGoodCut(one, two)){
      
      vector<Gate*> dfsListOne, dfsListTwo;
      DFSearch(one -> input[0], dfsListOne);
      DFSearch(two -> input[0], dfsListTwo);
      circuitOne -> resetTraversed();
      circuitTwo -> resetTraversed();
      genProofModel(circuitOne -> dfsList, circuitTwo -> dfsList);
      Var v = solver.newVar();
      if(one -> curSim == two -> curSim)
        solver.addXorCNF(v, one -> input[0] -> getVar(), (one->input[0]->phase) ? true:false, two -> input[0] -> getVar(), (two->input[0]->phase) ? true:false);
      else if(one -> curSim == ~(two -> curSim)){
        solver.addXorCNF(v, one -> input[0] -> getVar(), (one->input[0]->phase) ? false:true, two -> input[0] -> getVar(), (two->input[0]->phase) ? true:false);
      }
      else return false;
      solver.assumeRelease();
      for(int i=0; i < circuitOne -> input.size(); ++i){
        if(!(circuitOne -> input[i] -> included))
          solver.assumeProperty(circuitOne -> input[i] -> getVar(), false);
      }
      solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
      solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
      solver.assumeProperty(v, true);
      bool result = solver.assumpSolve();
 //solver.printStats();
      if(!result) return true;
      else return false;
   }
    else return false;
  }
  else return false;
}

void CircuitCmp::genProofModel(vector<Gate*>& dfslistOne, vector<Gate*>& dfslistTwo){
  for(int i=0; i < circuitOne -> input.size(); ++i){
    Var v = solver.newVar();
    circuitOne -> input[i] -> setVar(v);
    circuitTwo -> input[i] -> setVar(v);
  }
  for(int i=0; i < dfslistOne.size(); ++i){
    if(dfslistOne[i] -> gateType == Wir && ((Wire*)dfslistOne[i]) -> isCut()){
      Var v = solver.newVar();
      int cutNum = ((Wire*)dfslistOne[i]) -> cutNum; 
      for(int j=0; j < cutSet[cutNum] -> size(); ++j)
        cutSet[cutNum] -> at(j) -> setVar(v);
         }
    else if(dfslistOne[i] -> gateType != Wir && dfslistOne[i] -> gateType != Input && dfslistOne[i] -> gateType != Output){
      Var v = solver.newVar();
      dfslistOne[i] -> setVar(v);
         }
  }
  for(int i=0; i < dfslistTwo.size(); ++i){
    if(dfslistTwo[i] -> gateType != Input && dfslistTwo[i] -> gateType != Wir && dfslistTwo[i] -> gateType != Output){
      Var v = solver.newVar();
      dfslistTwo[i] -> setVar(v);
    }
  }
  Var T = solver.newVar();
  Var F = solver.newVar();
  circuitOne -> constTrueGate -> setVar(T);
  circuitOne -> constFalseGate -> setVar(F);
  circuitTwo -> constTrueGate -> setVar(T);
  circuitTwo -> constFalseGate -> setVar(F);
  if(equivalence){
    outputXor.clear();
    for(int i=0; i < CheckOutputNum.size(); ++i){
      outputXor.push_back(solver.newVar());
      Gate* one = circuitOne -> output[CheckOutputNum[i]] -> input[0];
      Gate* two = circuitTwo -> output[CheckOutputNum[i]] -> input[0];
      if(circuitOne -> output[CheckOutputNum[i]] -> curSim == circuitTwo -> output[CheckOutputNum[i]] -> curSim)
        solver.addXorCNF(outputXor[i], one -> getVar(), (one->phase) ? true:false, two -> getVar(), (two->phase) ? true:false);
      else{
        assert(circuitOne->output[i] ->curSim == ~(circuitTwo ->output[i]->curSim));
        solver.addXorCNF(outputXor[i], one -> getVar(), (one->phase) ? false:true, two -> getVar(), (two->phase) ? true:false);
      }
    }
  }
  
  
  for(int i=0; i < dfslistOne.size();++i)
    dfslistOne[i] -> constructSat(solver, T);
  for(int i=0; i < dfslistTwo.size();++i)
    dfslistTwo[i] -> constructSat(solver, T);
  solver.assumeRelease();
}
