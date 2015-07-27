#ifndef POTENTIALPAIR
#define POTENTIALPAIR

#include "Gate.h"
#include "util.h"
class PotentialPair{
	public:
		PotentialPair(vector<vector<Wire*>*>);//input為_FECpair
		void Out();
	private:
		vector<vector<Wire*>*> pair;
		vector<vector<Gate*>*> commonIn;//每一組pair的common input
		vector<Gate*>differentIn;
		vector<Gate*>store;

		bool RepeatInPair(int starting,Wire* unfind);//從pair中找到wire可能的位置
		void FindCommonInput(Wire*,Wire*,int);//找相同的input
		bool FindCommonInput(Wire*,int);//
		bool ProveDontCare(Wire*,Wire*);//證明不同的input為dont care term
		bool ProveFunction(Wire*,Wire*,int);//證明function相同或相反
		void DFSandOperate(Gate*);//做DFS的時候順便operate
		bool ProveEqual(Wire*,Wire*,int);
		
};
#endif
