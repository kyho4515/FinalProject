#include "pair.h"
#include "../lib/tm_usage.h"
using namespace std;
using namespace CommonNs;

//input為_FECpair
PotentialPair::PotentialPair(vector<vector<Wire*>*> FEC){
	int total_wire=0,ori=FEC.size();
	vector<Gate*> wire;
	output.open("./output.txt",ios::out);	

	for(int FEC_num=0;FEC_num < FEC.size();FEC_num++){
		
		///先依照gateLevel做sort，小的在前面
		sort(FEC[FEC_num]->begin(),FEC[FEC_num]->end(),sortcompare);
		for(int i=0;i<FEC[FEC_num]->size()-1;i++){
			cout<<FEC[FEC_num]->at(i)->gateLevel<<":";
			assert(FEC[FEC_num]->at(i)->gateLevel <= FEC[FEC_num]->at(i+1)->gateLevel);
		}		
		cout<<FEC[FEC_num]->at(FEC[FEC_num]->size()-1)->gateLevel<<endl;
		///////////////
		
		///確認任兩條wire不會有上下關係
		bool ex;
		for(int i=1;i < FEC[FEC_num]->size();i++){
			ex=false;
			DFS(FEC[FEC_num]->at(i));
			for(int j=0;j < i;j++){
				if(FEC[FEC_num]->at(j)->traversed){//存在在下面的wire
					FEC[FEC_num]->at(j)->backwire.push_back(FEC[FEC_num]->at(i));
					ex=true;				
				}
			}
			if(!ex){
				assert(((Wire*)store[store.size()-1])->mark==false);
				for(int j=0;j < store.size();j++)
					if(store[j]->gateType==Wir && ((Wire*)store[j])->mark)
						ex=true;
			}
			if(ex){
				FEC[FEC_num]->at(i)->potentialCut = false;
				vector<Wire*>::iterator it = FEC[FEC_num]->begin()+i;
      		FEC[FEC_num]->erase(it);
				--i;
			}
			cleanstore();
		}
		cout<<"......................"<<endl;
		for(int i=0;i<FEC[FEC_num]->size()-1;i++){
			if(FEC[FEC_num]->at(i)->gateLevel%2==1)
				cout<<FEC[FEC_num]->at(i)->name<<endl;
			assert(FEC[FEC_num]->at(i)->gateLevel%2==0);
			cout<<FEC[FEC_num]->at(i)->gateLevel<<":";
			assert(FEC[FEC_num]->at(i)->gateLevel <= FEC[FEC_num]->at(i+1)->gateLevel);
		}	
		cout<<FEC[FEC_num]->at(FEC[FEC_num]->size()-1)->gateLevel<<endl;
		
		/***清除只剩一條wire的FEC***/
		if(FEC[FEC_num]->size()==1){
			FEC[FEC_num]->at(0)->potentialCut = false;
      	FEC[FEC_num]->clear();
			delete FEC[FEC_num];
			vector<vector<Wire*>*>::iterator it = FEC.begin()+FEC_num;
      	FEC.erase(it);
			--FEC_num;
			cout<<"vanish"<<endl;
		}
		else{
			assert(FEC[FEC_num]->size()>1);
			for(int i=0;i < FEC[FEC_num]->size();i++)
				FEC[FEC_num]->at(i)->mark=true;
		}
		/***************************/		
		///////////////
		

		cout<<FEC.size()-FEC_num-1<<" left"<<endl<<endl;

	}
	for(int i=0;i<FEC.size();i++)
		total_wire+=FEC[i]->size();
	output<<"original size: "<<ori<<endl;
	output<<"new size: "<<FEC.size()<<endl;
	output<<"total wire: "<<total_wire<<endl;

	//assert(0);
	

/////以上為測試程式

	number=new int[5];
	number[0]=1431655765;
	number[1]=858993459;
	number[2]=252645135;
	number[3]=16711935;
	number[4]=65535;

	int count=0;	
	for(int FEC_num=0;FEC_num < FEC.size();FEC_num++){
		start=pair.size();//取這一組FEC的起點
		cout<<endl<<"an FEC start................"<<endl<<"starting:"<<start<<endl;
/*
		///先依照gateLevel做sort，小的在前面
		sort(FEC[FEC_num]->begin(),FEC[FEC_num]->end(),sortcompare);
		for(int i=0;i<FEC[FEC_num]->size()-1;i++){
			cout<<FEC[FEC_num]->at(i)->gateLevel<<":";
			assert(FEC[FEC_num]->at(i)->gateLevel <= FEC[FEC_num]->at(i+1)->gateLevel);
		}		
		cout<<FEC[FEC_num]->at(FEC[FEC_num]->size()-1)->gateLevel<<endl;
		///////////////

		///gateleveling
		bool ex;
		for(int i=1;i < FEC[FEC_num]->size();i++){
			ex=false;
			DFS(FEC[FEC_num]->at(i));
			for(int j=0;j < i;j++){
				if(FEC[FEC_num]->at(j)->traversed){//存在在下面的wire
					FEC[FEC_num]->at(j)->backwire.push_back(FEC[FEC_num]->at(i));
					ex=true;				
				}
			}
			if(ex){
				FEC[FEC_num]->at(i)->potentialCut = false;
				vector<Wire*>::iterator it = FEC[FEC_num]->begin()+i;
      		FEC[FEC_num]->erase(it);
				--i;
			}
			cleanstore();
		}
		cout<<"......................"<<endl;
		for(int i=0;i<FEC[FEC_num]->size()-1;i++){
			if(FEC[FEC_num]->at(i)->gateLevel%2==1)
				cout<<FEC[FEC_num]->at(i)->name<<endl;
			assert(FEC[FEC_num]->at(i)->gateLevel%2==0);
			cout<<FEC[FEC_num]->at(i)->gateLevel<<":";
			assert(FEC[FEC_num]->at(i)->gateLevel <= FEC[FEC_num]->at(i+1)->gateLevel);
		}	
		cout<<FEC[FEC_num]->at(FEC[FEC_num]->size()-1)->gateLevel<<endl;
		//////////////
*/		
		cout<<"FECpair size: "<<FEC[FEC_num]->size()<<endl;

		for(int element=0;element < FEC[FEC_num]->size();element++){			
			//cout<<FEC[FEC_num]->at(element)->name<<endl;
				
			if(!RepeatInPair(start,FEC[FEC_num]->at(element))){
				pair.push_back(new vector<Wire*>);
				commonIn.push_back(new vector<Gate*>);
				pair[pair.size()-1]->push_back(FEC[FEC_num]->at(element));

				assert(store.empty());
				DFS(FEC[FEC_num]->at(element));
				temDFSlist.push_back(store);
				cleanstore();	
			}
			
			cout<<FEC[FEC_num]->size()-element-1<<" wire left"<<endl;
		}
///檢查是否有Wire存在兩種不同的function(simulation有錯)
		count=0;
		for(int i=start; i < pair.size();i++){
			cout<<"pair "<<i<<" : "<<pair[i]->size()<<endl;
			count+=pair[i]->size();
		}
		if(count > FEC[FEC_num]->size()){
			overflowpair.push_back(FEC[FEC_num]->size());
			cout<<"overflow"<<endl;
		}
//////

///把只有一個wire的pair刪除
		for(int i=start;i<pair.size();i++){
   	   if(pair[i] -> size() == 1){
      		pair[i] -> at(0) -> potentialCut = false;
      		delete pair[i];
				delete commonIn[i];
      		vector<vector<Wire*>*>::iterator it = pair.begin()+i;
      		pair.erase(it);
				vector<vector<Gate*>*>::iterator at = commonIn.begin()+i;
      		commonIn.erase(at);
      		--i;
			}
    		}
//////

		cout<<"temDFSlist size = "<<temDFSlist.size()<<endl;
			temDFSlist.clear();
		cout<<"pair size: "<<pair.size()<<endl;
		cout<<FEC.size()-FEC_num-1<<" pair left"<<endl;
		cout<<"an FEC over................"<<endl;		
		

	}

}

