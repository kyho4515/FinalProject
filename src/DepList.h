#ifndef DEPLIST
#define DEPLIST

#include "Gate.h"

class DepList{


	public:
		DepList(vector<Gate*> output_list1,vector<Gate*>output_list2);
		~DepList();
		bool Check(Wire*,Wire*);//檢查兩個Wire是不是dependent
		void Out();//cout somthing

	private:
		vector<vector<Gate*>*>dep_list1;
		vector<vector<Gate*>*>dep_list2;
		vector<vector<Gate*>*>dep_list_all;

		vector<vector<Gate*>*> build_dep_list(vector<Gate*>);//利用傳進來的output建立dep_list
		void merge();//合併dep_list1和dep_list2
		bool check_dep(Gate*,Gate*);//確定兩個output是不是dependent
};	

#endif 
