/*
 *  getoturepcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 4/6/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "getoturepcommand.h"

#include "inputdata.h"
#include "read/readphylip.h"
#include "read/readcolumn.h"
#include "datastructures/optimatrix.h"
#include "commands/getseqscommand.h"

//********************************************************************************************************************
//sorts lowest to highest
inline bool compareName(repStruct left, repStruct right){
	return (left.name < right.name);	
}
//********************************************************************************************************************
//sorts lowest to highest
inline bool compareBin(repStruct left, repStruct right){
	return (left.simpleBin < right.simpleBin);
}
//********************************************************************************************************************
//sorts lowest to highest
inline bool compareSize(repStruct left, repStruct right){
	return (left.size < right.size);	
}
//********************************************************************************************************************
//sorts lowest to highest
inline bool compareGroup(repStruct left, repStruct right){
	return (left.group < right.group);	
}

//**********************************************************************************************************************
vector<string> GetOTURepCommand::setParameters(){	
	try {
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none","name",false,true, true); parameters.push_back(plist);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","fasta",false,false, true); parameters.push_back(pfasta);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "none","",false,false, true); parameters.push_back(pphylip);
        CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "ColumnName","",false,false, true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "ColumnName","count",false,false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none","",false,false, true); parameters.push_back(pgroup);
		CommandParameter pcolumn("column", "InputTypes", "", "", "PhylipColumn", "PhylipColumn", "ColumnName","",false,false, true); parameters.push_back(pcolumn);
		CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "","",false,false); parameters.push_back(pcutoff);
		CommandParameter pprecision("precision", "Number", "", "100", "", "", "","",false,false); parameters.push_back(pprecision);
		CommandParameter pweighted("weighted", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pweighted);
		CommandParameter psorted("sorted", "Multiple", "none-name-bin-size-group", "none", "", "", "","",false,false); parameters.push_back(psorted);
        CommandParameter pmethod("method", "Multiple", "distance-abundance", "distance", "", "", "","",false,false); parameters.push_back(pmethod);
        CommandParameter prename("rename", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(prename);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false; allLines = true;
        
        vector<string> tempOutNames;
        outputTypes["fasta"] = tempOutNames;
        outputTypes["name"] = tempOutNames;
        outputTypes["count"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOTURepCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The get.oturep command parameters are phylip, column, list, fasta, name, group, count, large, weighted, cutoff, precision, groups, sorted, method and rename.  The list parameter is required, as well as phylip or column and name if you are using method=distance. If method=abundance a name or count file is required.\n";
		helpString += "The rename parameter allows you to indicate you want the OTU label to replace the representative sequence name. Default=F. \n";
		helpString += "The phylip or column parameter is required for method=distance, but only one may be used.  If you use a column file the name or count filename is required. \n";
        helpString += "The method parameter allows you to select the method of selecting the representative sequence. Choices are distance and abundance.  The distance method finds the sequence with the largest number of close sequences in the OTU. If tie occurs, a sequence is randomly selected from the ties.  The abundance method chooses the most abundant sequence in the OTU as the representative.\n";
		helpString += "If you do not provide a cutoff value 0.03 is assumed. If you do not provide a precision value then 100 is assumed.\n";
        helpString += "Multiple cutoffs can be entered as follows cutoff=0.01-0.03. \n";
		helpString += "The get.oturep command should be in the following format: get.oturep(phylip=yourDistanceMatrix, fasta=yourFastaFile, list=yourListFile, name=yourNamesFile, group=yourGroupFile).\n";
		helpString += "Example get.oturep(phylip=amazon.dist, fasta=amazon.fasta, list=amazon.fn.list, group=amazon.groups).\n";
		helpString += "The sorted parameter allows you to indicate you want the output sorted. You can sort by sequence name, bin number, bin size or group. The default is no sorting, but your options are name, number, size, or group.\n";
		helpString += "The weighted parameter allows you to indicate that want to find the weighted representative. You must provide a namesfile to set weighted to true.  The default value is false.\n";
		helpString += "The representative is found by selecting the sequence with the most \"close\" sequences in the OTU. If a tie occurs a seqeunce is chosen at random from the ties.\n";
		helpString += "For weighted = false, mothur assumes the distance file contains only unique sequences, the list file may contain all sequences, but only the uniques are considered to become the representative. If your distance file contains all the sequences it would become weighted=true.\n";
		helpString += "For weighted = true, mothur assumes the distance file contains only unique sequences, the list file must contain all sequences, all sequences are considered to become the representative, but unique name will be used in the output for consistency.\n";
		helpString += "If your distance file contains all the sequence and you do not provide a name file, the weighted representative will be given, unless your listfile is unique. If you provide a namefile, then you can select weighted or unweighted.\n";
		helpString += "The group parameter allows you provide a group file.\n";
		helpString += "The groups parameter allows you to indicate that you want representative sequences for each group specified for each OTU, group name should be separated by dashes. ex. groups=A-B-C.\n";
		helpString += "The get.oturep command outputs a .fastarep and .rep.names file for each distance you specify, selecting one OTU representative for each bin.\n";
		helpString += "If you provide a groupfile, then it also appends the names of the groups present in that bin.\n";
		
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOTURepCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fasta") {  pattern = "[filename],[tag],rep.fasta-[filename],[tag],[group],rep.fasta"; } 
        else if (type == "name") {  pattern = "[filename],[tag],rep.names-[filename],[tag],[group],rep.names"; } 
        else if (type == "count") {  pattern = "[filename],count_table-[filename],[tag],rep.count_table-[filename],[tag],[group],rep.count_table"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "GetOTURepCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
GetOTURepCommand::GetOTURepCommand(string option) : Command()  {
	try{
		//allow user to run help
		if (option == "help") { 
			help(); abort = true; calledHelp = true;
		}else if(option == "citation") { citation(); abort = true; calledHelp = true;
		} else if(option == "category") {  abort = true; calledHelp = true;  }
        else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			
			
			//check for required parameters
			fastafile = validParameter.validFile(parameters, "fasta");
			if (fastafile == "not found") { fastafile = ""; }
			else if (fastafile == "not open") { abort = true; }	
			else { current->setFastaFile(fastafile); }
		
			listfile = validParameter.validFile(parameters, "list");
			if (listfile == "not found") { 			
				listfile = current->getListFile(); 
				if (listfile != "") { m->mothurOut("Using " + listfile + " as input file for the list parameter.\n");  }
				else { 	m->mothurOut("You have no current list file and the list parameter is required.\n");  abort = true; }
			}
			else if (listfile == "not open") { abort = true; }	
			else { current->setListFile(listfile); }
			
			phylipfile = validParameter.validFile(parameters, "phylip");
			if (phylipfile == "not found") { phylipfile = "";  }
			else if (phylipfile == "not open") { abort = true; }	
			else { distFile = phylipfile; format = "phylip"; current->setPhylipFile(phylipfile);   }
			
			columnfile = validParameter.validFile(parameters, "column");
			if (columnfile == "not found") { columnfile = ""; }
			else if (columnfile == "not open") { abort = true; }	
			else { distFile = columnfile; format = "column";  current->setColumnFile(columnfile); }
			
			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { abort = true; }	
			else if (namefile == "not found") { namefile = ""; }
			else { current->setNameFile(namefile); }
            
            hasGroups = false;
            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not found") { countfile =  "";   }
			else if (countfile == "not open") { abort = true; countfile =  ""; }	
			else {   
                current->setCountFile(countfile); 
                ct.readTable(countfile, true, false);
                if (ct.hasGroupInfo()) { hasGroups = true; }
            }
            
            groupfile = validParameter.validFile(parameters, "group");
			if (groupfile == "not open") { groupfile = ""; abort = true; }
			else if (groupfile == "not found") { groupfile = ""; }
			else { current->setGroupFile(groupfile); }
			
            method = validParameter.valid(parameters, "method");		if (method == "not found"){	method = "distance";	}
			if ((method != "distance") && (method != "abundance")) {
				m->mothurOut(method + " is not a valid option for the method parameter. The only options are: distance and abundance, aborting.\n");  abort = true;
			}
            
            if (method == "distance") {
                if ((phylipfile == "") && (columnfile == "")) { //is there are current file available for either of these?
                    //give priority to column, then phylip
                    columnfile = current->getColumnFile();
                    if (columnfile != "") {  distFile = columnfile; format = "column"; m->mothurOut("Using " + columnfile + " as input file for the column parameter.\n");  }
                    else {
                        phylipfile = current->getPhylipFile();
                        if (phylipfile != "") {  distFile = phylipfile; format = "phylip"; m->mothurOut("Using " + phylipfile + " as input file for the phylip parameter.\n");  }
                        else {
                            m->mothurOut("No valid current files. You must provide a phylip or column file before you can use the get.oturep command.\n"); 
                            abort = true;
                        }
                    }
                }else if ((phylipfile != "") && (columnfile != "")) { m->mothurOut("When executing a get.oturep command you must enter ONLY ONE of the following: phylip or column.\n");  abort = true; }
                
                if (columnfile != "") {
                    if ((namefile == "") && (countfile == "")) {
                        namefile = current->getNameFile();
                        if (namefile != "") {  m->mothurOut("Using " + namefile + " as input file for the name parameter.\n");  }
                        else {
                            countfile = current->getCountFile();
                            if (countfile != "") {  m->mothurOut("Using " + countfile + " as input file for the count parameter.\n");  }
                            else {
                                m->mothurOut("You need to provide a namefile or countfile if you are going to use the column format.\n"); 
                                abort = true; 
                            }	
                        }	
                    }
                }
            }else if (method == "abundance") {
                if ((namefile == "") && (countfile == "")) {
					namefile = current->getNameFile();
					if (namefile != "") {  m->mothurOut("Using " + namefile + " as input file for the name parameter.\n");  }
					else {
						countfile = current->getCountFile();
                        if (countfile != "") {  m->mothurOut("Using " + countfile + " as input file for the count parameter.\n");  }
                        else {
                            m->mothurOut("You need to provide a namefile or countfile if you are going to use the abundance method.\n"); 
                            abort = true;
                        }
					}
				}
                if ((phylipfile != "") || (columnfile != "")) {
                    m->mothurOut("[WARNING]: A phylip or column file is not needed to use the abundance method, ignoring.\n"); 
                    phylipfile = ""; columnfile = "";
                }
            }
            
            if ((namefile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: name or count.\n");  abort = true;
            }
            
            if ((groupfile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: group or count.\n");  abort=true;
            }

            cutoffSet = false;
            string temp = validParameter.valid(parameters, "cutoff");
            if (temp == "not found") {  temp = "0.03"; }
            else { cutoffSet = true; }
            
            int pos = temp.find('-');
            if (pos != string::npos) { //multiple cutoffs given
                util.splitAtDash(temp, cutoffs);  temp = *cutoffs.begin();
            }else {     cutoffs.insert(temp);  }
            util.mothurConvert(temp, cutoff);
						
			sorted = validParameter.valid(parameters, "sorted");		if (sorted == "not found"){	sorted = "";	}
			if (sorted == "none") { sorted=""; }
			if ((sorted != "") && (sorted != "name") && (sorted != "bin") && (sorted != "size") && (sorted != "group")) {
				m->mothurOut(sorted + " is not a valid option for the sorted parameter. The only options are: name, bin, size and group. I will not sort.\n"); 
				sorted = "";
			}
            
			if ((sorted == "group") && ((groupfile == "")&& !hasGroups)) {
				m->mothurOut("You must provide a groupfile or have a count file with group info to sort by group. I will not sort.\n"); 
				sorted = "";
			}
			
			groups = validParameter.valid(parameters, "groups");			
			if (groups == "not found") { groups = ""; }
			else { 
				if ((groupfile == "") && (!hasGroups)) {
					m->mothurOut("You must provide a groupfile to use groups.\n");
					abort = true;
				}else { 
					util.splitAtDash(groups, Groups);
                    if (Groups.size() != 0) { if (Groups[0]== "all") { Groups.clear(); } }
				}
			}
			
			temp = validParameter.valid(parameters, "weighted");		if (temp == "not found") {	 temp = "f"; 	} weighted = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "rename");		if (temp == "not found") {	 temp = "f"; 	} rename = util.isTrue(temp);
            
            if ((rename) && (namefile != "")) {
                m->mothurOut("[WARNING]: You cannot use the rename option when you provide a namesfile due to downstream issues. Setting rename to false.\n"); rename = false;
            }
			
			if ((weighted) && (namefile == "")) { m->mothurOut("[ERROR]: You cannot set weighted to true unless you provide a namesfile.\n"); abort = true; }
			
			temp = validParameter.valid(parameters, "precision");			if (temp == "not found") { temp = "100"; } util.mothurConvert(temp, precision);
			
            matrix = nullptr;
			
		}
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "GetOTURepCommand");
		exit(1);
	}
}

//**********************************************************************************************************************

int GetOTURepCommand::execute(){
	try {
	
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        if (method == "distance") {
            if ((namefile != "") && (groupfile != "")) { createCount();  } //create count file for simplicity
        }
       
        if (namefile != "") { nameMap = util.readNames(namefile); }
        
        if (m->getControl_pressed()) { return 0; }
        
        GroupMap groupMap;
        if (groupfile != "") {
            //read in group map info.
            GroupMap groupMap(groupfile);
            int error = groupMap.readMap();
            if (error == 1) { m->mothurOut("Error reading your groupfile. Proceeding without groupfile.\n");  groupfile = "";  }
        }
        
        if (!cutoffSet) {
            InputData input(listfile, "list", Groups);
            ListVector* list = input.getListVector();
            string lastLabel = list->getLabel();
            m->mothurOut("You did not provide a label, using " + lastLabel + ".\n");
            if (lastLabel == "unique") { cutoff = 0.0; }
            
            if (method == "distance") { readDist(); }
            process(list, groupMap); delete list;
        }
        else { //multiple cutoffs
            for (set<string>::iterator it = cutoffs.begin(); it != cutoffs.end(); it++) {
                if (*it == "unique") { cutoff = 0.0; }
                else { util.mothurConvert(*it, cutoff); }
                
                if (method == "distance") { readDist(); }
                
                InputData input(listfile, "list", Groups);
                ListVector* list = input.getListVector(*it);
                if (list != nullptr) {
                    string lastLabel = list->getLabel();
                
                    process(list, groupMap); delete list;
                }
            }
        }
        
        //handles multiple labels
        if (fastafile != "") {
            //read fastafile
            FastaMap fasta;
            fasta.readFastaFile(fastafile);
            
            //if user gave a namesfile then use it
            if (namefile != "") {	readNamesFile(fasta);	}
            
            //output create and output the .rep.fasta files
            map<string, string>::iterator itNameFile;
            for (itNameFile = outputNameFiles.begin(); itNameFile != outputNameFiles.end(); itNameFile++) {
                processFastaNames(itNameFile->first, itNameFile->second, fasta, groupMap);
            }
        }else {
            //output create and output the .rep.fasta files
            map<string, string>::iterator itNameFile;
            for (itNameFile = outputNameFiles.begin(); itNameFile != outputNameFiles.end(); itNameFile++) {
                processNames(itNameFile->first, itNameFile->second);
            }
        }
        
		if (m->getControl_pressed()) {  return 0; }
		
		//set fasta file as new current fastafile - use first one??
		string currentName = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFastaFile(currentName); }
		}
		
		itTypes = outputTypes.find("name");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setNameFile(currentName); }
		}
        
        itTypes = outputTypes.find("count");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setCountFile(currentName); }
		}
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
int GetOTURepCommand::readDist() {
	try {
        string nameOrCount = "";
        string thisNamefile = "";
        map<string, int> counts;
        if (countfile != "") { nameOrCount = "count"; thisNamefile = countfile; CountTable ct; ct.readTable(countfile, false, false); counts = ct.getNameMap(); }
        else if (namefile != "") { nameOrCount = "name"; thisNamefile = namefile; }
        
        string distfile = columnfile;
        if (format == "phylip") { distfile = phylipfile; }
        
        if (matrix != nullptr) { delete matrix; }
        
        matrix = new OptiMatrix(distfile, thisNamefile, nameOrCount, format, cutoff, false);
        
        if (m->getControl_pressed()) { return 0; }
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "readDist");
		exit(1);
	}
}
//**********************************************************************************************************************
void GetOTURepCommand::createCount() {
    try {
        CountTable ct; ct.createTable(namefile, groupfile, nullVector);
        
        if (outputdir == "") { outputdir = util.hasPath(namefile); }
        map<string, string> variables; variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(namefile));
        countfile = getOutputFileName("count", variables);
        
        ct.printCompressedTable(countfile);
        
        current->setCountFile(countfile);
        if (ct.hasGroupInfo()) { hasGroups = true; }
        
        vector<string> uniqueNames = ct.getNamesOfSeqs();
        util.printAccnos("temp.accnos", uniqueNames);
        
        string inputString = "list=" + listfile + ", accnos=temp.accnos";
        
        m->mothurOut("/******************************************/\n");
        m->mothurOut("\nRunning command: get.seqs(" + inputString + ")\n");
        current->setMothurCalling(true);
        
        Command* getSeqsCommand = new GetSeqsCommand(inputString);
        getSeqsCommand->execute();
        
        string templistfile = getSeqsCommand->getOutputFiles()["list"][0];
        string newName = util.getRootName(listfile) + "unique.list";
        util.renameFile(templistfile, newName);  listfile = newName;
        namefile = ""; groupfile = ""; util.mothurRemove("temp.accnos");
        
        delete getSeqsCommand;
        current->setMothurCalling(false);
        
        m->mothurOut("/******************************************/\n");
    }
    catch(exception& e) {
        m->errorOut(e, "GetOTURepCommand", "readDist");
        exit(1);
    }
}
//**********************************************************************************************************************
void GetOTURepCommand::readNamesFile(FastaMap& fasta) {
	try {
		vector<string> dupNames;
        ifstream in; util.openInputFile(namefile, in);
		
		string name, names, sequence;
	
		while(!in.eof()){
			in >> name;			//read from first column  A
			in >> names;		//read from second column  A,B,C,D
			
			dupNames.clear();
			
			//parse names into vector
			util.splitAtComma(names, dupNames);
			
			//store names in fasta map
            sequence = fasta.getSequence(name);
			for (int i = 0; i < dupNames.size(); i++) {
                fasta.push_back(dupNames[i], sequence);
			}
		
			gobble(in);
		}
		in.close();

	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "readNamesFile");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOTURepCommand::findRepAbund(vector<string> names, string group) {
	try{
        vector<string> reps;
        string rep = "notFound";
        Utils util;
    
        if (m->getDebug()) { m->mothurOut("[DEBUG]: group=" + group + " names.size() = " + toString(names.size()) + " " + names[0] + "\n"); }
        
        if ((names.size() == 1)) {
            return names[0];
        }else{
            //fill seqIndex and initialize sums
            int maxAbund = 0;
            for (int i = 0; i < names.size(); i++) {
                
                if (m->getControl_pressed()) { return "control"; }
                
                if (countfile != "") {  //if countfile is not blank then we can assume the list file contains only uniques, otherwise we assume list file contains everyone.
                    int numRep = 0;
                    if (group != "") {  numRep = ct.getGroupCount(names[i], group);  }
                    else { numRep = ct.getNumSeqs(names[i]);  }
                    if (numRep > maxAbund) {
                        reps.clear();
                        reps.push_back(names[i]);
                        maxAbund = numRep;
                    }else if(numRep == maxAbund) { //tie
                        reps.push_back(names[i]);
                    }
                }else { //name file used, we assume list file contains all sequences
                    map<string, int>::iterator itNameMap = nameMap.find(names[i]);
                    if (itNameMap == nameMap.end()) {} //assume that this sequence is not a unique
                    else {
                        if (itNameMap->second > maxAbund) {
                            reps.clear();
                            reps.push_back(names[i]);
                            maxAbund = itNameMap->second;
                        }else if(itNameMap->second == maxAbund) { //tie
                            reps.push_back(names[i]);
                        }
                    }
                }
            }
            
            if (reps.size() == 0) { m->mothurOut("[ERROR]: no rep found, file mismatch?? Quitting.\n"); m->setControl_pressed(true); }
            else if (reps.size() == 1) { rep = reps[0]; }
            else { //tie
                int maxIndex = reps.size()-1;
                int index = util.getRandomIndex(maxIndex);
                rep = reps[index];
            }
        }
        
        return rep;
    }
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "findRepAbund");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetOTURepCommand::findRep(vector<string> names, map<string, long long>& matrixNameIndexes, string group) {
	try{
        //if using abundance 
        if (method == "abundance") { return (findRepAbund(names, group)); }
        else { //find rep based on distance
            
            // if only 1 sequence in bin or processing the "unique" label, then
            // the first sequence of the OTU is the representative one
            if ((names.size() == 1)) {
                return names[0];
            }else{
                //unique sequence with greatest number of "close" seqs in the OTU
                vector<long long> binTranslated;
                
                //fill seqIndex and initialize sums
                for (size_t i = 0; i < names.size(); i++) {
                    
                    if (m->getControl_pressed()) { return names[0]; }
                    
                    map<string, long long>::iterator itNameIndex = matrixNameIndexes.find(names[i]);
                    
                    if (itNameIndex == matrixNameIndexes.end()) { } //no distances in matrix, or not unique
                    else { //you have a distance in the matrix, do we need to inflate the otu for weighted option?
                        long long matrixIndex = itNameIndex->second;
                        if (weighted) {
                            binTranslated.push_back(matrixIndex);
                            int numRep = 0;
                            if (countfile != "") {  //if countfile is not blank then we can assume the list file contains only uniques, otherwise we assume list file contains everyone.
                                if (group != "") {  numRep = ct.getGroupCount(names[i], group);  }
                                else { numRep = ct.getNumSeqs(names[i]);  }
                            }else if (namefile != "") {
                                map<string, int>::iterator itNameFile = nameMap.find(names[i]);
                                
                                if (itNameFile == nameMap.end()) { m->mothurOut("[ERROR]: " + names[i] + " is not in your namefile, please correct.\n");  m->setControl_pressed(true); }
                                else{ numRep = itNameFile->second; }
                            }
                            for (int j = 1; j < numRep; j++) { binTranslated.push_back(matrixIndex); } //inflate redundants
                        }else {
                            if (namefile == "") { binTranslated.push_back(matrixIndex);  } //will be unique and in matrix, possible rep
                            else {//name file, no group because if group file was present we could be usingthe count file
                                map<string, int>::iterator itNameFile = nameMap.find(names[i]);
                                
                                if (itNameFile == nameMap.end()) { m->mothurOut("[ERROR]: " + names[i] + " is not in your namefile, please correct.\n");  m->setControl_pressed(true); }
                                else{ binTranslated.push_back(matrixIndex); }
                            }
                        }
                    }
                }
                
                //True Negative - far, cluster apart
                //True Positive - close, cluster together
                //False Negative - close, cluster apart
                //False Positve - far, cluster together
                vector<int> numCloseInBin; numCloseInBin.resize(binTranslated.size(), 0);
                
                for (size_t i=0; i < binTranslated.size(); i++) {
                    if (m->getControl_pressed()) {  return  "control"; }
                    
                    for (size_t j = 0; j < i; j++) {
                        if (matrix->isClose(binTranslated[i], binTranslated[j])) {
                            numCloseInBin[i]++; numCloseInBin[j]++;
                        }else if (binTranslated[i] == binTranslated[j]) { //you have inflated the otu and need to count this as a close match
                            numCloseInBin[i]++; numCloseInBin[j]++;
                        }
                    }
                }
                
                string repName = "";
                if (binTranslated.size() == 0) { repName = names[0]; } //when names file is used singleton OTUs may contain multiple read names, but no dists in matrix
                else {
                    long long minIndex = binTranslated[0]; int min = numCloseInBin[0];
                    vector<long long> ties; ties.push_back(binTranslated[0]);
                    for (size_t i=1; i < numCloseInBin.size(); i++) {
                        if (m->getControl_pressed()) {  return  "control"; }
                        if (numCloseInBin[i] > min) {
                            ties.clear();
                            min = numCloseInBin[i];
                            minIndex = binTranslated[i];
                            ties.push_back(binTranslated[i]);
                        }else if (numCloseInBin[i] == min) { ties.push_back(binTranslated[i]); }
                    }
                    
                    if (ties.size() > 0) {
                        long long numTies = ties.size()-1;
                        long long randomIndex = util.getRandomIndex(numTies);
                        repName = matrix->getName(ties[randomIndex]);
                    }else { repName = matrix->getName(minIndex); }
                    
                    if (namefile != "") {
                        vector<string> redundNames;
                        util.splitAtComma(repName, redundNames);
                        repName = redundNames[0];
                    }
                }
                return repName;
            }
        }
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "FindRep");
		exit(1);
	}
}

