/*
 *  chopseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chopseqscommand.h"
#include "removeseqscommand.h"

//**********************************************************************************************************************
vector<string> ChopSeqsCommand::setParameters(){	
	try {
        CommandParameter pfastq("fastq", "InputTypes", "", "", "none", "none", "none","fastq",false,true,true); parameters.push_back(pfastq);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","fasta",false,true,true); parameters.push_back(pfasta);
        CommandParameter pqfile("qfile", "InputTypes", "", "", "none", "none", "none","qfile",false,false,true); parameters.push_back(pqfile);
        CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none","name",false,false,true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none","count",false,false,true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none","group",false,false,true); parameters.push_back(pgroup);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
        CommandParameter pnumbases("numbases", "Number", "", "0", "", "", "","",false,true,true); parameters.push_back(pnumbases);
		CommandParameter pcountgaps("countgaps", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pcountgaps);
		CommandParameter pshort("short", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pshort);
		CommandParameter pkeep("keep", "Multiple", "front-back", "front", "", "", "","",false,false); parameters.push_back(pkeep);
        CommandParameter pformat("format", "Multiple", "sanger-illumina-solexa-illumina1.8+", "illumina1.8+", "", "", "","",false,false,true); parameters.push_back(pformat);
        CommandParameter pkeepn("keepn", "Boolean", "", "f", "", "", "","",false,false); parameters.push_back(pkeepn);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["fasta"] = tempOutNames;
        outputTypes["fastq"] = tempOutNames;
        outputTypes["qfile"] = tempOutNames;
        outputTypes["accnos"] = tempOutNames;
        outputTypes["name"] = tempOutNames;
        outputTypes["group"] = tempOutNames;
        outputTypes["count"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ChopSeqsCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ChopSeqsCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The chop.seqs command reads a fasta or fastq file and outputs a *.chop.* file containing the trimmed sequences. Note: If a sequence is completely 'chopped', an accnos file will be created with the names of the sequences removed. \n";
		helpString += "The chop.seqs command parameters are fasta, fastq, qfile, name, group, count, numbases, countgaps and keep. fasta or fastq is required unless you have a valid current fasta file. numbases is required.\n";
		helpString += "The chop.seqs command should be in the following format: chop.seqs(fasta=yourFasta, numbases=yourNum, keep=yourKeep).\n";
        helpString += "If you provide a name, group or count file any sequences removed from the fasta file will also be removed from those files.\n";
        helpString += "The qfile parameter allows you to provide a quality file associated with the fastafile.\n";
		helpString += "The numbases parameter allows you to specify the number of bases you want to keep.\n";
		helpString += "The keep parameter allows you to specify whether you want to keep the front or the back of your sequence, default=front.\n";
		helpString += "The countgaps parameter allows you to specify whether you want to count gaps as bases, default=false.\n";
		helpString += "The short parameter allows you to specify you want to keep sequences that are too short to chop, default=false.\n";
        helpString += "The keepn parameter allows you to specify you want to keep ambigous bases, default=false.\n";
		helpString += "The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n";
        helpString += "For example, if you ran chop.seqs with numbases=200 and short=t, if a sequence had 100 bases mothur would keep the sequence rather than eliminate it.\n";
		helpString += "Example chop.seqs(fasta=amazon.fasta, numbases=200, keep=front).\n";
		;
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ChopSeqsCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ChopSeqsCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fasta") {  pattern = "[filename],chop.fasta"; }
        else if (type == "fastq") {  pattern = "[filename],chop.fastq"; }
        else if (type == "qfile") {  pattern = "[filename],chop.qual"; }
        else if (type == "name") {  pattern = "[filename],chop.names"; }
        else if (type == "group") {  pattern = "[filename],chop.groups"; }
        else if (type == "count") {  pattern = "[filename],chop.count_table"; } 
        else if (type == "accnos") {  pattern = "[filename],chop.accnos"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
ChopSeqsCommand::ChopSeqsCommand(string option) : Command()  {
	try {
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();

			//check for required parameters
            ValidParameters validParameter;
			fastafile = validParameter.validFile(parameters, "fasta");
			if (fastafile == "not open") { abort = true; }
            else if (fastafile == "not found") {  fastafile = ""; }
            else { current->setFastaFile(fastafile); }
			
            fastqfile = validParameter.validFile(parameters, "fastq");
            if (fastqfile == "not open") { fastqfile = ""; abort = true; }
            else if (fastqfile == "not found") { fastqfile = ""; }
            
			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { namefile = ""; abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { current->setNameFile(namefile); }
            
            qualfile = validParameter.validFile(parameters, "qfile");
            if (qualfile == "not open") { qualfile = ""; abort = true; }
            else if (qualfile == "not found") { qualfile = ""; }
            else { current->setQualFile(qualfile); }
			
			groupfile = validParameter.validFile(parameters, "group");
			if (groupfile == "not open") { groupfile = ""; abort = true; }
			else if (groupfile == "not found") { groupfile = ""; }
			else { current->setGroupFile(groupfile); }
            			
            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not open") { countfile = ""; abort = true; }
			else if (countfile == "not found") { countfile = "";  }
			else { current->setCountFile(countfile); }
            
            if ((namefile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: name or count.\n");  abort = true;
            }
			
            if ((groupfile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: group or count.\n");  abort=true;
            }
            
            if ((fastqfile == "") && (fastafile == "")) {
                fastafile = current->getFastaFile();
                if (fastafile != "") { m->mothurOut("Using " + fastafile + " as input file for the fasta parameter.\n");  }
                else {     m->mothurOut("[ERROR]: You have no current fastafile and did not provide a fastqfile. The fasta or fastq parameter is required to run the chop.seqs command.\n");  abort = true; }
            }
            
			string temp = validParameter.valid(parameters, "numbases");	if (temp == "not found") { temp = "0"; }
			util.mothurConvert(temp, numbases);   
			
            temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
			processors = current->setProcessors(temp);
            
			temp = validParameter.valid(parameters, "countgaps");	if (temp == "not found") { temp = "f"; }
			countGaps = util.isTrue(temp);  
			
			temp = validParameter.valid(parameters, "short");	if (temp == "not found") { temp = "f"; }
			Short = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "keepn");	if (temp == "not found") { if (qualfile!= "") { temp = "t"; }else { temp = "f"; } }
            keepN = util.isTrue(temp);
            
            format = validParameter.valid(parameters, "format");        if (format == "not found"){    format = "illumina1.8+";    }
            
            if (((!keepN) && (qualfile != "")) || ((countGaps) && (qualfile != ""))){ m->mothurOut("[ERROR]: You cannot set keepn=false with a quality file, or set countgaps to true.\n");  abort = true;  }
		
			keep = validParameter.valid(parameters, "keep");		if (keep == "not found") { keep = "front"; } 
				
			if (numbases == 0)  { m->mothurOut("You must provide the number of bases you want to keep for the chops.seqs command.\n");  abort = true;  }
		}

	}
	catch(exception& e) {
		m->errorOut(e, "ChopSeqsCommand", "ChopSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int ChopSeqsCommand::execute(){
	try {
		
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        bool wroteAccnos = true; string outputFileNameAccnos = "";
        if (fastafile != "")    {  wroteAccnos = runChopFasta(outputFileNameAccnos);  }
        else                    {  wroteAccnos = runChopFastq(outputFileNameAccnos);  }
		
        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }
        
        if (wroteAccnos) {
            map<string, string> variables;
            outputNames.push_back(outputFileNameAccnos); outputTypes["accnos"].push_back(outputFileNameAccnos);
            
             //use remove.seqs to create new name, group and count file
            if ((countfile != "") || (namefile != "") || (groupfile != "")) {
                string inputString = "accnos=" + outputFileNameAccnos;
                
                if (countfile != "") {  inputString += ", count=" + countfile;  }
                else{
                    if (namefile != "") {  inputString += ", name=" + namefile;  }
                    if (groupfile != "") {  inputString += ", group=" + groupfile;  }
                }
                
                m->mothurOut("/******************************************/\n"); 
                m->mothurOut("Running command: remove.seqs(" + inputString + ")\n"); 
                current->setMothurCalling(true);
                
                Command* removeCommand = new RemoveSeqsCommand(inputString);
                removeCommand->execute();
                
                map<string, vector<string> > filenames = removeCommand->getOutputFiles();
                
                delete removeCommand;
                current->setMothurCalling(false);
                m->mothurOut("/******************************************/\n"); 
                
                if (groupfile != "") {
                    string thisOutputDir = outputdir;
                    if (outputdir == "") {  thisOutputDir += util.hasPath(groupfile);  }
                    variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(groupfile));
                    string outGroup = getOutputFileName("group", variables);
                    util.renameFile(filenames["group"][0], outGroup);
                    outputNames.push_back(outGroup); outputTypes["group"].push_back(outGroup);
                }
                
                if (namefile != "") {
                    string thisOutputDir = outputdir;
                    if (outputdir == "") {  thisOutputDir += util.hasPath(namefile);  }
                    variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(namefile));
                    string outName = getOutputFileName("name", variables);
                    util.renameFile(filenames["name"][0], outName);
                    outputNames.push_back(outName); outputTypes["name"].push_back(outName);
                }
                
                if (countfile != "") {
                    string thisOutputDir = outputdir;
                    if (outputdir == "") {  thisOutputDir += util.hasPath(countfile);  }
                    variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(countfile));
                    string outCount = getOutputFileName("count", variables);
                    util.renameFile(filenames["count"][0], outCount);
                    outputNames.push_back(outCount); outputTypes["count"].push_back(outCount);
                }
            }
        }
		else {  util.mothurRemove(outputFileNameAccnos);  }
		
        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }
        
		//set fasta file as new current fastafile
		string currentName = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFastaFile(currentName); }
		}
        
		if (wroteAccnos) { //set accnos file as new current accnosfile
			itTypes = outputTypes.find("accnos");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setAccnosFile(currentName); }
			}
            
            itTypes = outputTypes.find("name");
            if (itTypes != outputTypes.end()) {
                if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setNameFile(currentName); }
            }
            
            itTypes = outputTypes.find("group");
            if (itTypes != outputTypes.end()) {
                if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setGroupFile(currentName); }
            }
            
            itTypes = outputTypes.find("count");
            if (itTypes != outputTypes.end()) {
                if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setCountFile(currentName); }
            }
		}
        
        m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
		
		return 0;		
	}

	catch(exception& e) {
		m->errorOut(e, "ChopSeqsCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

bool ChopSeqsCommand::runChopFasta(string& outputFileNameAccnos){
    try {
        
        map<string, string> variables;
        string thisOutputDir = outputdir;
        if (outputdir == "") {  thisOutputDir += util.hasPath(fastafile);  }
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(fastafile));
        string outputFileName = getOutputFileName("fasta", variables);
        outputNames.push_back(outputFileName); outputTypes["fasta"].push_back(outputFileName);
        outputFileNameAccnos = getOutputFileName("accnos", variables);
        
        string fastafileTemp = "";
        if (qualfile != "") {  fastafileTemp = outputFileName + ".qualFile.Positions.temp"; }
        
        bool wroteAccnos = createProcesses(fastafile, outputFileName, outputFileNameAccnos, fastafileTemp);
        
        if (m->getControl_pressed()) {  return wroteAccnos; }
        
        if (qualfile != "") {
            thisOutputDir = outputdir;
            if (outputdir == "") {  thisOutputDir += util.hasPath(qualfile);  }
            variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(qualfile));
            string outputQualFileName = getOutputFileName("qfile", variables);
            outputNames.push_back(outputQualFileName); outputTypes["qfile"].push_back(outputQualFileName);
            
            processQual(outputQualFileName, fastafileTemp);
            util.mothurRemove(fastafileTemp);
        }
        
        return wroteAccnos;
        
    }catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "runChopFasta");
        exit(1);
    }
}
//**********************************************************************************************************************

