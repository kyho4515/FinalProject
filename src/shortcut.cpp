#include "CircuitCmp.h"

void CircuitCmp::upwardCut(vector<Wire*>* cut_set){
	vector<Wire*> upward_wire;
	//尋找在buf上面的wire	
	for(int i=0;i<cut_set->size();i++){	
		if(cut_set->at(i)->output.size()==1){
			Gate* output=cut_set->at(i)->output[0],*output_output=cut_set->at(i)->output[0]->output[0];
			if(output->gateType==Buf && output_output->gateType==Wir)
				upward_wire.push_back(((Wire*)output_output));
		}
	}
	if(upward_wire.size()>1){
		int num=cutSet.size();
		cutSet.push_back(new vector<Wire*>);	
		CheckOutputNum.push_back(new vector<int>);
		for(int i=0;i<CheckOutputNum[num-1]->size();i++)
			CheckOutputNum[num]->push_back(CheckOutputNum[num-1]->at(i));	
		for(int i=0;i<upward_wire.size();i++){
			cutSet[num]->push_back(upward_wire[i]);
			upward_wire[i]->Cut();
			upward_wire[i]->cutNum=num;
		}
		upwardCut(cutSet[num]);
	}
	else{
		//cout<<"find upward wire end"<<endl;
	}
}

void CircuitCmp::findCutfromFECpair(vector<Wire*>* FECpair){
	int num=cutSet.size();	
	cutSet.push_back(new vector<Wire*>);
	map<int,Gate*> output_index_map;	
	vector<int>*output_index=new vector<int>;
	for(int i=0;i<FECpair->size();i++){
		//put wires into cutSet		
		cutSet[num]->push_back(FECpair->at(i));
		FECpair->at(i)->Cut();
		FECpair->at(i)->cutNum=num;

		//find outputs related to those wire
		for(int j=0;j<FECpair->at(i)->final_output.size();j++){
			map<int,Gate*>::iterator it=output_index_map.find(FECpair->at(i)->final_output[j]->idd);
			//find an output not be found			
			if(it == output_index_map.end()){;
				output_index_map.insert(pair<int,Gate*>(FECpair->at(i)->final_output[j]->idd,FECpair->at(i)->final_output[j]));
				output_index->push_back(FECpair->at(i)->final_output[j]->idd);
			} 
		}
	}
	do{
		if(SimCheckOutput(output_index,1)){		
			break;
		}
		else{
			//step1:找出不相等的output
			map<int,Gate*> falseOutput;
			bool unfind;
			vector<int>*new_output_index=new vector<int>;
			for(int i=0;i<output_index->size();i++)
				if(circuitOne->output[output_index->at(i)]->curSim != circuitTwo->output[output_index->at(i)]->curSim && circuitOne->output[output_index->at(i)]->curSim != ~(circuitTwo->output[output_index->at(i)]->curSim))
					falseOutput.insert(pair<int,Gate*>(circuitOne->output[output_index->at(i)]->idd,circuitOne->output[output_index->at(i)]));		
				else
					new_output_index->push_back(output_index->at(i));
						
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
				else{
					cutSet[num]->at(i)->CutReset();
					cutSet[num]->at(i)->potentialCut=false;
					potentialCutFalseWire.push_back(cutSet[num]->at(i));
					upwardSetFalse(cutSet[num]->at(i));
				}
			}
			delete cutSet[num];
			delete output_index;
			cutSet[num]=tmp;
			output_index=new_output_index;
		}
	}while(cutSet[num]->size()>1);
	//step3:如果只剩下1或0個wire，就把這個cut刪除
	if(cutSet[num]->size()<=1){
		if(cutSet[num]->size()==1){		
			cutSet[num]->at(0)->potentialCut=false;
			potentialCutFalseWire.push_back(cutSet[num]->at(0));
			assert(cutSet[num]->at(0)->isCut());
			cutSet[num]->at(0)->CutReset();
		}
		delete cutSet[num];
		vector<vector<Wire*>*>::iterator it = cutSet.begin()+num;
      cutSet.erase(it);
		for(int i=0;i<FECpair->size();i++)
			assert(!(FECpair->at(i)->isCut()));
	}
	else{
		CheckOutputNum.push_back(output_index);
		assert(cutSet[num]->size()>1);
	}
}

void CircuitCmp::upwardSetFalse(Wire* source){
	if(source->output.size()==1){
		Gate* output=source->output[0];
		Gate* output_=output->output[0];
		if(output->gateType==Buf && output_->gateType==Wir){
			((Wire*)output_)->potentialCut=false;
			potentialCutFalseWire.push_back((Wire*)output_);
			upwardSetFalse((Wire*)output_);
		}
	}
}

void CircuitCmp::FindWrongCut(){
	int num=cutSet.size()-1;//紀錄目前最後面的cutSet_index
	vector<Wire*> tmp;//暫時儲存被移除的cut裡面的wire
	bool check;
	//從最後面的cut開始移除
	for(int i=num;i>=0;i--){
		assert(tmp.size()==0);
		assert(cutSet.size()==cut_potentialCutFalseWire_index.size());
		for(int j=0;j<cutSet[cutSet.size()-1]->size();j++)
			tmp.push_back(cutSet[cutSet.size()-1]->at(j));
		cutSetClear(cutSet.size()-1,cutSet.size()-1);
		check=(SimCheckOutput(all_output_index,5) && SATCheckOutput(all_output_index));
		if(check){
			potentialCutReset(cut_potentialCutFalseWire_index[cut_potentialCutFalseWire_index.size()-1]);
			for(int k=0;k<tmp.size();k++){
				tmp[k]->potentialCut=false;
				potentialCutFalseWire.push_back(tmp[k]);	
			}
			cut_potentialCutFalseWire_index.pop_back();
			break;
		}
		cut_potentialCutFalseWire_index.pop_back();
		tmp.clear();
	}
	assert(check);
	fout<<cutSet.size()<<endl;
	if(cutSet.size()>=maxcutSet_size)
		maxcutSet_size=cutSet.size();
	else;
		//assert(0);
}
