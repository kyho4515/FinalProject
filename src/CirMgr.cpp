/* 6/28修正
	void CirMgr::DFSearch(Gate* source)
   7/14修正
	void CirMgr::DFSearch(Gate* source)	
*/

#include "CirMgr.h"
#include <cstring>
#include <queue>
#include "GateTool.h"
#include "GateTool.cpp"
bool CirMgr::ReadVerilog(string inputFilePath, int num){
	cout<<"start reading..."<<endl;
    ifstream inputFile(inputFilePath.c_str());
    constTrueGate = new ConstGate("1'b1", 0, true,~0);
    constFalseGate = new ConstGate("1'b0", 0, false,0);
    int count_id=0;
    int check_semi=0;
    string hold_gate;
    string hold_name;
    char check_first_semi='a';
    while(true){
        getline(inputFile,hold_name,' ');
        for (int i=0; i<hold_name.length(); i++) {
            if (hold_name[i]==';') {
                check_first_semi = hold_name[i];
                break;
            }
        }
        if(check_first_semi==';')
            break;
    }
    // 第一次遇到';'，開始讀input
		cout<<"read input"<<endl;
      bool check_semi_input=0;
      while (true) {
         getline(inputFile,hold_name,' ');
         for (int i=0; i<hold_name.length(); i++) {
             if(hold_name[i]==';'){
                check_semi = i;
                check_semi_input=1;
                break;
             }
         }
         if(check_semi_input==1)
             hold_name.erase(check_semi,hold_name.length()-1-check_semi+1);
         if (hold_name.length()>=2 && hold_name[0]==',') {
             hold_name.erase(0,2);
         }
         if(hold_name != "\n" && hold_name != "" && hold_name[0]!=',' && hold_name[check_semi]!=';'){
             Gate *a = new InputGate(hold_name,count_id);
             input.push_back(a);
             inputMap.insert(pair<string, Gate*>(hold_name, a));
             count_id++;
         }
                 
         if(check_semi_input==1)
             break;
      }
      // 開始讀ouput
      check_semi =0;
		count_id=0;
      bool check_semi_output=0;
      while (true) {
         getline(inputFile,hold_name,' ');
         for (int i=0; i<hold_name.length(); i++) {
             if(hold_name[i]==';'){
                check_semi = i;
                check_semi_output=1;
                break;
             }
         }
         if(check_semi_output==1)
             hold_name.erase(check_semi,hold_name.length()-1-check_semi+1);
         if (hold_name.length()>=2 && hold_name[0]==',') {
             hold_name.erase(0,2);
         }
         if(hold_name != "\n" && hold_name != "" && hold_name[0]!=',' && hold_name[check_semi]!=';'){
             Gate *a = new OutputGate(hold_name,count_id);
             output.push_back(a);
             outputMap.insert(pair<string, Gate*>(hold_name, a));
             count_id++;
         }
          
         if(check_semi_output==1)
             break;
     }
     // 開始讀wire
     check_semi=0;
     bool checkSemi=0;
     while (true) {
       checkSemi=0;
       getline(inputFile,hold_name,' ');
       for (int i=0; i<hold_name.length(); i++) {
         if(hold_name[i]==';'){
           checkSemi=1;
           check_semi = i;
           break;
         }
       }
       if(checkSemi==1){
         hold_gate = hold_name.substr(check_semi+1);
         hold_name.erase(check_semi,hold_name.length()-check_semi+1);
       }
       if (hold_name.length()>=2 && hold_name[0]==',') {
         hold_name.erase(0,2);
       }
       if(hold_name != "\n" && hold_name != "" && hold_name[0]!=',' && hold_name[check_semi]!=';'){
         Gate *a = new Wire(hold_name,count_id, num);
         wire.push_back((Wire*)a);
         wireMap.insert(pair<string, Gate*>(hold_name, a));
         count_id++;
       }
       if(checkSemi==1){
         if(!(hold_gate[0] == '\n' && hold_gate.substr(1) == "wire"))
           break;
       }
     } 
     int offset = -hold_gate.size();
     inputFile.seekg(offset, ios_base::cur);
     // 開始讀gate
     char buf [512];
     if(!inputFile){
       cout << "Connot find such file.";
       return false;
     }
     while (true) {
       inputFile.getline(buf, 512);
       while(buf[0] == '\0')
         inputFile.getline(buf, 512);
       char* ptr = strtok(buf, " (),;");
       string tmp = ptr;
       if(tmp == "endmodule")
         break;
       int mode = 0;
       Gate *a = NULL;
       while(ptr != NULL){
         tmp = ptr;
         if(mode == 0){
            if(tmp == "not")
              a = new BufGate(hold_gate, count_id, false);
            else if(tmp == "buf")
              a = new BufGate(hold_gate, count_id, true);
            else if(tmp == "and")
              a = new AndGate(hold_gate, count_id, true);
            else if(tmp == "nand")
              a = new AndGate(hold_gate, count_id, false);
            else if(tmp == "or")
              a = new OrGate(hold_gate, count_id, true);
            else if(tmp == "nor")
              a = new OrGate(hold_gate, count_id, false);
            else if(tmp == "xor")
              a = new XorGate(hold_gate, count_id, true);
            else if(tmp == "xnor")
              a = new XorGate(hold_gate, count_id, false);
            else
              cerr << "Doesn't support " << tmp << endl;
            ++count_id;
            ++mode;
        		        }
         else if(mode == 1){
           Gate* b = NULL;
           gmap::iterator it = wireMap.find(tmp);
           if(it != wireMap.end())
             b = it -> second;
           if(b == NULL){
             gmap::iterator it = outputMap.find(tmp);
             if(it != outputMap.end())
               b = it -> second; 
          			 }
           if(b == NULL)
             cerr << "read file error:connot find " << tmp << endl;
           b -> input.push_back(a);
           a -> output.push_back(b);
           ++mode;
         		}
         else {
           Gate* b = NULL;
           if(tmp == "1'b0"){
             b = constFalseGate;
          		      }
           else if(tmp == "1'b1"){
             b = constTrueGate;
                              }
           else{
             gmap::iterator it = wireMap.find(tmp);
             if(it != wireMap.end())
               b = it -> second;
             if(b == NULL){
                gmap::iterator it = inputMap.find(tmp);
            	 if(it != inputMap.end())
                b = it -> second;
           			  }
             if(b == NULL)
               cerr << "read file error:connot find " << tmp << endl;
           }
           a -> input.push_back(b);
           b -> output.push_back(a);
         }
         ptr = strtok(NULL, " (),;");
       }
       gate.push_back(a);
       gateMap.insert(pair<string, Gate*>(hold_name,a));
       count_id++;
		
    }
    
    cout << "Finishing reading file..." << endl;
    cout << "gate numbers: "<< gate.size() << endl;
    cout << "input numbers: "<< input.size() << endl;
    cout << "output numbers: " << output.size() << endl;
    cout << "wire numbers: " << wire.size() << endl << endl;
	 bool repeat;
	 bool full;
	//do DFS
    for(int i=0; i < output.size(); ++i){
      if(output[i] -> name != "\n"){
			 DFSearch(output[i]);
		 }
	   }
// 7/14 //
	//填入vector<Gate*> final_output
	for(int i=dfsList.size();i>0;i--){
		if(dfsList[i-1]->gateType!=Output && dfsList[i-1]->gateType!=Const && dfsList[i-1]->gateType!=Input){
			VectorGateAdd ori(dfsList[i-1]->final_output),out;
			for(int j=0;j<dfsList[i-1]->output.size();j++){
				out.update(dfsList[i-1]->output[j]->final_output);
				ori=ori+out;
			}
			dfsList[i-1]->final_output=ori.GetWholeOutput();
		}
	}
// 7/14 //
}