bool ChopSeqsCommand::runChopFastq(string& outputFileNameAccnos){
    try {
        
        map<string, string> variables;
        string thisOutputDir = outputdir;
        if (outputdir == "") {  thisOutputDir += util.hasPath(fastqfile);  }
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(fastqfile));
        string outputFileName = getOutputFileName("fastq", variables);
        outputNames.push_back(outputFileName); outputTypes["fastq"].push_back(outputFileName);
        outputFileNameAccnos = getOutputFileName("accnos", variables);
        
        ifstream in; util.openInputFile(fastqfile, in);
        ofstream out; util.openOutputFile(outputFileName, out);
        ofstream outAccnos; util.openOutputFile(outputFileNameAccnos, outAccnos);

        bool wroteAccnos = false; long long count = 0;
        while (!in.eof()) {
            
            if (m->getControl_pressed()) {  break;  }
            
            bool ignore;
            FastqRead seq(in, ignore, format); gobble(in);

            if (seq.getName() != "") {
                bool isGood = getFastqChopped(seq);
                
                //output trimmed sequence
                if (isGood) { seq.printFastq(out); }
                else{
                    outAccnos << seq.getName() << endl;
                    wroteAccnos = true;
                }
                count++;
            }

            //report progress
            if((count) % 10000 == 0){    m->mothurOut(toString(count)+"\n");         }
            
        }
        //report progress
        if((count) % 10000 != 0){    m->mothurOut(toString(count)+"\n");         }
        
        in.close(); out.close(); outAccnos.close();
        
        return wroteAccnos;
        
    }catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "runChopFastq");
        exit(1);
    }
}
//**********************************************************************************************************************
bool ChopSeqsCommand::getFastqChopped(FastqRead& seq) {
    try {
        string temp = seq.getSeq();
        vector<int> scores = seq.getScores();
        
        //if needed trim sequence
        if (keep == "front") {//you want to keep the beginning
            int tempLength = temp.length();
            
            if (tempLength > numbases) { //you have enough bases to remove some
                
                int stopSpot = 0;
                int numBasesCounted = 0;
                
                for (int i = 0; i < temp.length(); i++) {
                    
                    numBasesCounted++;
                    
                    if (numBasesCounted >= numbases) { stopSpot = i; break; }
                }
                
                if (stopSpot == 0) { temp = ""; scores.clear(); }
                else {
                    temp = temp.substr(0, stopSpot+1);
                    if(scores.size() >= stopSpot+1){
                        scores.resize((stopSpot+1));
                    }
                }
                
            }else {
                if (!Short) { temp = ""; scores.clear(); } //sequence too short
            }
        }else { //you are keeping the back
            int tempLength = temp.length();
            if (tempLength > numbases) { //you have enough bases to remove some
                
                int stopSpot = 0;
                int numBasesCounted = 0;
                
                for (int i = (temp.length()-1); i >= 0; i--) {
                    
                    numBasesCounted++;
                    
                    if (numBasesCounted >= numbases) { stopSpot = i; break; }
                }
                
                if (stopSpot == 0) { temp = ""; }
                else {
                    temp = temp.substr(stopSpot);
                    if(scores.size() >= stopSpot){
                        
                        vector<int> scores2;
                        for (int h = stopSpot; h < scores.size(); h++) { scores2.push_back(scores[h]); }
                        scores.clear(); scores = scores2;
                    }
                }
            }else {
                if (!Short) { temp = ""; scores.clear(); } //sequence too short
            }
        }

        seq.setSeq(temp); seq.setScores(scores);
        
        if (temp != "") { return true; }
        
        return false;
    }
    catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "getFastqChopped");
        exit(1);
    }
}
/**************************************************************************************************/
//custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct chopData {
    string filename, keep, qualValues, qualFileOutput;
    int count;
    unsigned long long start;
    unsigned long long end;
    OutputWriter* threadWriterAccnos;
    OutputWriter* threadWriterOutput;
    MothurOut* m;
    Utils util;
    
    int numbases;
    bool countGaps, Short, wroteAccnos, keepN;
    Sequence thisSeq;
    
    chopData(){}
    chopData(string f, unsigned long long st, unsigned long long en, OutputWriter* wo, OutputWriter* wa, string qv) {
        m = MothurOut::getInstance();
        filename = f;
        threadWriterOutput = wo;
        threadWriterAccnos = wa;
        qualFileOutput = qv;
        start = st;
        end = en;
        count = 0;
        qualValues = "";
        keep = "front";
        countGaps = false;
        Short = false;
        keepN = false; if (qv!="") { keepN = true; }
        wroteAccnos = false;
    }
    void setVariables(string k, bool cGaps, int nbases, bool S, bool kn) {
        keep = k;
        countGaps = cGaps;
        numbases = nbases;
        Short = S;
        keepN = kn;
    }
    void setChopped(Sequence s) { thisSeq = s; }
};
//**********************************************************************************************************************
string getChopped(chopData* params) {
    try {
        string temp = params->thisSeq.getAligned();
        string tempUnaligned = params->thisSeq.getUnaligned();
        
        if (params->countGaps) {
            //if needed trim sequence
            if (params->keep == "front") {//you want to keep the beginning
                int tempLength = temp.length();
                
                if (tempLength > params->numbases) { //you have enough bases to remove some
                    
                    int stopSpot = 0;
                    int numBasesCounted = 0;
                    
                    for (int i = 0; i < temp.length(); i++) {
                        //eliminate N's
                        if (!params->keepN) { if (toupper(temp[i]) == 'N') { temp[i] = '.'; } }
                        
                        numBasesCounted++;
                        
                        if (numBasesCounted >= params->numbases) { stopSpot = i; break; }
                    }
                    
                    if (stopSpot == 0) { temp = ""; }
                    else {  temp = temp.substr(0, stopSpot+1);  }
                    
                }else {
                    if (!params->Short) { temp = ""; } //sequence too short
                }
            }else { //you are keeping the back
                int tempLength = temp.length();
                if (tempLength > params->numbases) { //you have enough bases to remove some
                    
                    int stopSpot = 0;
                    int numBasesCounted = 0;
                    
                    for (int i = (temp.length()-1); i >= 0; i--) {
                        //eliminate N's
                        if (!params->keepN) { if (toupper(temp[i]) == 'N') { temp[i] = '.'; } }
                        
                        numBasesCounted++;
                        
                        if (numBasesCounted >= params->numbases) { stopSpot = i; break; }
                    }
                    
                    if (stopSpot == 0) { temp = ""; }
                    else {  temp = temp.substr(stopSpot+1);  }
                }else {
                    if (!params->Short) { temp = ""; } //sequence too short
                }
            }
            
        }else{
            
            //if needed trim sequence
            if (params->keep == "front") {//you want to keep the beginning
                int tempLength = tempUnaligned.length();
                
                if (tempLength > params->numbases) { //you have enough bases to remove some
                    
                    int stopSpot = 0;
                    int numBasesCounted = 0;
                    
                    for (int i = 0; i < temp.length(); i++) {
                        //eliminate N's
                        if (!params->keepN) {
                            if (toupper(temp[i]) == 'N') {
                                temp[i] = '.';
                                tempLength--;
                                if (tempLength < params->numbases) { stopSpot = 0; break; }
                            }
                        }
                        if(isalpha(temp[i])) { numBasesCounted++; }
                        
                        if (numBasesCounted >= params->numbases) { stopSpot = i; break; }
                    }
                    
                    if (stopSpot == 0) { temp = ""; }
                    else {  temp = temp.substr(0, stopSpot+1);  }
                    
                    params->qualValues = params->thisSeq.getName() +'\t' + toString(0) + '\t' + toString(stopSpot+1) + '\n';
                    
                }else {
                    if (!params->Short) { temp = ""; params->qualValues = params->thisSeq.getName() +'\t' + toString(0) + '\t' + toString(0) + '\n'; } //sequence too short
                    else { params->qualValues = params->thisSeq.getName() +'\t' + toString(0) + '\t' + toString(tempLength) + '\n'; }
                }
            }else { //you are keeping the back
                int tempLength = tempUnaligned.length();
                if (tempLength > params->numbases) { //you have enough bases to remove some
                    
                    int stopSpot = 0;
                    int numBasesCounted = 0;
                    
                    for (int i = (temp.length()-1); i >= 0; i--) {
                        if (!params->keepN) {
                            //eliminate N's
                            if (toupper(temp[i]) == 'N') {
                                temp[i] = '.';
                                tempLength--;
                                if (tempLength < params->numbases) { stopSpot = 0; break; }
                            }
                        }
                        if(isalpha(temp[i])) { numBasesCounted++; }
                        
                        if (numBasesCounted >= params->numbases) { stopSpot = i; break; }
                    }
                    
                    if (stopSpot == 0) { temp = ""; }
                    else {  temp = temp.substr(stopSpot);  }
                    
                    params->qualValues = params->thisSeq.getName() +'\t' + toString(stopSpot) + '\t' + toString(temp.length()-1) + '\n';
                    
                }else { 
                    if (!params->Short) { temp = ""; params->qualValues = params->thisSeq.getName() +'\t' + toString(0) + '\t' + toString(0) + '\n'; } //sequence too short
                    else { params->qualValues = params->thisSeq.getName() +'\t' + toString(0) + '\t' + toString(tempLength) + '\n'; }
                }
            }
        }
        
        return temp;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ChopSeqsCommand", "getChopped");
        exit(1);
    }
}

