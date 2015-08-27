#ifndef CONSTWIRE
#define CONSTWIRE

#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include "Gate.h"
#include "util.h"
class ConstWire{
	public:
		ConstWire(vector<Gate*>dfs_list);
		~ConstWire();
		void Out()const;
	private:
		vector<Wire*>const1;
		vector<Wire*>const0;
		vector<Gate*>input;
		vector<Gate*>constinput;
		vector<Gate*>dfslist;
		vector<vector<Gate*>*>cutset;
		vector<Gate*>storage;//儲存DFS後產生的dfslist
		int number[5];
		Gate* constTrue;
		Gate* constFalse;
		SatSolver solver;
		Var T;
		Var F;
		fstream fout;

		void Find();//挑出是const的wire
		bool checkconstant(Wire*);
		void DFS(Gate*,vector<Gate*>&);//do DFS
		void resetTraversed(vector<Gate*>&);//reset traversed
		static bool sortcompare(const Wire* l,const Wire* r){  //sort的判斷式
			return l->gateLevel < r->gateLevel;		
		}
		bool ProveBySimulate(Gate* source,int);
		bool ProveBySAT(Gate* source,int);
		void genProofModel();
		void Categorize();
		void Check();
		
};

#endif
