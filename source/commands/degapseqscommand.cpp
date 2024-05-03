/*
 *  degapseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 6/21/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "degapseqscommand.h"

#include "writer.h"
#include "datastructures/sequence.hpp"

//**********************************************************************************************************************
vector<string> DegapSeqsCommand::setParameters(){	
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","fasta",false,true,true); parameters.push_back(pfasta);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["fasta"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "DegapSeqsCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string DegapSeqsCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The degap.seqs command reads a fastafile and removes all gap characters.\n";
		helpString += "The degap.seqs command parameter are fasta and processors.\n";
		helpString += "The fasta parameter allows you to enter the fasta file containing your sequences, and is required unless you have a valid current fasta file. \n";
        helpString += "The processors parameter allows you to enter the number of processors you would like to use. \n";
		helpString += "The degap.seqs command should be in the following format: \n";
		helpString += "degap.seqs(fasta=yourFastaFile) \n";	
		helpString += "Example: degap.seqs(fasta=abrecovery.align) \n";
			
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "DegapSeqsCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string DegapSeqsCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        if (type == "fasta") {  pattern = "[filename],ng.fasta"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "DegapSeqsCommand", "getOutputPattern");
        exit(1);
    }
}
//***************************************************************************************************************
DegapSeqsCommand::DegapSeqsCommand(string option) : Command()  {
	try {
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
            fastafile = validParameter.validFile(parameters, "fasta");
            if (fastafile == "not found") {
                fastafile = current->getFastaFile();
                if (fastafile != "") { m->mothurOut("Using " + fastafile + " as input file for the fasta parameter.\n"); }
                else { 	m->mothurOut("[ERROR]: You have no current fasta file and the fasta parameter is required.\n");  abort = true; }
            }
            else if (fastafile == "not open") { abort = true; }
            else { current->setFastaFile(fastafile); }

            
            string temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
            processors = current->setProcessors(temp);
			
			 
            if (outputdir == ""){   outputdir += util.hasPath(fastafile); }
			
		}
	}
	catch(exception& e) {
		m->errorOut(e, "DegapSeqsCommand", "DegapSeqsCommand");
		exit(1);
	}
}
//***************************************************************************************************************
int DegapSeqsCommand::execute(){
	try{
		
		if (abort) { if (calledHelp) { return 0; }  return 2;	}

        m->mothurOut("Degapping sequences from " + fastafile + " ...\n" );
        
        string tempOutputDir = outputdir;
        if (outputdir == "") { tempOutputDir = util.hasPath(fastafile); }
        map<string, string> variables;
        variables["[filename]"] = tempOutputDir + util.getRootName(util.getSimpleName(fastafile));
        string degapFile = getOutputFileName("fasta", variables);
        outputNames.push_back(degapFile); outputTypes["fasta"].push_back(degapFile);
        
        long start = time(nullptr);
        
        int numSeqs = createProcesses(fastafile, degapFile);
        
        m->mothurOut("It took " + toString(time(nullptr) - start) + " secs to degap " + toString(numSeqs) + " sequences.\n\n");
        
        if (m->getControl_pressed()) {  for (int j = 0; j < outputNames.size(); j++) {	util.mothurRemove(outputNames[j]);	} return 0; }
		
		//set fasta file as new current fastafile
		string currentName = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFastaFile(currentName); }
		}
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}	
		m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "DegapSeqsCommand", "execute");
		exit(1);
	}
}
/**************************************************************************************************/
//custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct degapData {
    string filename;
    int count;
    unsigned long long start;
    unsigned long long end;
    OutputWriter* threadWriter;
    MothurOut* m;
    Utils util;
    
    degapData(){}
    degapData(string f, unsigned long long st, unsigned long long en, OutputWriter* w) { //InputReader* i
        m = MothurOut::getInstance();
        filename = f;
        threadWriter = w;
        start = st;
        end = en;
        count = 0;
    }
};

//***************************************************************************************************************
void driverDegap(degapData* params){
    try{
        ifstream inFASTA;
        params->util.openInputFile(params->filename, inFASTA);
        
        inFASTA.seekg(params->start);
        
        if (params->start == 0) {  params->util.zapGremlins(inFASTA); gobble(inFASTA); }
        
        while(!inFASTA.eof()){
            if (params->m->getControl_pressed()) {  break; }
            
            Sequence currSeq(inFASTA); gobble(inFASTA);
            if (currSeq.getName() != "") {
                params->threadWriter->write(">"+currSeq.getName()+"\n"+currSeq.getUnaligned()+"\n");
                params->count++;
            }
    
            //report progress
            if((params->count) % 10000 == 0){	params->m->mothurOutJustToScreen(toString(params->count) + "\n");  }
            
#if defined NON_WINDOWS
            unsigned long long pos = inFASTA.tellg();
            if ((pos == -1) || (pos >= params->end)) { break; }
#else
            if (params->count == params->end) { break; }
#endif
        }
        //report progress
        if((params->count) % 10000 != 0){	params->m->mothurOutJustToScreen(toString(params->count) + "\n"); 		}
        
        inFASTA.close();
    }
    catch(exception& e) {
        params->m->errorOut(e, "DegapSeqsCommand", "driver");
        exit(1);
    }
}
//***************************************************************************************************************
long long DegapSeqsCommand::createProcesses(string filename, string outputFileName){
    try{
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<degapData*> data;
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
        
        auto synchronizedFile = std::make_shared<SynchronizedOutputFile>(outputFileName);
        
        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            OutputWriter* threadWriter = new OutputWriter(synchronizedFile);
            degapData* dataBundle = new degapData(filename, lines[i+1].start, lines[i+1].end, threadWriter);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(driverDegap, dataBundle));
        }
        
        OutputWriter* threadWriter = new OutputWriter(synchronizedFile);
        degapData* dataBundle = new degapData(filename, lines[0].start, lines[0].end, threadWriter);
        driverDegap(dataBundle);
        num = dataBundle->count;

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            num += data[i]->count;
            
            delete data[i]->threadWriter;
            delete data[i];
            delete workerThreads[i];
        }
        synchronizedFile->close();
        delete threadWriter;
        delete dataBundle;
        
        return num;
    }
    catch(exception& e) {
        m->errorOut(e, "DegapSeqsCommand", "createProcesses");
        exit(1);
    }
}
//***************************************************************************************************************