void CirMgr::DFSearch(Gate* source){
  int max = 0;
  for(int i = 0; i < source -> input.size(); ++i){
    if(source -> input[i] -> gateType == Input)
      source -> input[i] -> gateLevel = 0;
    if(!(source -> input[i] -> traversed)){
      source -> input[i] -> traversed = true;
      DFSearch(source -> input[i]);
    	   }
    if(max < source -> input[i] -> gateLevel)
      max = source -> input[i] -> gateLevel;
      }
  if(source -> gateType != Input)
    source -> gateLevel = max + 1;
// 7/14 //
	//填入vector<Gate*> total_input
	if(source->gateType!=Input && source->gateType!=Const){
		VectorGateAdd ori(source->total_input),inp;		
		for(int j=0;j<source->input.size();++j){
			  inp.update(source->input[j]->total_input);		  
				ori=ori+inp;
		}
		source->total_input=ori.GetWholeOutput();
	}
// 7/14 //
  dfsList.push_back(source);
}

void CirMgr::resetTraversed(){
  for(int i=0; i < dfsList.size(); ++i)
    dfsList[i] -> traversed = false;
}


void CirMgr::WriteOutputFile(const char* name){
	vector<Gate*> cutwire;
	fstream fout(name, ios::out);
	//write the first line	
	fout<<"module top(";
	for(int i=0;i<input.size();i++){
		fout<<input[i]->name<<",";	
	}
	for(int i=0;i<output.size();i++){
		if (i==output.size()-1)
			fout<<output[i]->name<<");"<<endl;
		else		
			fout<<output[i]->name<<",";
	}
	
	//write input
	fout<<"input ";
	for(int i=0;i<input.size();i++){
		if (i==input.size()-1)
			fout<<input[i]->name<<";"<<endl;
		else		
			fout<<input[i]->name<<",";
	}
	//write output
	fout<<"output ";
	for(int i=0;i<output.size();i++){
		if(i==output.size()-1)
			fout<<output[i]->name<<";"<<endl;
		else 
			fout<<output[i]->name<<",";
	}

	//write wire
	fout<<"wire ";
	for (int i=0;i<wire.size();i++){
		if(((Wire*)wire[i])->isCut())
			cutwire.push_back(wire[i]);		
		if(i==wire.size()-1)
			fout<<wire[i]->name<<";"<<endl;
		else 
			fout<<wire[i]->name<<",";
	}

	//write cut wire
	if(cutwire.size()!=0){
		fout<<"wire ";
		for (int i=0;i<cutwire.size();i++){
			if(i==cutwire.size()-1)
				fout<<cutwire[i]->name<<cutwire[i]->name<<";"<<endl;
			else 
				fout<<cutwire[i]->name<<cutwire[i]->name<<",";
		}
	}

	//write gate
	for (int i=0;i<gate.size();i++){
		//gate type		
		switch(gate[i]->gateType){
			case And:{
				if (gate[i]->phase)				
					fout<<"and (";
				else
					fout<<"nand (";
				break;
			}
			case Or:{
				if (gate[i]->phase)
					fout<<"or (";
				else 
					fout<<"nor (";
				break;
			}
			case Xor:{
				if(gate[i]->phase)
					fout<<"xor (";
				else 
					fout<<"xnor (";
				break;
			}
			case Buf:{
				if (gate[i]->phase)
					fout<<"buf (";
				else
					fout<<"not (";
				break;
			}
			default:
				cout<<"case not concern"<<endl;
		}
		//output wire
		for(int n=0;n<gate[i]->output.size();n++){
			fout<<gate[i]->output[n]->name<<",";
		}		
		//input wire
		for(int n=0;n<gate[i]->input.size();n++){
			if(((Wire*)(gate[i]->input[n]))->isCut())
				fout<<gate[i]->input[n]->name<<gate[i]->input[n]->name;
			else 
				fout<<gate[i]->input[n]->name;
			
			if(n==gate[i]->input.size()-1)
				fout<<");"<<endl;
			else 
				fout<<",";
		}
	}		

 	//write cut
	if(cutwire.size()!=0){
		for(int i=0;i<cutwire.size();i++){
			fout<<"_cut cut_"<<((Wire*)cutwire[i])->cutNum;
      if(cutwire[i]->phase == false)
        fout << "_bar";
      fout <<"_1"<<" ("<<cutwire[i]->name<<cutwire[i]->name<<","<<cutwire[i]->name<<")"<<endl;
		}
	}
	//end
	fout<<"endmodule"<<endl;
}



Gate* CirMgr::Getoutput(int n){
	return output[n];
}

 int CirMgr::GetoutputSize(){
	return output.size();
} 
