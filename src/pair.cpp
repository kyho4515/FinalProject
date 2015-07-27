#include "pair.h"

//input為_FECpair
PotentialPair::PotentialPair(vector<vector<Wire*>*> FEC){
	int start;	
	for(int FEC_num=0;FEC_num < FEC.size();FEC_num++){
		start=pair.size();//取起點		
		for(int element=0;element < FEC[FEC_num]->size();element++){
			if(!RepeatInPair(start,FEC[FEC_num]->at(element))){
				pair.push_back(new vector<Wire*>);
				commonIn.push_back(new vector<Gate*>);
				pair[pair.size()-1]->push_back(FEC[FEC_num]->at(element));
			}
		}
	}
}

//從pair中找到wire可能的位置
bool PotentialPair::RepeatInPair(int starting,Wire* unfind){
	bool repeat=false;
	for(int i=starting;i < pair.size();i++){
		assert(differentIn.size()==0);
		if(pair[i]->size()==1){
			assert(commonIn[i]->size() == 0);
			FindCommonInput(pair[i]->at(0),unfind,i);
			if(ProveFunction(pair[i]->at(0),unfind,i) && ProveDontCare(pair[i]->at(0),unfind)){
				pair[i]->push_back(unfind);
				repeat=true;
			}
			else
				commonIn[i]->clear();
		}
		else{
			assert(pair[i]->size() > 1);
			if(FindCommonInput(unfind,i) && ProveFunction(pair[i]->at(0),unfind,i) && ProveDontCare(pair[i]->at(0),unfind)){
				pair[i]->push_back(unfind);
				repeat=true;
			}
		}
		differentIn.clear();
		if(commonIn[i]->size()>1 && commonIn[i]->at(0)->name == commonIn[i]->at(1)->name){
			assert(commonIn[i]->size()%2 == 0);			
			vector<Gate*> tem;
			for(int j=0;j<commonIn[i]->size();j+=2){
				tem.push_back(commonIn[i]->at(j));
			}
			commonIn[i]->clear();
			for(int j=0;j < tem.size();j++)
				commonIn[i]->push_back(tem[j]);
		}
	}
	
	return	repeat;
}

//找相同的input，n是commonIn的位置，還有不同的input
void PotentialPair::FindCommonInput(Wire* a,Wire* b,int n){
	for(int i=0,j=0;i<a->input.size() || j<b->input.size();){
		if(j==b->input.size() || (a->input.size()!=0 && a->input[i]->name < b->input[j]->name)){
			differentIn.push_back(a->input[i]);
			i++;
		}
		else if(i==a->input.size() || (b->input.size()!=0 && a->input[i]->name > b->input[j]->name)){
			differentIn.push_back(b->input[j]);
			j++;
		}
		else{
			assert(a->input[i]->name==b->input[j]->name);
			commonIn[n]->push_back(a->input[i]);
			if(a->circuitNum!=b->circuitNum)
				commonIn[n]->push_back(b->input[j]);
			i++;j++;
		}
	}
}


bool PotentialPair::FindCommonInput(Wire* b,int n){
	vector<Gate*> tem;	
	int i,j;	
	for(i=0,j=0;i < commonIn[n]->size() && j < b->input.size();){
		if(commonIn[n]->at(i)->name < b->input[j]->name){
			differentIn.push_back(commonIn[n]->at(i));
			i++;
		}
		else if(commonIn[n]->at(i)->name > b->input[j]->name){
			differentIn.push_back(b->input[j]);
			j++;
		}
		else{
			assert(commonIn[n]->at(i)->name == b->input[j]->name);
			tem.push_back(commonIn[n]->at(i));
			if(pair[n]->at(0)->circuitNum != b->circuitNum)
				tem.push_back(b->input[j]);
			i++;j++;	
		}
	}
	if(i == commonIn[n]->size()){
		for(;j < b->input.size();j++)
			differentIn.push_back(b->input[j]);
		commonIn[n]->clear();
		for(int k=0;k<tem.size();k++)
			commonIn[n]->push_back(tem[k]);
		return true;
	}	
	else 
		return false;
}


//證明不同的input為dont care term
bool PotentialPair::ProveDontCare(Wire* a,Wire* b){
	if(differentIn.size()==0)
		return true;

	assert(differentIn.size()>0);
	RandGenerator rand;
	int a_cur=a->curSim,b_cur=b->curSim;
	//給different input新的值	
	for(int i=0;i<differentIn.size();i++){
		int tem = rand.getRand();
		differentIn[i]->curSim=tem;
	}

	//執行並證明different input不影響值
	DFSandOperate(a);
	DFSandOperate(b);
	//將traversed設為0並清除store
	for(int i=0;i<store.size();i++)
		store[i]->traversed=false;
	store.clear();
	if(a_cur!=a->curSim || b_cur!=b->curSim)
		return false;	
	return true;
}

//證明function相同或相反
bool PotentialPair::ProveFunction(Wire* a,Wire*b ,int n){
	if(commonIn[n]->size()==0)
		return true;

	RandGenerator rand;

	if(a->circuitNum!=b->circuitNum){
		assert(commonIn[n]->size()%2==0);
		for(int i=0;i<commonIn[n]->size();i+=2){
			int tem = rand.getRand();
			commonIn[n]->at(i)->curSim=tem;
			commonIn[n]->at(i+1)->curSim=tem;
		}
	}
	else{
		for(int i=0;i<commonIn[n]->size();i++){
			int tem = rand.getRand();
			commonIn[n]->at(i)->curSim=tem;
		}
	}
	DFSandOperate(a);
	DFSandOperate(b);
	//將traversed設為0並清除store
	for(int i=0;i<store.size();i++)
		store[i]->traversed=false;
	store.clear();
	
	if(a->curSim == b->curSim || a->curSim == ~(b->curSim))
		return true;	
	return false;
	
}

//做DFS的時候順便operate
void PotentialPair::DFSandOperate(Gate* source){
	for(int i = 0; i < source -> input.size(); ++i){
   	if(!(source -> input[i] -> traversed)){
    		source -> input[i] -> traversed = true;
    		DFSandOperate(source -> input[i]);
    	   	}
      	}
	source->operate();
	store.push_back(source);
}

void PotentialPair::Out(){
	cout<<pair.size()<<endl;
	int count=0;	
	for(int i=0;i<pair.size();i++)
		if(pair[i]->size()== 1)
			count++;
	cout<<count<<endl;

}