//**********************************************************************************************************************
int GetOTURepCommand::process(ListVector* processList, GroupMap& groupMap) {
	try{
        
        m->mothurOut(processList->getLabel() + "\t" + toString(processList->getNumBins()) + "\n");
        
		string name, sequence;
		string nameRep;

		//create output file
		if (outputdir == "") { outputdir += util.hasPath(listfile); }
				
		ofstream newNamesOutput;
		string outputNamesFile;
		map<string, string> files; //group -> filenameAW
		
        map<string, string> variables; 
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(listfile));
        
		if (Groups.size() == 0) { //you don't want to use groups
            variables["[tag]"] = processList->getLabel();
            if (countfile == "") { 
                outputNamesFile = getOutputFileName("name", variables);
                outputNames.push_back(outputNamesFile); outputTypes["name"].push_back(outputNamesFile); 
            }else {
                outputNamesFile = getOutputFileName("count", variables);
                outputNames.push_back(outputNamesFile); outputTypes["count"].push_back(outputNamesFile); 
            }
			outputNameFiles[outputNamesFile] = processList->getLabel();
            util.openOutputFile(outputNamesFile, newNamesOutput);
            newNamesOutput << "noGroup" << endl;
		}else{ //you want to use groups
			for (int i=0; i<Groups.size(); i++) {
                variables["[tag]"] = processList->getLabel();
                variables["[group]"] = Groups[i];
				outputNamesFile = outputdir + util.getRootName(util.getSimpleName(listfile)) + processList->getLabel() + "." + Groups[i] + ".";
                if (countfile == "") { 
                    outputNamesFile = getOutputFileName("name", variables);
                    outputNames.push_back(outputNamesFile); outputTypes["name"].push_back(outputNamesFile); 
                }else {
                    outputNamesFile = getOutputFileName("count", variables);
                    outputNames.push_back(outputNamesFile); outputTypes["count"].push_back(outputNamesFile); 
                }
				files[Groups[i]] = outputNamesFile;
                ofstream temp;
				util.openOutputFile(outputNamesFile, temp);
                temp << Groups[i] << endl; temp.close();
				outputNameFiles[outputNamesFile] = processList->getLabel() + "." + Groups[i];
			}
		}
		
        map<string, long long> matrixNameIndexes;
        if (method != "abundance") {  matrix->getNameIndexMap(); } //maps unique names to index in matrix
        
		//for each bin in the list vector
        vector<string> binLabels = processList->getLabels();
		for (int i = 0; i < processList->getNumBins(); i++) {
        
            
			if (m->getControl_pressed()) { out.close(); if (Groups.size() == 0) { newNamesOutput.close(); } return 0; }
			
			string temp = processList->get(i);
			vector<string> namesInBin;
			util.splitAtComma(temp, namesInBin);
			
			if (Groups.size() == 0) {
				nameRep = findRep(namesInBin, matrixNameIndexes, "");
				newNamesOutput << binLabels[i] << '\t' << nameRep << '\t';
                
                //put rep at first position in names line
                string outputString = nameRep + ",";
                for (int k=0; k<namesInBin.size()-1; k++) {//output list of names in this otu
                    if (namesInBin[k] != nameRep) { outputString += namesInBin[k] + ","; }
                }
                
                //output last name
                if (namesInBin[namesInBin.size()-1] != nameRep) { outputString += namesInBin[namesInBin.size()-1]; }
                
                if (outputString[outputString.length()-1] == ',') { //rip off comma
                    outputString = outputString.substr(0, outputString.length()-1);
                }
                newNamesOutput << outputString << endl;
			}else{
				map<string, vector<string> > NamesInGroup;
				for (int j=0; j<Groups.size(); j++) { //initialize groups
					NamesInGroup[Groups[j]].resize(0);
				}
				
				for (int j=0; j<namesInBin.size(); j++) {
                    if (groupfile != "") {
                        string thisgroup = groupMap.getGroup(namesInBin[j]);
                        if (thisgroup == "not found") { m->mothurOut(namesInBin[j] + " is not in your groupfile, please correct.\n");  m->setControl_pressed(true); }
                        
                        //add this name to correct group
                        if (util.inUsersGroups(thisgroup, Groups)) { NamesInGroup[thisgroup].push_back(namesInBin[j]);  }
                    }else {
                        vector<string> thisSeqsGroups = ct.getGroups(namesInBin[j]);
                        for (int k = 0; k < thisSeqsGroups.size(); k++) {
                            if (util.inUsersGroups(thisSeqsGroups[k], Groups)) { NamesInGroup[thisSeqsGroups[k]].push_back(namesInBin[j]);  }
                        }
                    }
				}
				
				//get rep for each group in otu
				for (int j=0; j<Groups.size(); j++) {
					if (NamesInGroup[Groups[j]].size() != 0) { //are there members from this group in this otu?
						//get rep for each group
						nameRep = findRep(NamesInGroup[Groups[j]], matrixNameIndexes, Groups[j]);
						
						//output group rep and other members of this group
                        ofstream outGroup;
                        util.openOutputFileAppend(files[Groups[j]], outGroup);
						outGroup << binLabels[i] << '\t' << nameRep << '\t';
						
                        //put rep at first position in names line
                        string outputString = nameRep + ",";
                        for (int k=0; k<NamesInGroup[Groups[j]].size()-1; k++) {//output list of names in this otu from this group
                            if (NamesInGroup[Groups[j]][k] != nameRep) { outputString +=  NamesInGroup[Groups[j]][k] + ","; }
                        }
                        
                        //output last name
                        if (NamesInGroup[Groups[j]][NamesInGroup[Groups[j]].size()-1] != nameRep) { outputString += NamesInGroup[Groups[j]][NamesInGroup[Groups[j]].size()-1]; }
                        
                        if (outputString[outputString.length()-1] == ',') { //rip off comma
                            outputString = outputString.substr(0, outputString.length()-1);
                        }
                        outGroup << outputString << endl; outGroup.close();
					}
				}
			}
		}
		
        //you don't want to use groups
		if (Groups.size() == 0) { newNamesOutput.close(); }
		
		return 0;

	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "process");
		exit(1);
	}
}
//**********************************************************************************************************************
int GetOTURepCommand::processFastaNames(string filename, string label, FastaMap& fasta, GroupMap& groupMap) {
	try{

		//create output file
		if (outputdir == "") { outputdir += util.hasPath(listfile); }
        map<string, string> variables; 
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(listfile));
        variables["[tag]"] = label;
		string outputFileName = getOutputFileName("fasta",variables);
		util.openOutputFile(outputFileName, out);
		vector<repStruct> reps;
		outputNames.push_back(outputFileName); outputTypes["fasta"].push_back(outputFileName);
		
		ofstream out2;
		string tempNameFile = filename + ".temp";
		util.openOutputFile(tempNameFile, out2);
            
		ifstream in; util.openInputFile(filename, in);
		
        string tempGroup = "";
        in >> tempGroup; gobble(in);
        
        CountTable thisCt;
        if (countfile != "") {
            thisCt.readTable(countfile, true, false);
            if (tempGroup != "noGroup") { out2 << "Representative_Sequence\ttotal\t" << tempGroup << endl; }
        }
    
        int thistotal = 0;
		while (!in.eof()) {
			string rep, binnames, binLabel;
			in >> binLabel >> rep >> binnames; gobble(in);
            
            string repName = rep;
            if (rename) { repName = binLabel; }
			
			vector<string> names;
			util.splitAtComma(binnames, names);
			int binsize = names.size();
            
            if (countfile == "") { out2 << repName << '\t' << binnames << endl; }
            else {
                if (tempGroup == "noGroup") {
                    for (int j = 0; j < names.size(); j++) {
                        if (names[j] != rep) { thisCt.mergeCounts(rep, names[j]); }
                    }
                    binsize = thisCt.getNumSeqs(rep);
                    if (rename) { thisCt.renameSeq(rep, repName); }
                }else {
                    int total = 0; 
                    for (int j = 0; j < names.size(); j++) {  total += thisCt.getGroupCount(names[j], tempGroup);  }
                    out2 << repName << '\t' << total << '\t' << total << endl;
                    binsize = total;
                }
            }
			thistotal += binsize;
			//if you have a groupfile
			string group = "";
            map<string, string> groups;
            map<string, string>::iterator groupIt;
			if (groupfile != "") {
				//find the groups that are in this bin
				for (int i = 0; i < names.size(); i++) {
                    string groupName = groupMap.getGroup(names[i]);
					if (groupName == "not found") {  
						m->mothurOut(names[i] + " is missing from your group file. Please correct.\n");
						groupError = true;
					} else {
						groups[groupName] = groupName;
					}
				}
				
				//turn the groups into a string
				for (groupIt = groups.begin(); groupIt != groups.end(); groupIt++) {
					group += groupIt->first + "-";
				}
				//rip off last dash
				group = group.substr(0, group.length()-1);
			}else if (hasGroups) {
                map<string, string> groups;
                for (int i = 0; i < names.size(); i++) {
                    vector<string> thisSeqsGroups = ct.getGroups(names[i]);
                    for (int j = 0; j < thisSeqsGroups.size(); j++) { groups[thisSeqsGroups[j]] = thisSeqsGroups[j]; }
                }
                //turn the groups into a string
				for (groupIt = groups.begin(); groupIt != groups.end(); groupIt++) {
					group += groupIt->first + "-";
				}
				//rip off last dash
				group = group.substr(0, group.length()-1);
            }
            else{ group = ""; }

			
			//print out name and sequence for that bin
            string sequence = fasta.getSequence(rep);

			if (sequence != "not found") {
				if (sorted == "") { //print them out
					repName = repName + "\t" + binLabel;
					repName = repName + "|" + toString(binsize);
					if (group != "") {
						repName = repName + "|" + group;
					}
					out << ">" << repName << endl;
					out << sequence << endl;
				}else { //save them
                    int simpleLabel;
                    util.mothurConvert(util.getSimpleLabel(binLabel), simpleLabel);
					repStruct newRep(repName, sequence, binLabel, simpleLabel, binsize, group);
					reps.push_back(newRep);
				}
			}else { 
				m->mothurOut(rep + " is missing from your fasta or name file, ignoring. Please correct.\n");
			}
		}
		
			
		if (sorted != "") { //then sort them and print them
			if (sorted == "name")		{  sort(reps.begin(), reps.end(), compareName);		}
			else if (sorted == "bin")	{  sort(reps.begin(), reps.end(), compareBin);		}
			else if (sorted == "size")	{  sort(reps.begin(), reps.end(), compareSize);		}
			else if (sorted == "group")	{  sort(reps.begin(), reps.end(), compareGroup);	}
			
			//print them
			for (int i = 0; i < reps.size(); i++) {
				string outputName = reps[i].name + "\t" + reps[i].bin;
				outputName = outputName + "|" + toString(reps[i].size);
				if (reps[i].group != "") {
					outputName = outputName + "|" + reps[i].group;
				}
				out << ">" << outputName << endl;
				out << reps[i].sequence << endl;
			}
		}
		
		in.close();
		out.close();
		out2.close();
        	
		util.mothurRemove(filename);
		util.renameFile(tempNameFile.c_str(), filename.c_str());
        
        if ((countfile != "") && (tempGroup == "noGroup")) {
            if (rename) { //want otu order not order from old count
                thisCt.printSortedTable(filename);
            }else { thisCt.printTable(filename); }
        }
		
		return 0;

	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "processFastaNames");
		exit(1);
	}
}
//**********************************************************************************************************************
int GetOTURepCommand::processNames(string filename, string label) {
	try{
		
		//create output file
		if (outputdir == "") { outputdir += util.hasPath(listfile); }
		
		ofstream out2;
		string tempNameFile = filename + ".temp";
		util.openOutputFile(tempNameFile, out2);
		
		ifstream in; util.openInputFile(filename, in);
		
		string rep, binnames;
        
        string tempGroup = "";
        in >> tempGroup; gobble(in);
        
        CountTable thisCt;
        if (countfile != "") {
            thisCt.readTable(countfile, true, false);
            if (tempGroup != "noGroup") { out2 << "Representative_Sequence\ttotal\t" << tempGroup << endl; }
        }
        
		while (!in.eof()) {
			if (m->getControl_pressed()) { break; }
            string binLabel;
			in >> binLabel >> rep >> binnames; gobble(in);
            
            string repName = rep;
            if (rename) { repName = binLabel; }
            
			if (countfile == "") { out2 << repName << '\t' << binnames << endl; }
            else {
                vector<string> names;
                util.splitAtComma(binnames, names);
                if (tempGroup == "noGroup") {
                    for (int j = 0; j < names.size(); j++) {
                        if (names[j] != rep) { thisCt.mergeCounts(rep, names[j]); }
                    }
                    if (rename) { thisCt.renameSeq(rep, repName); }
                }else {
                    int total = 0; 
                    for (int j = 0; j < names.size(); j++) {  total += thisCt.getGroupCount(names[j], tempGroup);  }
                    out2 << repName << '\t' << total << '\t' << total << endl;
                }
            }

		}
		in.close();
		out2.close();
		
		util.mothurRemove(filename);
		util.renameFile(tempNameFile.c_str(), filename.c_str());
		
        if ((countfile != "") && (tempGroup == "noGroup")) {
            if (rename) { //want otu order not order from old count
                thisCt.printSortedTable(filename);
            }else { thisCt.printTable(filename); }
        }
        
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "GetOTURepCommand", "processNames");
		exit(1);
	}
}
//**********************************************************************************************************************

