#ifndef CONSTWIRE
#define CONSTWIRE

#include <vector>
#include <queue>
#include "Gate.h"
#include "util.h"
class ConstWire{
	public:
		ConstWire(vector<Gate*>input,vector<Gate*>dfs_list);
		~ConstWire();
		void Out()const;
	private:
		vector<Wire*>const1;
		vector<Wire*>const0;
		vector<Gate*>input;
		vector<Gate*>dfslist;
		vector<Gate*>storage;//儲存DFSandOperate後產生的dfslist
		queue<Wire*> simulation(vector<Gate*>&, vector<Gate*>&, int);//初步挑出是const的wire(回傳的vector是1.0混合的)
		bool checkconstant(Wire*);
		void DFSandOperate(Gate*);//更新gate的curSim
		void resetTraversed();//reset traversed
		
};

#endif
