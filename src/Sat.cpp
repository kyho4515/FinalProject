/*
	6/29修改項目
		void CircuitCmp::Sat()
		
*/
#include "CircuitCmp.h"

void CircuitCmp::Sat(int test){
	if(test>0)
		cout<<"start SAT..."<<endl;
	int start;
	bool result;
	vector<Wire*> cirOne,cirTwo; 
	int max;
	for(int FEC_num=0;FEC_num<_FECpair.size();FEC_num++){
		start=cutSet.size();		
		if(test>0){
			cout<<"_FECpair "<<FEC_num<<" size:"<<_FECpair[FEC_num]->size()<<endl;
			cout<<"start="<<start<<endl;	
		}
		//將在circuitOne的wire和在circuitTwo的wire分開
		assert(cirOne.size()==0 && cirTwo.size()==0);
		for(int i=0;i<_FECpair[FEC_num]->size();i++){
			if(_FECpair[FEC_num]->at(i)->circuitNum==1)
				cirOne.push_back(_FECpair[FEC_num]->at(i));
			else if(_FECpair[FEC_num]->at(i)->circuitNum==2){
				cirTwo.push_back(_FECpair[FEC_num]->at(i));
			}
			else 
				assert(0);
		}
		if(test>0)
			cout<<cirOne.size()<<":"<<cirTwo.size()<<endl;
		
		//如果有circuitOne的wire也有circuitTwo的wire
		if(cirOne.size()!=0 && cirTwo.size()!=0){
			findCutfromFECpair(_FECpair[FEC_num]);
			/*for(int i=0;i<_FECpair[FEC_num]->size();i++){
				result=false;
				for(int s=start;s<cutSet.size();s++){						
					result=Cut(s,_FECpair[FEC_num]->at(i));							
					if(result){
						cutSet[s]->push_back(_FECpair[FEC_num]->at(i));
						break;
					}
				}				
				if(!result){
					assert(!(_FECpair[FEC_num]->at(i)->isCut()));
					cutSet.push_back(new vector<Wire*>);
					CheckOutputNum.push_back(new vector<int>);
					assert(cutSet.size()==CheckOutputNum.size());
					cutSet[cutSet.size()-1]->push_back(_FECpair[FEC_num]->at(i));
					for(int j=0;j<_FECpair[FEC_num]->at(i)->final_output.size();j++){
						CheckOutputNum[CheckOutputNum.size()-1]->push_back(_FECpair[FEC_num]->at(i)->final_output[j]->idd);
					}
					cutSet[cutSet.size()-1]->at(0)->cutNum=cutSet.size()-1;
					assert(!(cutSet[cutSet.size()-1]->at(0)->isCut()));
				}			
			}*/
		}
		//如果只有circuitOne的wire或只有circuitTwo的wire
		else{
			for(int i=0;i<_FECpair[FEC_num]->size();i++){
				result=false;
				for(int s=start;s<cutSet.size();s++){						
					result=Cut(s,_FECpair[FEC_num]->at(i));							
					if(result){
						cutSet[s]->push_back(_FECpair[FEC_num]->at(i));
						break;
					}
				}				
				if(!result){
					assert(!(_FECpair[FEC_num]->at(i)->isCut()));
					cutSet.push_back(new vector<Wire*>);
					CheckOutputNum.push_back(new vector<int>);
					assert(cutSet.size()==CheckOutputNum.size());
					cutSet[cutSet.size()-1]->push_back(_FECpair[FEC_num]->at(i));
					for(int j=0;j<_FECpair[FEC_num]->at(i)->final_output.size();j++){
						CheckOutputNum[CheckOutputNum.size()-1]->push_back(_FECpair[FEC_num]->at(i)->final_output[j]->idd);
					}
					cutSet[cutSet.size()-1]->at(0)->cutNum=cutSet.size()-1;
					assert(!(cutSet[cutSet.size()-1]->at(0)->isCut()));
				}			
			}
		}
		cirOne.clear();
		cirTwo.clear();
		//保留最多cut的cutSet
		max=0;
		for(int i=start;i<cutSet.size();i++){
			if(cutSet[i]->size()>max)
				max=cutSet[i]->size();
		}
		cout<<max<<endl;
		if(max==1 && test>0)
			cout<<"all one"<<endl;
		bool find_onecutSet=false;
		for(int i=start;i<cutSet.size();i++){
			assert(cutSet[i]->size()>=0);		
			if(cutSet[i]->size()<max || max<2 || find_onecutSet){
				if(cutSet[i]->size()==1){
					for(int j=0;j<cutSet[i]->size();j++){			
						cutSet[i]->at(j)->potentialCut=false;
					}
				}
				delete cutSet[i];
				delete CheckOutputNum[i];
				vector<vector<Wire*>*>::iterator it = cutSet.begin()+i;
      		cutSet.erase(it);
				vector<vector<int>*>::iterator he = CheckOutputNum.begin()+i;
				CheckOutputNum.erase(he); 
				--i;
			}
			else if(cutSet[i]->size()==max){
				find_onecutSet=true;
				for(int j=0;j<cutSet[i]->size();j++)
					cutSet[i]->at(j)->Cut();
			}
			else
				assert(0);
		}
		assert(cutSet.size()-start<2);
		//確認cutNum和在cutSet的位置相同
		assert(cutSet.size()==CheckOutputNum.size());
		for(int i=start;i<cutSet.size();i++)
			for(int j=0;j<cutSet[i]->size();j++)
				cutSet[i]->at(j)->cutNum = i;
		if(cutSet.size()-start==1)		
			upwardCut(cutSet[cutSet.size()-1]);
		if(cutSet.size()-start>1 && test>0)
			cout<<"find upward Wire"<<endl;
		//use simulation to check outputs are equal
		vector<Gate*>one,two;
		for(int i=start;i<cutSet.size();i++){
			for(int j=0;j<CheckOutputNum[i]->size();j++){
				DFSearch(circuitOne->output[CheckOutputNum[i]->at(j)],one);
				DFSearch(circuitTwo->output[CheckOutputNum[i]->at(j)],two);
			}
		}
		if(cutSet.size()-start!=0 && !SimCheck(true,one,two)){
			//assert(0);
			if(test>0)
				cout<<"Wrong simulate"<<endl;
			if(cutSet.size()-start==0)
				assert(0);
			else{
				delete CheckOutputNum[start];
				for(int i=cutSet.size()-1;i>=start;i--){
					for(int j=0;j<cutSet[i]->size();j++){
						cutSet[i]->at(j)->CutReset();
					}
					assert(cutSet.size()==CheckOutputNum.size());
					delete cutSet[i];			
					vector<vector<Wire*>*>::iterator it = cutSet.begin()+i;
      			cutSet.erase(it);
					vector<vector<int>*>::iterator he = CheckOutputNum.begin()+i;
      			CheckOutputNum.erase(he);
				}
			}
		}
		circuitOne->resetTraversed();
		circuitTwo->resetTraversed();
		one.clear();
		two.clear();
		//use SAT to prove outputs are equal
		/*vector<Gate*> one,two;	
		for(int i=start;i<cutSet.size();i++){
			if(test>0)
				
			for(int k=0;k<CheckOutputNum[i]->size();k++){
				assert(one.size()==0 && two.size()==0);
				solver.initialize();
				DFSearch(circuitOne->output[CheckOutputNum[i]->at(k)],one);
				DFSearch(circuitTwo->output[CheckOutputNum[i]->at(k)],two);	
				genProofModel(false,one,two);
				resetTraversed(one);
				resetTraversed(two);
				one.clear();
				two.clear();
				Var fin=solver.newVar();
				if(circuitOne -> output[CheckOutputNum[i]->at(k)] -> curSim == circuitTwo -> output[CheckOutputNum[i]->at(k)] -> curSim)
					solver.addXorCNF(fin,circuitOne->output[CheckOutputNum[i]->at(k)]->getVar(),false,circuitTwo->output[CheckOutputNum[i]->at(k)]->getVar(),false);
				else
					solver.addXorCNF(fin,circuitOne->output[CheckOutputNum[i]->at(k)]->getVar(),true,circuitTwo->output[CheckOutputNum[i]->at(k)]->getVar(),false);
				solver.assumeProperty(fin,true);
				solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
  				solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
				solver.assumeProperty(circuitTwo -> constTrueGate -> getVar(), true);
  				solver.assumeProperty(circuitTwo -> constFalseGate -> getVar(), false);

				if(solver.assumpSolve()){
					assert(0);
				}
			}
		}*/
		if(test>0){
			cout<<"cutSet size:"<<cutSet.size()<<endl;
			cout<<"________________"<<endl;
		}
	}
	
	vector<Gate*> one,two;	
	for(int k=0;k<circuitOne->output.size();k++){l;
		assert(one.size()==0 && two.size()==0);
		solver.initialize();
		DFSearch(circuitOne->output[k],one);
		DFSearch(circuitTwo->output[k],two);
		resetTraversed(one);
		resetTraversed(two);
		genProofModel(false,one,two);		
		one.clear();
		two.clear();
		Var fin=solver.newVar();
		if(circuitOne -> output[k] -> curSim == circuitTwo -> output[k] -> curSim)
			solver.addXorCNF(fin,circuitOne->output[k]->getVar(),false,circuitTwo->output[k]->getVar(),false);
		else
			solver.addXorCNF(fin,circuitOne->output[k]->getVar(),true,circuitTwo->output[k]->getVar(),false);
		solver.assumeProperty(fin,true);
		solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
  		solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
		if(solver.assumpSolve()){
			assert(0);
		}
	}

	
/*
	for(int i=0; i < _FECpair.size();++i){
	   if(_FECpair[i] -> size() < 10){
      	for(int j=0; j < _FECpair[i] -> size(); ++j){
      		for(int k=_FECpair[i] -> size()-1; k > j; --k){	
      			if(proveSAT(_FECpair[i] -> at(j), _FECpair[i] -> at(k))){
      				if(Cut(_FECpair[i]->at(j)->input[0], _FECpair[i]->at(k)->input[0])){
      					vector<Wire*>::iterator it = _FECpair[i] -> begin()+k;
      					_FECpair[i] -> erase(it);
           					}
          				}
        			}
      			}
  		}
  	}
*/
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

bool CircuitCmp::proveSAT(Gate* one, Gate* two){
		vector<Gate*> dfsListOne, dfsListTwo;
		solver.initialize();
		if(((Wire*)one) -> circuitNum == ((Wire*)two) -> circuitNum){
      	DFSearch(one, dfsListOne);
      	DFSearch(two, dfsListOne);
			genProofModel(true,dfsListOne, dfsListTwo);		
		}
		else{
			DFSearch(one, dfsListOne);
      	DFSearch(two, dfsListTwo);
			genProofModel(false,dfsListOne, dfsListTwo);
		}
      	
		circuitOne -> resetTraversed();
      circuitTwo -> resetTraversed();

      Var v = solver.newVar();

	   if(one -> curSim == two -> curSim)
	      solver.addXorCNF(v, one  -> getVar(), false, two-> getVar(),false);
	   else if(one -> curSim == ~(two -> curSim))
	      solver.addXorCNF(v, one -> getVar(), true, two-> getVar(), false);
      else 
			assert(0);
   
      solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
      solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
      solver.assumeProperty(v, true);

	   bool result = solver.assumpSolve();
		//solver.printStats();

		return !result;
}



//if in the same circuit, mode = true
void CircuitCmp::genProofModel(bool mode,vector<Gate*>& dfslistOne, vector<Gate*>& dfslistTwo,int test){	
	Var T = solver.newVar();
  	Var F = solver.newVar();
	if(test>0)
		cout<<"Kk"<<endl;
	//in same circuit
	if(mode){
		assert(dfslistTwo.size()==0);
		if(test>0)
			cout<<dfslistOne.size()<<endl;
		resetTraversed(dfslistOne);
		for(int i=0;i<dfslistOne.size();i++){
			if(test>0 )
				cout<<"***no***"<<":"<<i<<":"<<dfslistOne[i]->name<<endl;
			if(dfslistOne[i]->gateType==Input || dfslistOne[i]->gateType==Output){
				Var v=solver.newVar();
				dfslistOne[i]->setVar(v);
			}
			else if(dfslistOne[i]->gateType==Wir && !(dfslistOne[i]->traversed)){
				if(((Wire*)dfslistOne[i])->isCut()){
					Var v =solver.newVar();
					int cutNum = ((Wire*)dfslistOne[i]) -> cutNum; 
      			for(int j=0; j < cutSet[cutNum] -> size(); ++j){
        				cutSet[cutNum] -> at(j) -> setVar(v);
						cutSet[cutNum] -> at(j) -> traversed=true;
					}
				}
				else{
					Var v=solver.newVar();
					dfslistOne[i]->setVar(v);
				}
			}	
			else if(dfslistOne[i]->gateType==Const){
				if(dfslistOne[i]->phase)
					dfslistOne[i]->setVar(T);
				else
					dfslistOne[i]->setVar(F);
				dfslistOne[i]->traversed=true;

			}	
		}
	}
	//in different circuit
	else{
		assert(dfslistOne.size()!=0 && dfslistTwo.size()!=0);
		resetTraversed(dfslistOne);
		resetTraversed(dfslistTwo);
		//for circuit one
		for(int i=0;i<dfslistOne.size();i++){
			if(test>0 )
				cout<<"***one***"<<":"<<i<<":"<<dfslistOne[i]->name<<endl;
			if(dfslistOne[i]->gateType==Wir && !(dfslistOne[i]->traversed)){
				if(((Wire*)dfslistOne[i])->isCut()){
					Var v =solver.newVar();
					int cutNum = ((Wire*)dfslistOne[i]) -> cutNum; 
					assert(cutSet[cutNum]->size()>1);
      			for(int j=0; j < cutSet[cutNum] -> size(); ++j){
        				cutSet[cutNum] -> at(j) -> setVar(v);
						cutSet[cutNum] -> at(j) -> traversed=true;
					}
				}
				else{
					Var v=solver.newVar();
					dfslistOne[i]->setVar(v);
				}
			}				
			else if(dfslistOne[i]->gateType==Input){
				Var v=solver.newVar();
				dfslistOne[i]->setVar(v);
				(circuitTwo->GetInput(dfslistOne[i]->idd))->setVar(v);
				(circuitTwo->GetInput(dfslistOne[i]->idd))->traversed=true;
			}
			else if(dfslistOne[i]->gateType==Const ){
				if(dfslistOne[i]->phase)
					dfslistOne[i]->setVar(T);
				else
					dfslistOne[i]->setVar(F);
				dfslistOne[i]->traversed=true;
			}	
			else if(dfslistOne[i]->gateType==Output){
				Var v=solver.newVar();
				dfslistOne[i]->setVar(v);
			}
		}
		//for circuit two
		for(int i=0;i<dfslistTwo.size();i++){
			if(test>0 )
				cout<<"***two***"<<":"<<i<<":"<<dfslistTwo[i]->name<<endl;
			if(dfslistTwo[i]->gateType==Input && !(dfslistTwo[i]->traversed)){
				Var v=solver.newVar();
				dfslistTwo[i]->setVar(v);
			}
			else if(dfslistTwo[i]->gateType==Wir && !(dfslistTwo[i]->traversed)){
				if(test>1 )
					cout<<"is wire"<<endl;
				if(((Wire*)dfslistTwo[i])->isCut()){
					if(test>2 )
						cout<<"is cut wire"<<endl;
					Var v =solver.newVar();
					int cutNum = ((Wire*)dfslistTwo[i]) -> cutNum; 
					if(test>2 )
						cout<<"cutNum="<<cutNum<<endl;
					assert(cutSet[cutNum]->size()>1);
      			for(int j=0; j < cutSet[cutNum] -> size(); ++j){
        				cutSet[cutNum] -> at(j) -> setVar(v);
						cutSet[cutNum] -> at(j) -> traversed=true;
					}
				}
				else{
					if(test>2 )
						cout<<"is uncut wire"<<endl;
					Var v=solver.newVar();
					dfslistTwo[i]->setVar(v);
				}
			}
			else if (dfslistTwo[i]->gateType==Const){
				if(dfslistTwo[i]->phase)
					dfslistTwo[i]->setVar(T);
				else
					dfslistTwo[i]->setVar(F);
			}
			else if(dfslistTwo[i]->gateType==Output){
				Var v=solver.newVar();
				dfslistTwo[i]->setVar(v);
			}
			if(test>3)
				cout<<"***two***"<<":"<<i<<":"<<dfslistTwo[i]->name<<"...gen over"<<endl;
		}
	}
	
/*
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
    	else if((dfslistOne[i] -> gateType == Wir && !(((Wire*)dfslistOne[i])->isCut())) || dfslistOne[i] -> gateType == Output){
      	Var v = solver.newVar();
      	dfslistOne[i] -> setVar(v);
         	}
  	}

  	for(int i=0; i < dfslistTwo.size(); ++i){
    	if((dfslistTwo[i] -> gateType == Wir && !(((Wire*)dfslistTwo[i])->isCut()))|| dfslistTwo[i] -> gateType == Output){
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
  	//if(equivalence){
    	outputXor.clear();
    	for(int i=0; i < CheckOutputNum.size(); ++i){
      	outputXor.push_back(solver.newVar());
      	Gate* one = circuitOne -> output[CheckOutputNum[i]] ;
      	Gate* two = circuitTwo -> output[CheckOutputNum[i]] ;
      	if(circuitOne -> output[CheckOutputNum[i]] -> curSim == circuitTwo -> output[CheckOutputNum[i]] -> curSim)
        		solver.addXorCNF(outputXor[i], one -> getVar(), false, two -> getVar(), false);
      	else{
				if(circuitOne->output[i] ->curSim != ~(circuitTwo ->output[i]->curSim))
					cout<<i<<endl;
        		assert(circuitOne->output[i] ->curSim == ~(circuitTwo ->output[i]->curSim));
        		solver.addXorCNF(outputXor[i], one -> getVar(), true, two -> getVar(), false);
      			}
    		}
  	//}*/
	circuitOne->resetTraversed();
	circuitTwo->resetTraversed();
  	for(int i=0; i < dfslistOne.size();++i){
		if(test>3)
			cout<<"operate one:"<<dfslistOne[i]->name<<endl;
    	dfslistOne[i] -> constructSat(solver, T);
	}
	if(test>3)
			cout<<"operate one over"<<endl;
  	for(int i=0; i < dfslistTwo.size();++i){
		if(test>3)
			cout<<"operate two:"<<dfslistTwo[i]->name<<endl;
    	dfslistTwo[i] -> constructSat(solver, T);
	}
		if(test>3)
			cout<<"operate two over"<<endl;
}


