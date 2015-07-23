#include "ConstWire.h"


ConstWire::ConstWire(vector<Gate*>input_list,vector<Gate*>dfs_list){
	input=input_list;
	dfslist=dfs_list;
	queue<Wire*>constwire=simulation(dfslist,input,input.size());
	cout<<"size of constwire : "<<constwire.size()<<endl;
}



queue<Wire*> ConstWire::simulation(vector<Gate*>& dfs_list,vector<Gate*>&input_list,int repeat){
	queue<Wire*>constwire;
	RandGenerator rand;
	int tmp,queuesize,befSim;
	for(int r=0;r<repeat;r++){
		//隨機給予每個input不同的值
		for(int input_num=0; input_num < input_list.size(); ++input_num){
			tmp = rand.getRand();
			input_list[input_num] -> curSim = tmp;
  		}
		//挑出simulation後結果為const的wire
		if(constwire.size()==0){
			//第一次simulation
  			for(int gate_num=0;gate_num < dfs_list.size(); gate_num++){
				dfs_list[gate_num] -> operate();
				if(dfs_list[gate_num]->gateType == Wir && (dfs_list[gate_num]->curSim==0 || dfs_list[gate_num]->curSim==~0))
					constwire.push((Wire*)dfs_list[gate_num]);
			}
		}
		else{
			//第N次simulation
			queuesize=constwire.size();
			for(int i=0;i<queuesize;i++){
				befSim=constwire.front()->curSim;
				DFSandOperate(constwire.front());
				if(befSim==constwire.front()->curSim)
					constwire.push(constwire.front());
				constwire.pop();
				assert(queuesize>=constwire.size());
			}
			resetTraversed();
		}
		//cout<<"size of constwire after simulation "<<r<<" : "<<constwire.size()<<endl;
	}

	return constwire;
  
}


void ConstWire::Out()const{



}

void ConstWire::DFSandOperate(Gate* source){
	for(int i = 0; i < source -> input.size(); ++i){
   	if(!(source -> input[i] -> traversed)){
    		source -> input[i] -> traversed = true;
    		DFSandOperate(source -> input[i]);
    	   	}
      	}
	source->operate();
	storage.push_back(source);
}

void ConstWire::resetTraversed(){
   for(int i=0; i < storage.size(); ++i)
     storage[i] -> traversed = false;
	storage.clear();
}