/**************************************************************************************/
void driverChop(chopData* params) {
	try {
		ifstream in; params->util.openInputFile(params->filename, in);
        
		in.seekg(params->start);
        
        //adjust
        if (params->start == 0) {  params->util.zapGremlins(in); gobble(in); }
        
        ofstream outfTemp;
        if (params->qualFileOutput != "") { params->util.openOutputFile(params->qualFileOutput, outfTemp); }
		
        bool wroteAccnos = false;
		while (!in.eof()) {
            
            if (params->m->getControl_pressed()) {  break;  }
            
			Sequence seq(in); gobble(in);

			if (seq.getName() != "") {
                params->qualValues = "";
                params->thisSeq = seq;
                
				string newSeqString = getChopped(params);
				
				//output trimmed sequence
                if (newSeqString != "") { params->threadWriterOutput->write(">"+seq.getName()+"\n"+newSeqString+"\n");  }
				else{
                    params->threadWriterAccnos->write(seq.getName()+"\n");
					wroteAccnos = true;
				}
                if (params->qualFileOutput != "") {  outfTemp << params->qualValues << endl;  }
                params->count++;
			}
			
#if defined NON_WINDOWS
            unsigned long long pos = in.tellg();
            if ((pos == -1) || (pos >= params->end)) { break; }
#else
            if (params->count == params->end) { break; }
#endif
            //report progress
			if((params->count) % 10000 == 0){	params->m->mothurOut(toString(params->count)+"\n"); 		}
			
		}
		//report progress
		if((params->count) % 10000 != 0){	params->m->mothurOut(toString(params->count)+"\n"); 		}
		
		in.close();
        if (params->qualFileOutput != "") { outfTemp.close(); }
        
		params->wroteAccnos = wroteAccnos;
	}
	catch(exception& e) {
		params->m->errorOut(e, "ChopSeqsCommand", "driver");
		exit(1);
	}
}
/**************************************************************************************************/
bool ChopSeqsCommand::createProcesses(string filename, string outFasta, string outAccnos, string fastafileTemp) {
    try {
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<chopData*> data;
        vector<linePair> lines;
        
        long long num = 0;
        vector<double> positions;
#if defined NON_WINDOWS
        positions = util.divideFile(filename, processors);
        for (int i = 0; i < (positions.size()-1); i++) {	lines.push_back(linePair(positions[i], positions[(i+1)]));	}
#else
        if (processors == 1) { lines.push_back(linePair(0, -1)); }//forces it to read whole file
        else {
            positions = util.setFilePosFasta(filename, num);
            if (num < processors) { processors = num; }
            
            //figure out how many sequences you have to process
            int numSeqsPerProcessor = num / processors;
            for (int i = 0; i < processors; i++) {
                int startIndex =  i * numSeqsPerProcessor;
                if(i == (processors - 1)){	numSeqsPerProcessor = num - i * numSeqsPerProcessor; 	}
                lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
            }
        }
#endif
        
        auto synchronizedOutputFile = std::make_shared<SynchronizedOutputFile>(outFasta);
        auto synchronizedAccnosFile = std::make_shared<SynchronizedOutputFile>(outAccnos);
        
        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            string extension = toString(i+1) + ".temp";
            OutputWriter* threadOutputWriter = new OutputWriter(synchronizedOutputFile);
            OutputWriter* threadAccnosWriter = new OutputWriter(synchronizedAccnosFile);
            
            chopData* dataBundle = new chopData(filename, lines[i+1].start, lines[i+1].end, threadOutputWriter, threadAccnosWriter, fastafileTemp+extension);
            dataBundle->setVariables(keep, countGaps, numbases, Short, keepN);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(driverChop, dataBundle));
        }
        
        OutputWriter* threadOutputWriter = new OutputWriter(synchronizedOutputFile);
        OutputWriter* threadAccnosWriter = new OutputWriter(synchronizedAccnosFile);
    
        chopData* dataBundle = new chopData(filename, lines[0].start, lines[0].end, threadOutputWriter, threadAccnosWriter, fastafileTemp);
        dataBundle->setVariables(keep, countGaps, numbases, Short, keepN);
        driverChop(dataBundle);
        num = dataBundle->count;
        bool wroteAccnos = dataBundle->wroteAccnos;

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            num += data[i]->count;
            
            delete data[i]->threadWriterOutput;
            delete data[i]->threadWriterAccnos;
            if (data[i]->wroteAccnos) { wroteAccnos = true; }
            
            #if defined NON_WINDOWS
            #else
            if (data[i]->count != data[i]->end) {
                m->mothurOut("[ERROR]: process " + toString(i+1) + " only processed " + toString(data[i]->count) + " of " + toString(data[i]->end) + " sequences assigned to it, quitting. \n"); m->setControl_pressed(true);
            }
            #endif
            if (fastafileTemp != "") {
                util.appendFiles(data[i]->qualFileOutput, fastafileTemp);
                util.mothurRemove(data[i]->qualFileOutput);
            }
            delete data[i];
            delete workerThreads[i];
        }
        synchronizedOutputFile->close(); synchronizedAccnosFile->close();
        delete threadOutputWriter; delete threadAccnosWriter;
        delete dataBundle;
        return wroteAccnos;
    }
    catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "createProcesses");
        exit(1);
    }
}
//**********************************************************************************************************************
int ChopSeqsCommand::processQual(string outputFile, string inputFile) {
    try {
        ofstream out; util.openOutputFile(outputFile, out);
        ifstream in;  util.openInputFile(inputFile, in);
        ifstream inQual; util.openInputFile(qualfile, inQual);

        m->mothurOut("Processing the quality file.\n");
        
        int count = 0;
        while (!in.eof()) {
            
            if (m->getControl_pressed()) { in.close(); out.close(); return 0; }
            
            QualityScores qual(inQual); gobble(inQual);
            
            string name = "";
            int start = 0; int end = 0;
            in >> name >> start >> end; gobble(in);
            
            if (qual.getName() != "") {
                if (qual.getName() != name) { start = 0; end = 0; }
                else if (start != 0) {
                    qual.trimQScores(start, -1);
                    qual.printQScores(out);
                }else if ((start == 0) && (end == 0)) {}
                else if ((start == 0) && (end != 0)) {
                    qual.trimQScores(-1, end);
                    qual.printQScores(out);
                }
            }
            count++;
            //report progress
            if((count) % 10000 == 0){	m->mothurOut(toString(count)); m->mothurOutEndLine();		}
            
        }
        //report progress
        if((count) % 10000 != 0){	m->mothurOut(toString(count)); m->mothurOutEndLine();		}
        
        in.close(); inQual.close(); out.close();
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "ChopSeqsCommand", "processQual");
        exit(1);
    }
}
//**********************************************************************************************************************