//從pair中找到wire可能的位置
bool PotentialPair::RepeatInPair(int starting,Wire* unfind){
	bool repeat=false;
	for(int i=starting;i < pair.size();i++){
		assert(differentIn.size()==0);
		if(pair[i]->size()==1){
			assert(commonIn[i]->size() == 0);
			FindCommonInput(pair[i]->at(0),unfind,i);
			if(ProveFunction(pair[i]->at(0),unfind,i) && ProveDontCare(pair[i]->at(0),unfind,i)){			
				pair[i]->push_back(unfind);
				repeat=true;		
			}
			else
				commonIn[i]->clear();
		}
		else{
			assert(pair[i]->size() > 1);
			vector<Gate*>* tem=new vector<Gate*>;
			for(int c = 0; c < commonIn[i]->size();c++)
				tem->push_back(commonIn[i]->at(c));
			FindCommonInput(unfind,i);
			if(ProveFunction(pair[i]->at(0),unfind,i) && ProveDontCare(pair[i]->at(0),unfind,i)){
				pair[i]->push_back(unfind);
				repeat=true;
			}
			else{
				delete commonIn[i];
				commonIn[i]=tem;	
			}
		} 
		
///清除vector<> differentIn留給下一個wire使用 
		differentIn.clear();
//////

///如果commonIn存到不同circuit的wire的input，將commonIn變成只有和第一個wire有關
		if(commonIn[i]->size()>1 && commonIn[i]->at(0)->name == commonIn[i]->at(1)->name){
			
			assert(commonIn[i]->size()%2 == 0);			
			vector<Gate*>* tem=new vector<Gate*>;
			for(int j=0;j<commonIn[i]->size();j+=2){
				tem->push_back(commonIn[i]->at(j));
			}
			delete commonIn[i];
			commonIn[i]=tem;
		}
//////

	}
	return repeat;
}

