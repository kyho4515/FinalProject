
#include "DepList.h"
#include <fstream>
DepList::DepList(vector<Gate*> output_list1,vector<Gate*> output_list2){
	cout<<"start bulit dep_list..."<<endl;
	dep_list1=build_dep_list(output_list1);
	dep_list2=build_dep_list(output_list2);
	merge();
}

DepList::~DepList(){
  for(int i=0; i < dep_list1.size(); ++i){
    delete dep_list1[i];
  }
  for(int i=0; i < dep_list2.size(); ++i){
    delete dep_list2[i];
  }
  for(int i=0; i < dep_list_all.size(); ++i){
    delete dep_list_all[i];
  }
}

//利用傳進來的output建立dep_list
vector<vector<Gate*>*> DepList::build_dep_list(vector<Gate*>& output_list){
	
	vector<vector<Gate*>*>dep_list;
	//每個output都要檢查
	for(int output_num=0;output_num < output_list.size();output_num++){
		if (dep_list.size()!=0){
			for(int deplist_num=0;deplist_num<dep_list.size();deplist_num++){
				if(check_dep(dep_list[deplist_num]->at(0),output_list[output_num])){
					dep_list[deplist_num]->push_back(output_list[output_num]);
				}
			}
		}
		dep_list.push_back(new vector<Gate*>);
		dep_list[output_num]->push_back(output_list[output_num]);
	}
	return dep_list;
}

//確定兩個output是不是dependent
bool DepList::check_dep(Gate* g1,Gate* g2)const{
	for(int g1_num=0,g2_num=0;g1_num < g1->total_input.size() && g2_num < g2->total_input.size();){
		if(g1->total_input[g1_num]->name < g2->total_input[g2_num]->name)
			g1_num++;
		else if (g1->total_input[g1_num]->name > g2->total_input[g2_num]->name)
			g2_num++;
		else 
			return true;

	}
	return false;
}

//合併dep_list1和dep_list2
void DepList::merge(){
	assert(dep_list1.size()==dep_list2.size());
	for(int list_index=0;list_index < dep_list1.size();list_index++){
		dep_list_all.push_back(new vector<Gate*>);
		for(int l1=0,l2=0;l1 < dep_list1[list_index]->size() && l2 <dep_list2[list_index]->size();){
			if(dep_list1[list_index]->at(l1)->idd < dep_list2[list_index]->at(l2)->idd){
				l1++;
			}
			else if (dep_list1[list_index]->at(l1)->idd > dep_list2[list_index]->at(l2)->idd){
				l2++;
			}
			else {
				assert(dep_list1[list_index]->at(l1)->idd==dep_list2[list_index]->at(l2)->idd);
				dep_list_all[list_index]->push_back(dep_list1[list_index]->at(l1));
				l1++;
				l2++;
			}
		}
	}	
}

//檢查兩個Wire的output是不是dependent
bool DepList::Check(Wire* w1,Wire* w2){
	if(w1->circuitNum==1 && w2->circuitNum==1)
		return CheckinOne(w1,w2);
	else if(w1->circuitNum==2 && w2->circuitNum==2)
		return CheckinTwo(w1,w2);
	else {
		assert(w1->circuitNum!=w2->circuitNum);
		return CheckinAll(w1,w2);
	}
}

bool DepList::CheckinOne(Wire* w1,Wire* w2){
	//assert(w1->gateType == Wire && w2->gateType == Wire);
	vector<Gate*>check1=w1->final_output,check2=w2->final_output;
	for(int w1_index=0;w1_index < check1.size();w1_index++){
		for(int w2_index=0; w2_index < check2.size();w2_index++){
			if(!findinOne(check1[w1_index],check2[w2_index]))
				return false;
		}
	}
	return true;
}

bool DepList::findinOne(Gate* o1,Gate* o2){
	assert(o1->gateType==Output && o2->gateType==Output);	
	Gate* big;
	Gate* small;
	if(o1->idd>o2->idd){
		big=o1;small=o2;
	}
	else{
		big=o2;small=o2;
	}
	assert(big->idd>small->idd);
	for(int i=0;i<dep_list1[small->idd]->size();++i){
		if(dep_list1[small->idd]->at(i)->idd == big->idd)
			return true;
	}
	return false;
}

