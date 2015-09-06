/*
	6/29修改項目
		void CircuitCmp::Sat()
		
*/
#include "CircuitCmp.h"

void CircuitCmp::Sat(int test){
	if(test>0)
		cout<<"start SAT..."<<endl;
	int start,ini_start=cutSet.size();
	bool result;
	vector<Wire*> cirOne,cirTwo; 
	vector<int> cut_FECnum;//儲存有進入cutSet的FEC_num
	//每次呼叫Sat都先紀錄cutSet供偵錯使用
	if(start_record.size()==0 ||start_record[start_record.size()-1]!=ini_start)
		start_record.push_back(ini_start);
		
	assert(cut_FECnum.size()==0);
	for(int FEC_num=0;FEC_num<_FECpair.size();FEC_num++){
		start=cutSet.size();		
		if(test>0){
			cout<<"_FECpair "<<FEC_num<<" size:"<<_FECpair[FEC_num]->size()<<endl;
			cout<<"start="<<start<<endl;	
			cout<<cutSet.size()<<":"<<cut_potentialCutFalseWire_index.size()<<endl;
		}
		//將在circuitOne的wire和在circuitTwo的wire分開
		assert(cirOne.size()==0 && cirTwo.size()==0);
		for(int i=0;i<_FECpair[FEC_num]->size();i++){
			if(_FECpair[FEC_num]->at(i)->circuitNum==1)
				cirOne.push_back(_FECpair[FEC_num]->at(i));
			else if(_FECpair[FEC_num]->at(i)->circuitNum==2)
				cirTwo.push_back(_FECpair[FEC_num]->at(i));
			else{
				cout<<_FECpair[FEC_num]->at(i)->circuitNum<<endl;
				assert(0);
			}
		}
		if(test>0)
			cout<<"在circuitOne的wire數："<<cirOne.size()<<endl<<"在circuitTwo的wire數："<<cirTwo.size()<<endl;
		cirOne.clear();
		cirTwo.clear();
		//

		findCutfromFECpair(_FECpair[FEC_num]);

		if(test>0){
			cout<<"多出的cutset:"<<cutSet.size()-start<<endl;
			cout<<"多出的cut Wire:"<<((cutSet.size()-start>0)?cutSet[cutSet.size()-1]->size():0)<<endl;
			assert(cutSet[cutSet.size()-1]->size()>1);
		}

		//如果cutSet增加的話再檢查
		assert(cutSet.size()-start<2);
		if(cutSet.size()-start==1){
			//use simulation to check outputs are equal			
			if(!SimCheckOutput(all_output_index,3)){
				cout<<"Wrong simulate"<<endl;
				cutSetClear(cutSet.size()-1,cutSet.size()-1);
				if(SimCheckOutput(all_output_index,5)){
					for(int i=0;i<_FECpair[FEC_num]->size();i++){
						_FECpair[FEC_num]->at(i)->potentialCut=false;
						potentialCutFalseWire.push_back(_FECpair[FEC_num]->at(i));
					}
					assert(cutSet.size()==cut_potentialCutFalseWire_index.size());
				}
				else{
					cout<<"wrong choices lead to wrong end."<<endl;
					//如果在這組之前都沒有cut，代表前面的_FECpair有錯誤
					if(ini_start==cutSet.size()){
						break;
					}
					//若前面還有cut就檢查前面的cut有沒有切錯
					else{
						bool check;
						for(int i=cut_FECnum.size()-1;i >= 0;i--){
							assert(cutSet[cutSet.size()-1]->at(0)->FECNum == cut_FECnum[i]);
							cutSetClear(cutSet.size()-1,cutSet.size()-1);
							check=SimCheckOutput(all_output_index,5);
							//拿掉某個cut之後電路相等->將這cut對應到的_FECpair設為potentialCut false,並跳到此FECpair的下一個繼續
							if(check){
								potentialCutReset(cut_potentialCutFalseWire_index[cut_potentialCutFalseWire_index.size()-1]);
								for(int j=0;j<_FECpair[cut_FECnum[i]]->size();j++){
									_FECpair[cut_FECnum[i]]->at(j)->potentialCut=false;
									potentialCutFalseWire.push_back(_FECpair[cut_FECnum[i]]->at(j));	
								}
								assert(i==cut_FECnum.size()-1);
								FEC_num=cut_FECnum[i];
								cut_FECnum.pop_back();
								cut_potentialCutFalseWire_index.pop_back();
								break;
							}
							//如果拿掉後依然不相等，就檢查下一個
							else{
								cut_FECnum.pop_back();
								cut_potentialCutFalseWire_index.pop_back();
							}							
						}
						assert(cutSet.size()==cut_potentialCutFalseWire_index.size());
						//如果check是false代表前面的_FECpair有錯
						if(!check){
							break;
						}
					}
				}
			}
			else{
				cut_FECnum.push_back(FEC_num);
				cut_potentialCutFalseWire_index.push_back(potentialCutFalseWire.size());
			}
		}
		if(test>0){
			cout<<"cutSet size:"<<cutSet.size()<<endl;
			cout<<"________________"<<endl;
		}
	}

	///用simulation檢查circuit是否相等
	//如果發現不相等
	if(!SimCheckOutput(all_output_index,10)){
		cout<<"final check false"<<endl;
		FindWrongCut();
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

bool CircuitCmp::proveSAT(Gate* one, Gate* two){
		vector<Gate*> dlistOne, dlistTwo;
		DFSearch(one, dlistOne);
      DFSearch(two, dlistTwo);
		resetTraversed(dlistOne);
		resetTraversed(dlistTwo);		

		solver.initialize();		
		genProofModel(dlistOne, dlistTwo);	
      Var v = solver.newVar();

	   if(one -> phase == two -> phase)
	      solver.addXorCNF(v, one  -> getVar(), false, two-> getVar(),false);
	   else if(one -> phase == !(two -> phase))
	      solver.addXorCNF(v, one -> getVar(), true, two-> getVar(), false);
   
      solver.assumeProperty(circuitOne -> constTrueGate -> getVar(), true);
      solver.assumeProperty(circuitOne -> constFalseGate -> getVar(), false);
      solver.assumeProperty(v, true);

	   bool result = solver.assumpSolve();
		//solver.printStats();

		return !result;
}

void CircuitCmp::genProofModel(vector<Gate*>& dfslistOne, vector<Gate*>& dfslistTwo,int test){	
	//input set var
	for(int i=0; i < input_size; ++i){
    	Var v = solver.newVar();
    	circuitOne -> input[i] -> setVar(v);
    	circuitTwo -> input[i] -> setVar(v);
  	}
	//cutSet set var
	for(int i=0;i<cutSet.size();i++){
		Var T = solver.newVar();
		Var F = solver.newVar(); 
		for(int j=0;j<cutSet[i]->size();j++){
			if(cutSet[i]->at(j)->phase)
				cutSet[i]->at(j)->setVar(T);
			else
				cutSet[i]->at(j)->setVar(F);
			assert(cutSet[i]->at(j)->isCut());
		}
		solver.addAigCNF(F,T,true,T,true);
	}
	//output set var
	for(int i=0;i<output_size;i++){
		Var v = solver.newVar();
    	circuitOne -> output[i] -> setVar(v);
    	circuitTwo -> output[i] -> setVar(v);
	}
	//wire set var
	for(int i=0;i<dfslistOne.size();i++){
		if(dfslistOne[i]->gateType==Wir && !(((Wire*)dfslistOne[i])->isCut())){
			Var v = solver.newVar();
			dfslistOne[i]->setVar(v);
		}
	}
	for(int i=0;i<dfslistTwo.size();i++){
		if(dfslistTwo[i]->gateType==Wir && !(((Wire*)dfslistTwo[i])->isCut())){
			Var v = solver.newVar();
			dfslistTwo[i]->setVar(v);
		}
	}
 	//const set var
  	Var T = solver.newVar();
  	Var F = solver.newVar();
  	circuitOne -> constTrueGate -> setVar(T);
  	circuitOne -> constFalseGate -> setVar(F);
  	circuitTwo -> constTrueGate -> setVar(T);
  	circuitTwo -> constFalseGate -> setVar(F);
	//constructSAT
  	for(int i=0; i < dfslistOne.size();++i)
    	dfslistOne[i] -> constructSat(solver, T);
  	for(int i=0; i < dfslistTwo.size();++i)	
    	dfslistTwo[i] -> constructSat(solver, T);
}

bool CircuitCmp::SATCheckOutput(vector<int>* number,bool op){
	for(int i=0;i<number->size();i++){
		if(!proveSAT(circuitOne->output[number->at(i)],circuitTwo->output[number->at(i)])){
			if(op)
				cout<<"output "<<number->at(i)<<" check:false"<<endl;
			return false;
		}
		else{
			if(op)
				cout<<"output "<<number->at(i)<<" check:true"<<endl;
		}
	}
	
	return true;
}