//找相同的input還有不同的input，n是commonIn的位置
void PotentialPair::FindCommonInput(Wire* a,Wire* b,int n){
	//cout<<"find common input (size=1)"<<endl;
	for(int i=0,j=0;i < a->total_input.size() || j < b->total_input.size();){
		if(j==b->total_input.size() || (a->total_input.size()-i!=0 && a->total_input[i]->name < b->total_input[j]->name)){
			assert(i< a->total_input.size());			
			differentIn.push_back(a->total_input[i]);
			i++;
		}
		else if(i==a->total_input.size() ||  a->total_input[i]->name > b->total_input[j]->name){
			assert(j<b->total_input.size());			
			differentIn.push_back(b->total_input[j]);
			j++;
		}
		else{
			assert(a->total_input[i]->name==b->total_input[j]->name);
			assert(i < a->total_input.size() || j < b->total_input.size());
			commonIn[n]->push_back(a->total_input[i]);
			if(a->circuitNum!=b->circuitNum)
				commonIn[n]->push_back(b->total_input[j]);
			i++;j++;
		}
	}

	
}


void PotentialPair::FindCommonInput(Wire* b,int n){
	//cout<<"find common input (size>1)"<<endl;	
	vector<Gate*>*tem=new vector<Gate*>;
	for(int i=0, j=0;i < commonIn[n]->size() || j < b->total_input.size();){
		if(j==b->total_input.size() ||( commonIn[n]->size()-i!=0 && commonIn[n]->at(i)->name < b->total_input[j]->name)){
			differentIn.push_back(commonIn[n]->at(i));
			i++;
		}
		else if(i==commonIn[n]->size() || commonIn[n]->at(i)->name > b->total_input[j]->name){
			differentIn.push_back(b->total_input[j]);
			j++;
		}
		else{
			assert(commonIn[n]->at(i)->name == b->total_input[j]->name);
			tem->push_back(commonIn[n]->at(i));
			if(pair[n]->at(0)->circuitNum != b->circuitNum)
				tem->push_back(b->total_input[j]);
			i++;j++;	
		}
	}
	delete commonIn[n];
	commonIn[n]=tem;

}


//證明不同的input為dont care term
bool PotentialPair::ProveDontCare(Wire* a,Wire* b, int n){
	//cout<<"prove dont care"<<endl;
	
///如果沒有different input就直接回傳true
	if(differentIn.size()==0)
		return true;
	
//////
	
	assert(differentIn.size()>0);
	
	RandGenerator rand;
	int repeat;
	int a_cur=a->curSim,b_cur=b->curSim;

	assert(n-start>=0);

	store=temDFSlist[n-start];
	for(int i=0;i<store.size();i++)
		store[i]->traversed=true;
	DFS(b);	
///決定執行simulation的次數
	if(differentIn.size()<=5)
		repeat=1;
	else if(differentIn.size()<=20)
		repeat=pow(2,differentIn.size()-5);
	else
		repeat=1000;
//////

	for(int c=0;c<repeat;c++){
		assert(!store.empty());		
		///給different input新的值	
		if(differentIn.size()<=20){
			GiveNumber(&differentIn,c,0);
		}
		else
			for(int i=0;i<differentIn.size();i++){
				int tem = rand.getRand();
				differentIn[i]->curSim=tem;
			}
		//////

		///執行並證明different input會不會影響值
		for(int i=0;i<store.size();i++)
			store[i]->operate();	
		
		if(a_cur!=a->curSim || b_cur!=b->curSim){
			cleanstore();
			return false;
		}	
	}
	cleanstore();
	return true;
}