bool DepList::CheckinTwo(Wire* w1,Wire* w2){
	//assert(w1->gateType==Wire && w2->gateType==Wire);
	vector<Gate*>check1=w1->final_output,check2=w2->final_output;
	for(int w1_index=0;w1_index < check1.size();w1_index++){
		for(int w2_index=0; w2_index < check2.size();w2_index++){
			if(!findinTwo(check1[w1_index],check2[w2_index]))
				return false;
		}
	}
	return true;
}

bool DepList::findinTwo(Gate* o1,Gate* o2){
	assert(o1->gateType==Output && o2->gateType==Output);	
	Gate* big;
	Gate* small;
	if(o1->idd>o2->idd){
		big=o1;small=o2;
	}
	else{
		big=o2;small=o2;
	}
	assert(big->idd>small->idd);
	for(int i=0;i<dep_list2[small->idd]->size();++i){
		if(dep_list2[small->idd]->at(i)->idd == big->idd)
			return true;
	}
	return false;
}

bool DepList::CheckinAll(Wire* w1,Wire* w2){
	//assert(w1->gateType==Wire && w2->gateType==Wire);
	vector<Gate*>check1=w1->final_output,check2=w2->final_output;
	for(int w1_index=0;w1_index < check1.size();w1_index++){
		for(int w2_index=0; w2_index < check2.size();w2_index++){
			if(!findinAll(check1[w1_index],check2[w2_index]))
				return false;
		}
	}
	return true;
}

bool DepList::findinAll(Gate* o1,Gate* o2){
	assert(o1->gateType==Output && o2->gateType==Output);	
	Gate* big;
	Gate* small;
	if(o1->idd>o2->idd){
		big=o1;small=o2;
	}
	else{
		big=o2;small=o2;
	}
	assert(big->idd>small->idd);
	for(int i=0;i<dep_list_all[small->idd]->size();++i){
		if(dep_list_all[small->idd]->at(i)->idd == big->idd)
			return true;
	}
	return false;
}

void DepList::Out()const{
	int count;
	/*cout<<"dep_list of circuitOne:"<<endl;
	for(int i=0;i<dep_list1.size();++i){
		cout<<"dep_list of output "<<dep_list1[i]->at(0)->name<<":";
		for(int j=0;j<dep_list1[i]->size();j++){
			cout<<dep_list1[i]->at(j)->name<<", ";
		}
		cout<<endl;
	}	
	
	cout<<endl<<"dep_list of CircuitTwo:"<<endl;
	for(int i=0;i<dep_list2.size();++i){
		cout<<"dep_list of output "<<dep_list2[i]->at(0)->idd<<":";
		for(int j=0;j<dep_list2[i]->size();j++){
			cout<<dep_list2[i]->at(j)->name<<", ";
		}
		cout<<endl;
	}
	*/
	/*fstream fout("output/graph.dot",ios::out);
	fout<<"graph{"<<endl;
	for(int i=0;i<dep_list1.size();++i){
		for(int j=1;j<dep_list1[i]->size();j++){
			fout<<dep_list1[i]->at(0)->idd<<"--"<<dep_list1[i]->at(j)->idd<<";"<<endl;
		}
	}
	fout<<"}"<<endl;*/
	
	cout<<endl<<"dep_list of 1:"<<endl;
	cout<<"edge number:";
	count=0;
	for(int i=0;i<dep_list1.size();++i){
		count+=dep_list1[i]->size()-1;
	}
	cout<<count<<endl;
	cout<<endl<<"dep_list of 2:"<<endl;
	cout<<"edge number:";
	count=0;
	for(int i=0;i<dep_list2.size();++i){
		count+=dep_list2[i]->size()-1;
	}
	cout<<count<<endl;
	cout<<endl<<"dep_list of All:"<<endl;
	cout<<"edge number:";
	count=0;
	for(int i=0;i<dep_list_all.size();++i){
		count+=dep_list_all[i]->size()-1;
	}
	cout<<count<<endl;
}
