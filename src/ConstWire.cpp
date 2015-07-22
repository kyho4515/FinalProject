#include "ConstWire.h"


ConstWire::ConstWire(CirMgr* circuitOne,CirMgr* circuitTwo){
	findconst(circuitOne->dfsList,circuitOne->input);//找circuitOne的const wire
	findconst(circuitTwo->dfsList,circuitTwo->input);//找circuitTwo的const wire
}


void ConstWire::findconst(vector<Gate*>& dfs_list, vector<Gate*>& input_list){
	queue<Wire*>constwire=simulation(dfs_list,input_list);
	vector<Wire*>
	while(constwire.size()!=0){
		


	}	
}


queue<Wire*> ConstWire::simulation(vector<Gate*>& dfs_list,vector<Gate*>&input_list){
	vector<Wire*>constwire;
	RandGenerator rand;
	int tmp;
	int repeat=1;
	//隨機給予每個input不同的值
	for(int r=0;r<repeat;r++){
		for(int input_num=0; input_num < input_list.size(); ++input_num){
			tmp = rand.getRand();
			input_list[input_num] -> curSim = tmp;
  		}
		//挑出simulation後結果為const的wire
  		for(int gate_num=0;gate_num < dfs_list.size(); gate_num++){
			assert(dfs_list[gate_num]->gateType!=Const);
			dfs_list[gate_num] -> operate();
			if(dfs_list[gate_num]->gateType == Wir && (dfs_list[gate_num)->curSim==0 || dfs_list[gate_num)->curSim==~0)
				constwire.push_back((Wire*)dfs_list[gate_num]);
		}
	}

	return constwire;
  
}


void ConstWire::Out(){



}

