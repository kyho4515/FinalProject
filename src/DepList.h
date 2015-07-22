#ifndef DEPLIST
#define DEPLIST

#include "Gate.h"

class DepList{


	public:
		DepList(vector<Gate*>  ,vector<Gate*> );
		~DepList();
		bool Check(Wire*,Wire*);//檢查兩個Wire是不是dependent
		void Out()const;//cout somthing

	private:
		vector<vector<Gate*>*>dep_list1;//照idd由小到大排序
		vector<vector<Gate*>*>dep_list2;//照idd由小到大排序
		vector<vector<Gate*>*>dep_list_all;//照idd由小到大排序

		vector<vector<Gate*>*> build_dep_list(vector<Gate*>&);//利用傳進來的output建立dep_list
		void merge();//合併dep_list1和dep_list2
		bool check_dep(Gate*,Gate*)const;//確定兩個output是不是dependent
		bool CheckinOne(Wire*,Wire*);//檢查兩個Wire在cutcuitOne中是不是dependent
		bool CheckinTwo(Wire*,Wire*);//檢查兩個Wire在curcuitTwo中式不是dependent
		bool CheckinAll(Wire*,Wire*);//檢查兩個Wire在合併的dep_list中是不是dependent
		bool findinOne(Gate*,Gate*);//在dep_list1中找到gate是否dependent
		bool findinTwo(Gate*,Gate*);//在dep_list2中找到gate是否dependent
		bool findinAll(Gate*,Gate*);//在dep_list_all中找到gate是否dependent
};	

#endif 
