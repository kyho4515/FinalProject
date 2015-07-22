#ifndef CONSTWIRE
#define CONSTWIRE

#include <vector>
#include <queue>
#include "Gate.h"
class ConstWire{
	public:
		ConstWire(CirMgr*,CirMgr*);
		~ConstWire();
		void Out()const;
	private:
		vector<vector<Wire*>*>const1;
		vector<vector<Wire*>*>const0;
		void findconst(vector<Gate*>&);//找到所有const wire
		vector<Wire*> simulation(vector<Gate*>&, vector<Gate*>&);//初步挑出是const的wire(回傳的vector是1.0混合的)
		bool checkconstant(Wire*)
};

#endif