//證明function相同或相反
bool PotentialPair::ProveFunction(Wire* a,Wire*b ,int n){
	//cout<<"prove fnction"<<endl;
	
	if(commonIn[n]->size()==0){
		return true;
	}

	RandGenerator rand;
	int repeat;
	
	assert(n-start>=0);
	store=temDFSlist[n-start];
	for(int i=0;i<store.size();i++)
		store[i]->traversed=true;
	DFS(b);

///決定執行simulation的次數
	if((a->circuitNum==b->circuitNum && commonIn[n]->size()<=5)||(a->circuitNum!=b->circuitNum && commonIn[n]->size()<=10))
		repeat=1;
	else if(a->circuitNum==b->circuitNum && commonIn[n]->size()<=20)
		repeat=pow(2,commonIn[n]->size()-5);
	else if(a->circuitNum!=b->circuitNum && commonIn[n]->size()<=40)
		repeat=pow(2,commonIn[n]->size()/2-5);
	else
		repeat=1000;
//////
	for(int c=0;c<repeat;c++){
///給common input新的值	
		if(a->circuitNum!=b->circuitNum){
			assert(commonIn[n]->size()%2==0);
			assert(!store.empty());			
			if(commonIn[n]->size()/2<=20){
				GiveNumber(commonIn[n],c,1);
			}
			else{
				for(int i=0;i<commonIn[n]->size();i+=2){
					int tem = rand.getRand();
					commonIn[n]->at(i)->curSim=tem;
					commonIn[n]->at(i+1)->curSim=tem;
				}
			}
		}
		else{
			if(commonIn[n]->size()<=20){
				GiveNumber(commonIn[n],c,0);
			}
			else{
				for(int i=0;i<commonIn[n]->size();i++){
					int tem = rand.getRand();
					commonIn[n]->at(i)->curSim=tem;
				}
			}
		}
//////

		///	
		for(int i=0;i<store.size();i++)
			store[i]->operate();	
		///

		if(a->curSim != b->curSim && a->curSim != ~(b->curSim)){
			cleanstore();
			return false;
		}	
	}	
	cleanstore();
	return true;
	
}

//20個input以內用固定的值，超過20個用隨機的值，mode0用於provedontcare和provefunction時相同circuit的wire，mode1用於provefunction時不同circuit的wire
void PotentialPair::GiveNumber(vector<Gate*>* input,int time, bool mode){	
	//cout<<mode<<endl;
	int divisor,i;	
	if(time==0){
		for(i=0;i<input->size() && ((!mode && i<5) || (mode && i<10));){
			if(mode){
				assert(input->size()%2==0);
				input->at(i)->curSim=number[i/2];
				input->at(i+1)->curSim=number[i/2];
				i+=2;
			}
			else{			
				input->at(i)->curSim=number[i];
				i++;
			}
		}
		for(;i<input->size();i++){
			assert(i>=5 || i>=10);
			input->at(i)->curSim=0;
		}
	}
	else{
		if(mode){
			for(i=10;i<input->size();i+=2){
				divisor=pow(2,i/2-5);
				if(time%divisor==0){
					if(input->at(i)->curSim==0){
						input->at(i)->curSim=~0;
						input->at(i+1)->curSim=~0;
					}
					else{ 
						input->at(i)->curSim=0;
						input->at(i+1)->curSim=0;
					}
				}
			}
		}
		else{
			for(i=5;i<input->size();i++){
				divisor=pow(2,i-5);
				if(time%divisor==0){
					if(input->at(i)->curSim==0)
						input->at(i)->curSim=~0;
					else 
						input->at(i)->curSim=0;
				}
			}
		}
	}
}

//證明wire沒有上下關係
bool PotentialPair::InSameLine(Wire* a,Wire* b){
	if(a->circuitNum != b->circuitNum || a->gateLevel == b->gateLevel)
		return false;
	Wire* source=a->gateLevel > b->gateLevel ? a : b;
	Wire* target=a->gateLevel < b->gateLevel ? a : b;
	
	assert(target->gateType == Wir);
	//cout<<"InSameLine end"<<endl;
	return DFSFind(source,target);	
	

}

//DFS尋找後面的wire
bool PotentialPair::DFSFind(Gate* source,Gate* target){
	source->traversed = true;
	store.push_back(source);
	if(source->gateType == Input || source->gateType == Const || source->gateLevel < target->gateLevel)
		return false;
	else if(source->gateType == Wir && source->name == target->name)
		return true;
	else {
		for(int i=0;i < source->input.size();i++)
			 if(!(source->input[i]->traversed) && DFSFind(source->input[i],target))
				return true;
		return false;
	}
}

//做DFS
void PotentialPair::DFS(Gate* source){
	source -> traversed = true;
	for(int i = 0; i < source -> input.size(); ++i){
   	if(!(source -> input[i] -> traversed)){
    		DFS(source -> input[i]);
    	   	}
      	}
	store.push_back(source);

}


void PotentialPair::cleanstore(){
	for(int i=0;i<store.size();i++)
		store[i]->traversed=false;
	store.clear();
}

void PotentialPair::Out(){
	cout<<endl<<"...........result............."<<endl;
	cout<<"pair size : "<<pair.size()<<endl;
	int count=0;	
	for(int i=0;i<pair.size();i++)
		count+=pair[i]->size();
	cout<<"total wire size : "<<count<<endl;
	cout<<"overflow pair number : "<<overflowpair.size()<<endl;
	for (int i=0;i<overflowpair.size();i++)
		cout<<overflowpair[i]<<endl;
	cout<<endl<<"...........result............."<<endl;

}
