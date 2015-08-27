#include "CircuitCmp.h"

void CircuitCmp::upwardCut(vector<Wire*>* cut_set){
	vector<Wire*> upward_wire;
	//尋找在buf上面的wire	
	for(int i=0;i<cut_set->size();i++)
		for(int j=0;j<cut_set->at(i)->output.size();j++)
			if(cut_set->at(i)->output[j]->gateType==Buf && cut_set->at(i)->output[j]->output[0]->gateType==Wir)
				upward_wire.push_back(((Wire*)cut_set->at(i)->output[j]->output[0]));
	if(upward_wire.size()>1){
		int num=cutSet.size();
		cutSet.push_back(new vector<Wire*>);	
		CheckOutputNum.push_back(new vector<int>);
		CheckOutputNum[num]=CheckOutputNum[num-1];	
		for(int i=0;i<upward_wire.size();i++){
			cutSet[num]->push_back(upward_wire[i]);
			upward_wire[i]->Cut();
			upward_wire[i]->cutNum=num;
		}
		upwardCut(cutSet[num]);
	}
	else{
		cout<<"find upward wire end"<<endl;
	}
}

void CircuitCmp::findCutfromFECpair(vector<Wire*>* FECpair){
	int num=cutSet.size();	
	cutSet.push_back(new vector<Wire*>);
	//cout<<"put in cut"<<endl;
	for(int i=0;i<FECpair->size();i++){
		cutSet[num]->push_back(FECpair->at(i));
		FECpair->at(i)->Cut();
	}
	//cout<<"into while"<<endl;
	do{
		vector<Gate*>one,two;
		for(int i=0;i<circuitOne->output.size();i++){
			DFSearch(circuitOne->output[i],one);
			DFSearch(circuitTwo->output[i],two);
		}
		resetTraversed(one);
		resetTraversed(two);
		//cout<<"simcheck"<<endl;
		if(SimCheck(true,one, two)){
			//cout<<"true"<<endl;			
			break;
		}
		else{
			//cout<<"false"<<endl;
			//cout<<"step 1"<<endl;
			//step1:找出不相等的output
			map<int,Gate*> falseOutput;
			bool unfind;
			for(int i=0;i<circuitOne->output.size();i++)
				if(circuitOne->output[i]->curSim != circuitTwo->output[i]->curSim ||circuitOne->output[i]->curSim != ~(circuitTwo->output[i]->curSim))
					falseOutput.insert(pair<int,Gate*>(circuitOne->output[i]->idd,circuitOne->output[i]));
			//cout<<"step 2"<<endl;			
			//step2:將連接到不相等output的cut移除
			vector<Wire*>* tmp=new vector<Wire*>;
			for(int i=0;i<cutSet[num]->size();i++){
				unfind=true;
				for(int j=0;j<cutSet[num]->at(i)->final_output.size();j++){
					map<int,Gate*>::iterator it = falseOutput.find(cutSet[num]->at(i)->final_output[j]->idd);
					if(it != falseOutput.end()){
						unfind=false;
						break;
					}
				}
				if(unfind)
					tmp->push_back(cutSet[num]->at(i));				
				else
					cutSet[num]->at(i)->CutReset();
			}
			delete cutSet[num];
			cutSet[num]=tmp;
		}
	}while(cutSet[num]->size()>1);
	//cout<<"step 3"<<endl;
	//step3:如果只剩下1或0個wire，就把這個cut刪除
	//cout<<"size is "<<cutSet[num]->size()<<endl;
	if(cutSet[num]->size()<=1){
	//	cout<<"size is one"<<endl;
		if(cutSet[num]->size()==1){		
			cutSet[num]->at(0)->potentialCut=false;
			assert(cutSet[num]->at(0)->isCut());
			cutSet[num]->at(0)->CutReset();
		}
		delete cutSet[num];
		vector<vector<Wire*>*>::iterator it = cutSet.begin()+num;
      cutSet.erase(it);
	}
	else{
		CheckOutputNum.push_back(new vector<int>);
		//cout<<"size is more than one"<<endl;
		assert(cutSet[num]->size()>1);
		map<int, Gate*> remainOutput;		
		for(int i=0;i<cutSet[num]->size();i++){
			cutSet[num]->at(i)->CutReset();
			for(int j=0;j<cutSet[num]->at(i)->final_output.size();j++){
				map<int,Gate*>::iterator it=remainOutput.find(cutSet[num]->at(i)->final_output[j]->idd); 
				//cout<<cutSet[num]->at(i)->final_output[j]->idd<<endl;
				if(it == remainOutput.end()){
					remainOutput.insert(pair<int,Gate*>(cutSet[num]->at(i)->final_output[j]->idd,cutSet[num]->at(i)->final_output[j]));
					CheckOutputNum[num]->push_back(cutSet[num]->at(i)->final_output[j]->idd);
				}
			}
		}
	}
	//cout<<"end"<<endl;
}
