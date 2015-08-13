#ifndef POTENTIALPAIR
#define POTENTIALPAIR

#include "Gate.h"
#include "util.h"
#include <fstream>
#include <algorithm>
#include <math.h>  
#include <stack>

class PotentialPair{
	public:
		PotentialPair(vector<vector<Wire*>*>);//input為_FECpair
		~PotentialPair(){
			for(int i=0;i<pair.size();i++){
				delete pair[i];
				delete commonIn[i];
			}
			delete number;
		}
		void Out();
	private:
		vector<vector<Wire*>*> pair;
		vector<vector<Gate*>*> commonIn;//每一組pair的common input
		vector<Gate*>differentIn;
		vector<Gate*>store;
		vector<int> overflowpair;
		vector<vector<Gate*> > temDFSlist;
		int start;	
		int *number;
		fstream output;

		bool RepeatInPair(int starting,Wire* unfind);//從pair中找到wire可能的位置
		void FindCommonInput(Wire*,Wire*,int);//找相同的input
		void FindCommonInput(Wire*,int);//
		bool ProveDontCare(Wire*,Wire*,int);//證明不同的input為dont care term
		bool ProveFunction(Wire*,Wire*,int);//證明function相同或相反
		void DFS(Gate*);//做DFS
		bool ProveEqual(Wire*,Wire*,int);
		bool InSameLine(Wire*,Wire*);//證明wire沒有上下關係
		bool DFSFind(Gate*,Gate*);//DFS尋找後面的wire
		void cleanstore();
		static bool sortcompare(const Wire* l,const Wire* r){  //sort的判斷式
			return l->gateLevel < r->gateLevel;		
		}
		void GiveNumber(vector<Gate*>*,int,bool);//10個input以內用確定的證明，超過10個用隨機證明
		
};
#endif
