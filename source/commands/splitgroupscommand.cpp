/*
 *  splitgroupscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/20/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "splitgroupscommand.h"

#include "commands/getseqscommand.h"
#include "datastructures/counttable.h"
#include "datastructures/sequenceparser.h"
#include "inputdata.h"

//**********************************************************************************************************************
vector<string> SplitGroupCommand::setParameters(){
	try {
        CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none","list",false,false,true); parameters.push_back(plist);
        CommandParameter pflow("flow", "InputTypes", "", "", "none", "none", "none","fasta",false,false,true); parameters.push_back(pflow);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","fasta",false,false,true); parameters.push_back(pfasta);
        CommandParameter pfastq("fastq", "InputTypes", "", "", "none", "none", "none","fastq",false,false,true); parameters.push_back(pfastq);
        CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none","name",false,false,true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "CountGroup", "none","count",false,false,true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "CountGroup", "none","group",false,false,true); parameters.push_back(pgroup);
		CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
        CommandParameter pformat("format", "Multiple", "sanger-illumina-solexa-illumina1.8+", "sanger", "", "", "","",false,false,true); parameters.push_back(pformat);
        CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

        vector<string> tempOutNames;
        outputTypes["fasta"] = tempOutNames;
        outputTypes["flow"] = tempOutNames;
        outputTypes["fastq"] = tempOutNames;
        outputTypes["name"] = tempOutNames;
        outputTypes["count"] = tempOutNames;
        outputTypes["group"] = tempOutNames;
        outputTypes["list"] = tempOutNames;

        abort = false; calledHelp = false;

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "SplitGroupCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string SplitGroupCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The split.groups command reads a group or count file, and parses your files by groups. \n";
		helpString += "The split.groups command parameters are fasta, fastq, flow, name, group, count, groups and processors.\n";
		helpString += "The group or count parameter is required.\n";
		helpString += "The groups parameter allows you to select groups to create files for.  \n";
        helpString += "The format parameter is used with the fastq parameter to indicate whether your sequences are sanger, solexa, illumina1.8+ or illumina, default=illumina1.8+.\n";
		helpString += "For example if you set groups=A-B-C, you will get a .A.fasta, .A.names, .B.fasta, .B.names, .C.fasta, .C.names files.  \n";
		helpString += "If you want .fasta and .names files for all groups, set groups=all.  \n";
		helpString += "The split.groups command should be used in the following format: split.group(fasta=yourFasta, group=yourGroupFile).\n";
		helpString += "Example: split.groups(fasta=abrecovery.fasta, group=abrecovery.groups).\n";
		;
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "SplitGroupCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string SplitGroupCommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "fasta")        {  pattern = "[filename],[group],fasta";        }
        else if (type == "list")    {  pattern = "[filename],[group],list";         }
        else if (type == "fastq")    {  pattern = "[filename],[group],fastq";       }
        else if (type == "flow")    {  pattern = "[filename],[group],flow";         }
        else if (type == "name")    {  pattern = "[filename],[group],names";        }
        else if (type == "count")   {  pattern = "[filename],[group],count_table";  }
        else if (type == "group")   {  pattern = "[filename],[group],groups";  }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "SplitGroupCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************

SplitGroupCommand::SplitGroupCommand(vector<string> g, string ffile, string count, string outd) : Command() {
    try {
        abort = false; calledHelp = false;
        vector<string> tempOutNames;
        outputTypes["count"] = tempOutNames;
        outputTypes["fasta"] = tempOutNames;

        Groups = g; groupfile = ""; namefile = "";
        fastafile = ffile; countfile = count; outputdir = outd;
        string temp = current->getProcessors(); processors = current->setProcessors(temp);

        if (processors > Groups.size()) { processors = Groups.size(); m->mothurOut("Reducing processors to " + toString(Groups.size()) + ".\n"); }

        //divide the groups between the processors
        int remainingPairs = Groups.size();
        int startIndex = 0;
        for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
          int numPairs = remainingPairs; //case for last processor
          if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
          lines.push_back(linePair(startIndex, (startIndex+numPairs))); //startIndex, endIndex
          startIndex = startIndex + numPairs;
          remainingPairs = remainingPairs - numPairs;
        }

        splitFastaCount();
    }
    catch(exception& e) {
        m->errorOut(e, "SplitGroupCommand", "SplitGroupCommand - mothurRun");
        exit(1);
    }
}
//**********************************************************************************************************************
SplitGroupCommand::SplitGroupCommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }

		else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser.getParameters();

			ValidParameters validParameter;
			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { namefile = ""; abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { current->setNameFile(namefile); }

			fastafile = validParameter.validFile(parameters, "fasta");
			if (fastafile == "not open") { abort = true; }
            else if (fastafile == "not found") {	fastafile = "";  }
            else { current->setFastaFile(fastafile); }

            listfile = validParameter.validFile(parameters, "list");
            if (listfile == "not open") { abort = true; }
            else if (listfile == "not found") {	listfile = "";  }
            else { current->setListFile(listfile); }

            fastqfile = validParameter.validFile(parameters, "fastq");
            if (fastqfile == "not open") { abort = true; }
            else if (fastqfile == "not found") {	fastqfile = "";  }

            flowfile = validParameter.validFile(parameters, "flow");
            if (flowfile == "not open") { abort = true; }
            else if (flowfile == "not found") {	flowfile = "";  }
            else { current->setFlowFile(flowfile); }

			groupfile = validParameter.validFile(parameters, "group");
			if (groupfile == "not open") {  groupfile = ""; abort = true; }
			else if (groupfile == "not found") { groupfile = "";
			}else {  current->setGroupFile(groupfile); }

            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not open") { countfile = ""; abort = true; }
			else if (countfile == "not found") { countfile = ""; }
			else { current->setCountFile(countfile); }

            if ((fastafile == "") && (flowfile == "") && (fastqfile == "")) {
                fastafile = current->getFastaFile();
                if (fastafile != "") { m->mothurOut("Using " + fastafile + " as input file for the fasta parameter.\n"); }
                else {
                    flowfile = current->getFlowFile();
                    if (flowfile != "") {  m->mothurOut("Using " + flowfile + " as input file for the flow parameter.\n");  }
                    else {
                        listfile = current->getListFile();
                        if (listfile != "") {  m->mothurOut("Using " + listfile + " as input file for the list parameter.\n");  }
                        else { m->mothurOut("[ERROR]: You need to provide a fasta, list, fastq or flow file.\n");  abort = true; }
                    }
                }
            }

            if ((countfile != "") && (namefile != "")) { m->mothurOut("You must enter ONLY ONE of the following: count or name.\n");  abort = true; }

            if ((countfile != "") && (groupfile != "")) { m->mothurOut("You must enter ONLY ONE of the following: count or group.\n");  abort = true; }

            if ((countfile == "") && (groupfile == "")) {
                if (namefile == "") { //check for count then group
                    countfile = current->getCountFile();
					if (countfile != "") {  m->mothurOut("Using " + countfile + " as input file for the count parameter.\n");  }
					else {
						groupfile = current->getGroupFile();
                        if (groupfile != "") {  m->mothurOut("Using " + groupfile + " as input file for the group parameter.\n");  }
                        else {
                            m->mothurOut("You need to provide a count or group file.\n");
                            abort = true;
                        }
					}
                }else { //check for group
                    groupfile = current->getGroupFile();
                    if (groupfile != "") {  m->mothurOut("Using " + groupfile + " as input file for the group parameter.\n");  }
                    else {
                        m->mothurOut("You need to provide a count or group file.\n");
                        abort = true;
                    }
                }
            }

			groups = validParameter.valid(parameters, "groups");
			if (groups == "not found") { groups = ""; }
			else {
                util.splitAtDash(groups, Groups);
                if (Groups.size() != 0) {
                    if (Groups[0]== "all") { Groups.clear(); }
                }
            }

            string temp = validParameter.valid(parameters, "processors");    if (temp == "not found"){    temp = current->getProcessors();    }
            processors = current->setProcessors(temp);

            format = validParameter.valid(parameters, "format");		if (format == "not found"){	format = "illumina1.8+";	}

            if ((format != "sanger") && (format != "illumina") && (format != "illumina1.8+") && (format != "solexa"))  {
                m->mothurOut(format + " is not a valid format. Your format choices are sanger, solexa, illumina1.8+ and illumina, aborting.\n" );
                abort=true;
            }


            if (outputdir == ""){
                if (groupfile != "") { outputdir = util.hasPath(groupfile); }
                else { outputdir = util.hasPath(countfile);  }
            }

		}

	}
	catch(exception& e) {
		m->errorOut(e, "SplitGroupCommand", "SplitGroupCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
int SplitGroupCommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

        vector<string> namesGroups;
        if (groupfile != "") {
            GroupMap groupMap(groupfile);
            groupMap.readMap();
            namesGroups = groupMap.getNamesOfGroups();
        }else if (countfile != ""){
            CountTable ct;
            ct.readTable(countfile, true, true, Groups);
            namesGroups = ct.getNamesOfGroups();
        }else { m->mothurOut("[ERROR]: you must provide a count or group file to split by group. quitting... \n"); m->setControl_pressed(true);  return 0; }

        if (Groups.size() == 0) { Groups = namesGroups; }

        if (processors > Groups.size()) { processors = Groups.size(); m->mothurOut("Reducing processors to " + toString(Groups.size()) + ".\n"); }

        //divide the groups between the processors
        int remainingPairs = Groups.size();
        int startIndex = 0;
        for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
          int numPairs = remainingPairs; //case for last processor
          if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
          lines.push_back(linePair(startIndex, (startIndex+numPairs))); //startIndex, endIndex
          startIndex = startIndex + numPairs;
          remainingPairs = remainingPairs - numPairs;
        }


        if (flowfile != "")         {  splitFastqOrFlow(flowfile, ".flow");     }
        if (fastqfile != "")        {  splitFastqOrFlow(fastqfile, ".fastq");   }
        if ((fastafile != "") || (listfile != ""))      {
            bool isCount = true;
            if (countfile == "" )   {  isCount = false;  }
            splitCountOrGroup(isCount);
        }

		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);	} return 0; }

		string currentName = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFastaFile(currentName); }
		}

        itTypes = outputTypes.find("flow");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFlowFile(currentName); }
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
		m->errorOut(e, "SplitGroupCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
int driverRunNameGroup(splitGroups2Struct* params){
	try {
		if (params->m->getControl_pressed()) { return 0; }

        GroupMap groupMap; groupMap.readMap(params->groupfile, params->Groups);
		vector<string> namesGroups = groupMap.getNamesOfGroups();
        if (params->Groups.size() == 0) { params->Groups = namesGroups; }

		//GroupName -> files(fasta, list, group, name)
        for (int i = 0; i < params->Groups.size(); i++) {

            vector<string> files;
            map<string, vector<string> >::iterator it = params->group2Files.find(params->Groups[i]);

            if (it != params->group2Files.end()) { files = it->second; }
            else { params->m->mothurOut("[ERROR]: Can find group " + params->Groups[i] + ", quitting.\n"); params->m->setControl_pressed(true); break; }

			params->m->mothurOut("Processing group: " + params->Groups[i] + "\n");

            vector<string> namesSeqsInThisGroup = groupMap.getNamesSeqs(params->Groups[i]);
            ofstream outGroup, outAccnos;
            params->util.openOutputFile(files[2], outGroup);
            params->util.openOutputFile(files[2]+".accnos", outAccnos);
            for (long long j = 0; j < namesSeqsInThisGroup.size(); j++) {
                outGroup << namesSeqsInThisGroup[j] << '\t' << params->Groups[i] << endl;
                outAccnos << namesSeqsInThisGroup[j] << endl;
            }
            outGroup.close(); outAccnos.close();
            params->outputNames.push_back(files[2]); params->outputTypes["group"].push_back(files[2]);

            //use unique.seqs to create new name and fastafile
            string uniqueFasta = params->fastafile+params->Groups[i];
            string uniqueName = params->namefile+params->Groups[i];
            string uniqueList = params->listfile+params->Groups[i];

            string inputString = "dups=f, accnos=" + files[2]+".accnos";
            if (params->namefile != "") {
                inputString += ", name=" + uniqueName;
                params->util.copyFile(params->namefile, uniqueName);
            }
            if (params->fastafile != "") {
                inputString += ", fasta=" + uniqueFasta;
                params->util.copyFile(params->fastafile, uniqueFasta);
            }
            if (params->listfile != "")  {
                inputString += ", list=" + uniqueList;
                params->util.copyFile(params->listfile, uniqueList);
            }

            params->m->mothurOut("/******************************************/\n");
            params->m->mothurOut("Running command: get.seqs(" + inputString + ")\n");

            Command* getCommand = new GetSeqsCommand(inputString);
            getCommand->execute();
            map<string, vector<string> > filenames = getCommand->getOutputFiles();
            delete getCommand;

            if (params->fastafile != "") {
                params->util.renameFile(filenames["fasta"][0], files[0]);
                params->outputNames.push_back(files[0]); params->outputTypes["fasta"].push_back(files[0]);
                params->util.mothurRemove(uniqueFasta);
            }
            if (params->listfile != "") {
                params->util.renameFile(filenames["list"][0], files[1]);
                params->outputNames.push_back(files[1]); params->outputTypes["list"].push_back(files[1]);
                params->util.mothurRemove(uniqueList);
            }
            if (params->namefile != "") {
                params->util.renameFile(filenames["name"][0], files[3]);
                params->outputNames.push_back(files[3]); params->outputTypes["name"].push_back(files[3]);
            }

            params->m->mothurOut("/******************************************/\nDone.\n");

            params->util.mothurRemove(files[2]+".accnos");
            params->util.mothurRemove(uniqueName);

			if (params->m->getControl_pressed()) {  for (int i = 0; i < params->outputNames.size(); i++) {	params->util.mothurRemove(params->outputNames[i]);	} return 0; }
		}

        return 0;
    }
	catch(exception& e) {
		params->m->errorOut(e, "SplitGroupCommand", "driverRunNameGroup");
		exit(1);
	}
}
//**********************************************************************************************************************
int driverRunCount(splitGroups2Struct* params){
    try {
        CountTable ct; ct.readTable(params->countfile, true, false, params->Groups);
        if (!ct.hasGroupInfo()) { params->m->mothurOut("[ERROR]: your count file does not contain group info, cannot split by group.\n"); params->m->setControl_pressed(true); }

        if (params->m->getControl_pressed()) { return 0; }

        params->Groups = ct.getNamesOfGroups();

        vector<string> listFileOutputNames;
        if (params->listfile != "") { //create output file names
            InputData input(params->listfile, "list", nullVector);
            ListVector* list = input.getListVector();

            vector<string> files;
            map<string, vector<string> >::iterator it = params->group2Files.find(params->Groups[0]);

            if (it != params->group2Files.end()) { files = it->second; }
            else { params->m->mothurOut("[ERROR]: Can find group " + params->Groups[0] + ", quitting.\n"); params->m->setControl_pressed(true);  }

            string listFileRoot = params->outputDir + params->util.getRootName(params->util.getSimpleName(files[1]));
            string listExt = params->util.getExtension(files[1]);

            while(list != nullptr) {

                listFileOutputNames.push_back(listFileRoot + list->getLabel() + listExt);

                delete list;
                list = input.getListVector();
            }
        }

        //GroupName -> files(fasta, list, count)
        for (int i = 0; i < params->Groups.size(); i++) { //Groups only contains the samples assigned to this process

            vector<string> files;
            map<string, vector<string> >::iterator it = params->group2Files.find(params->Groups[i]);

            if (it != params->group2Files.end()) { files = it->second; }
            else { params->m->mothurOut("[ERROR]: Can find group " + params->Groups[i] + ", quitting.\n"); params->m->setControl_pressed(true); break; }

            //print new count file
            string newCountFile = files[2];
            vector<string> tempGroups; tempGroups.push_back(params->Groups[i]);
            ct.printCompressedTable(newCountFile, tempGroups);
            params->outputNames.push_back(newCountFile); params->outputTypes["count"].push_back(newCountFile);
            vector<string> namesOfSeqsInGroup = ct.getNamesOfSeqs(params->Groups[i]);
            unordered_set<string> thisGroupsNames = params->util.mothurConvert(namesOfSeqsInGroup);

            //If more than one distance, use vector of list file outputs
            pair<string, string> fastaFilePair(params->fastafile, files[0]);
            pair<string, vector<string> > listFilePair(params->listfile, listFileOutputNames);

            params->m->mothurOut("/******************************************/\n");
            params->m->mothurOut("Selecting sequences for group " + params->Groups[i] + "\n\n");

            Command* getCommand = new GetSeqsCommand(thisGroupsNames, fastaFilePair, listFilePair, nullStringPair, "");

            delete getCommand;

            if (params->listfile != "") { params->outputNames.insert(params->outputNames.end(), listFileOutputNames.begin(), listFileOutputNames.end()); params->outputTypes["list"].insert(params->outputNames.end(), listFileOutputNames.begin(), listFileOutputNames.end()); }

            params->m->mothurOut("/******************************************/\n\n");

            if (params->fastafile != "") { params->outputNames.push_back(files[0]); params->outputTypes["fasta"].push_back(files[0]); }

            if (params->m->getControl_pressed()) {  for (int i = 0; i < params->outputNames.size(); i++) {	params->util.mothurRemove(params->outputNames[i]);	} break; }
        }

        return 0;
    }
    catch(exception& e) {
        params->m->errorOut(e, "SplitGroupCommand", "driverRunCount");
        exit(1);
    }
}
//**********************************************************************************************************************

//assumes nameToGroup[seq1] -> 1,3 means seq1 should be written to outputFiles[1] and outputFiles[3]

int driverFastaCount(splitGroups3Struct* params){
    try {
        CountTable ct; ct.readTable(params->countfile, true, false, params->Groups);
        if (!ct.hasGroupInfo()) { params->m->mothurOut("[ERROR]: your count file does not contain group info, cannot split by group.\n"); params->m->setControl_pressed(true); }

        if (params->m->getControl_pressed()) { return 0; }

        params->Groups = ct.getNamesOfGroups();
        int numGroups = params->Groups.size();

        for (int i = 0; i < numGroups; i++) { //Groups only contains the samples assigned to this process

            vector<string> outputFastaFiles; string g = ""; int count = 0;
            unordered_map<string, vector<int> > nameToGroup; unordered_map<string, vector<int> >::iterator it;
            string groupNameOutput = ""; vector<string> groups;

            while ((count < params->splitNum) && (i < numGroups) ){

                //print new count file
                string newCountFile = params->fileRootExts[2] + params->Groups[i] + params->fileRootExts[3];
                vector<string> tempGroups; tempGroups.push_back(params->Groups[i]);
                ct.printCompressedTable(newCountFile, tempGroups);
                params->outputNames.push_back(newCountFile); params->outputTypes["count"].push_back(newCountFile);

                vector<string> namesOfSeqsInGroup = ct.getNamesOfSeqs(params->Groups[i]);

                string newFasta = params->fileRootExts[0] + params->Groups[i] + params->fileRootExts[1];
                outputFastaFiles.push_back(newFasta);

                for (string name : namesOfSeqsInGroup) {
                    it = nameToGroup.find(name);

                    if (it != nameToGroup.end()) {
                        (it->second).push_back(count);
                    }else {
                        vector<int> thisSeqsGroups; thisSeqsGroups.push_back(count);
                        nameToGroup[name] = thisSeqsGroups;
                    }
                }

                groups.push_back(params->Groups[i]);

                groupNameOutput += params->Groups[i] + "-";

                count++; i++;
            }

            i--;

            if (groupNameOutput.length() != 0) { groupNameOutput = groupNameOutput.substr(0, groupNameOutput.length()-1); }

            params->m->mothurOut("\nSelecting sequences for groups " + groupNameOutput + "\n\n");

            Command* getCommand = new GetSeqsCommand(nameToGroup, params->fastafile, outputFastaFiles, groups);

            map<string, vector<string> > fileOutputs = getCommand->getOutputFiles();

            delete getCommand;

            map<string, vector<string> >::iterator itOut = fileOutputs.find("fasta");
            if (itOut != fileOutputs.end()) {
                vector<string> o = itOut->second;
                for (string name : o) { params->outputNames.push_back(name); params->outputTypes["fasta"].push_back(name);  }
            }

            if (params->m->getControl_pressed()) {  for (int i = 0; i < params->outputNames.size(); i++) {    params->util.mothurRemove(params->outputNames[i]);    } break; }
        }

        return 0;
    }
    catch(exception& e) {
        params->m->errorOut(e, "SplitGroupCommand", "driverRunCount");
        exit(1);
    }
}
//**********************************************************************************************************************
void SplitGroupCommand::splitCountOrGroup(bool isCount){
    try {
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<splitGroups2Struct*> data;

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            splitGroups2Struct* dataBundle = new splitGroups2Struct(groupfile, countfile, namefile, Groups, lines[i+1].start, lines[i+1].end);
            dataBundle->setFiles(fastafile, listfile, outputdir);
            data.push_back(dataBundle);

            if (isCount) {
                workerThreads.push_back(new std::thread(driverRunCount, dataBundle));
            }else {
                workerThreads.push_back(new std::thread(driverRunNameGroup, dataBundle));
            }
        }

        splitGroups2Struct* dataBundle = new splitGroups2Struct(groupfile, countfile, namefile, Groups, lines[0].start, lines[0].end);
        dataBundle->setFiles(fastafile, listfile, outputdir);
        if (isCount) {
            driverRunCount(dataBundle);
        }else {
            driverRunNameGroup(dataBundle);
        }
        outputNames.insert(outputNames.end(), dataBundle->outputNames.begin(), dataBundle->outputNames.end());
        for (itTypes = dataBundle->outputTypes.begin(); itTypes != dataBundle->outputTypes.end(); itTypes++) {
            outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
        }

        delete dataBundle;

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();

            outputNames.insert(outputNames.end(), data[i]->outputNames.begin(), data[i]->outputNames.end());
            for (itTypes = data[i]->outputTypes.begin(); itTypes != data[i]->outputTypes.end(); itTypes++) {
                outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
            }

            delete data[i];
            delete workerThreads[i];
        }
    }
    catch(exception& e) {
        m->errorOut(e, "SplitGroupCommand", "splitCountOrGroup");
        exit(1);
    }
}
//**********************************************************************************************************************
//splitGroups3Struct(string count, string fasta, string outd, vector<string> g, int st, int en) : countfile(count), start(st), end(en)
void SplitGroupCommand::splitFastaCount(){
    try {
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<splitGroups3Struct*> data;

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            splitGroups3Struct* dataBundle = new splitGroups3Struct(countfile, fastafile, outputdir, Groups, lines[i+1].start, lines[i+1].end);
            data.push_back(dataBundle);

            workerThreads.push_back(new std::thread(driverFastaCount, dataBundle));
        }

        splitGroups3Struct* dataBundle = new splitGroups3Struct(countfile, fastafile, outputdir, Groups, lines[0].start, lines[0].end);

        driverFastaCount(dataBundle);

        outputNames.insert(outputNames.end(), dataBundle->outputNames.begin(), dataBundle->outputNames.end());
        for (itTypes = dataBundle->outputTypes.begin(); itTypes != dataBundle->outputTypes.end(); itTypes++) {
            outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
        }

        delete dataBundle;

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();

            outputNames.insert(outputNames.end(), data[i]->outputNames.begin(), data[i]->outputNames.end());
            for (itTypes = data[i]->outputTypes.begin(); itTypes != data[i]->outputTypes.end(); itTypes++) {
                outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
            }

            delete data[i];
            delete workerThreads[i];
        }
    }
    catch(exception& e) {
        m->errorOut(e, "SplitGroupCommand", "splitFastaCount");
        exit(1);
    }
}
//**********************************************************************************************************************
int driverSplitFlow(splitGroupsStruct* params){
    try {
        GroupMap* groupMap = nullptr;
        CountTable* ct = nullptr;
        vector<string> namesGroups;
        if (params->groupfile != "") {
            groupMap = new GroupMap(params->groupfile);
            groupMap->readMap();
            namesGroups = groupMap->getNamesOfGroups();
        }else if (params->countfile != ""){
            ct = new CountTable();
            ct->readTable(params->countfile, true, true, params->Groups);
            namesGroups = ct->getNamesOfGroups();
        }else { params->m->mothurOut("[ERROR]: you must provide a count or group file to split by group. quitting... \n"); params->m->setControl_pressed(true);  }

        if (params->Groups.size() == 0) { params->Groups = namesGroups; }

        if (params->m->getControl_pressed()) { if (groupMap != nullptr) { delete groupMap; }else if (ct != nullptr) { delete ct; } return 0; }

        string name, flows;
        int count = 0;
        ifstream in; params->util.openInputFile(params->inputFileName, in);
        in >> flows; gobble(in);

        while (!in.eof()) {
            if (params->m->getControl_pressed()) { break; }

            in >> name; gobble(in);
            flows = params->util.getline(in); gobble(in);

            vector<string> thisSeqsGroups;
            if (groupMap != nullptr) {
                string thisGroup = groupMap->getGroup(name);
                thisSeqsGroups.push_back(thisGroup);
            }else if (ct != nullptr) { if (ct->inTable(name)) { thisSeqsGroups  = ct->getGroups(name); } }

            for (int i = 0; i < thisSeqsGroups.size(); i++) {

                map<string, flowOutput>::iterator it = params->parsedFlowData.find(thisSeqsGroups[i]);

                if (it != params->parsedFlowData.end()) {
                    it->second.total++; it->second.output += name + ' ' + flows + '\n';
                    if (it->second.total % 100 == 0) { //buffer write
                        ofstream out; params->util.openOutputFileAppend(it->second.filename, out);
                        out << it->second.output; it->second.output = ""; out.close();
                    }
                } //else not in the groups we are looking to parse, so ignore
            }

            count++;
        }

        //output rest
        for (map<string, flowOutput>::iterator it = params->parsedFlowData.begin(); it != params->parsedFlowData.end(); it++) {
            if (params->m->getControl_pressed()) { break; }

            if (it->second.output != "") { //more seqs to output
                ofstream out; params->util.openOutputFileAppend(it->second.filename, out);
                out << it->second.output; it->second.output = ""; params->outputNames.push_back(it->second.filename); params->outputTypes["flow"].push_back(it->second.filename);
            }else if (it->second.total == 0) { //no seqs for this group, remove file
                params->util.mothurRemove(it->second.filename);
            }else { //finished writing, just add to list of output files
                params->outputNames.push_back(it->second.filename); params->outputTypes["flow"].push_back(it->second.filename);
            }
        }

        if (params->m->getControl_pressed()) { if (groupMap != nullptr) { delete groupMap; }else if (ct != nullptr) { delete ct; } return 0; }

        return count;
    }
    catch(exception& e) {
        params->m->errorOut(e, "SplitGroupCommand", "driverSplitFlow");
        exit(1);
    }
}

//**********************************************************************************************************************
int driverSplitFastq(splitGroupsStruct* params){
    try {
        GroupMap* groupMap = nullptr;
        CountTable* ct = nullptr;
        vector<string> namesGroups;
        if (params->groupfile != "") {
            groupMap = new GroupMap(params->groupfile);
            groupMap->readMap();
            namesGroups = groupMap->getNamesOfGroups();
        }else if (params->countfile != ""){
            ct = new CountTable();
            ct->readTable(params->countfile, true, true, params->Groups);
            namesGroups = ct->getNamesOfGroups();
        }else { params->m->mothurOut("[ERROR]: you must provide a count or group file to split by group. quitting... \n"); params->m->setControl_pressed(true);  return 0; }

        if (params->m->getControl_pressed()) { if (groupMap != nullptr) { delete groupMap; }else if (ct != nullptr) { delete ct; } return 0; }

        int count = 0;
        ifstream in; params->util.openInputFile(params->inputFileName, in);

        while (!in.eof()) {
            if (params->m->getControl_pressed()) { break; }

            bool ignore = false;
            FastqRead thisRead(in, ignore, params->format); gobble(in);
            string name = thisRead.getName();

            vector<string> thisSeqsGroups;
            if (groupMap != nullptr) {
                string thisGroup = groupMap->getGroup(name);
                thisSeqsGroups.push_back(thisGroup);
            }else if (ct != nullptr) { if (ct->inTable(name)) { thisSeqsGroups  = ct->getGroups(name); } }

            for (int i = 0; i < thisSeqsGroups.size(); i++) {

                map<string, fastqOutput>::iterator it = params->parsedFastqData.find(thisSeqsGroups[i]);

                if (it != params->parsedFastqData.end()) {
                    it->second.total++; it->second.output.push_back(thisRead);
                    if (it->second.total % 500 == 0) { //buffer write
                        ofstream out; params->util.openOutputFileAppend(it->second.filename, out);
                        for (int j = 0; j < it->second.output.size(); j++) { it->second.output[j].printFastq(out); }
                        it->second.output.clear(); out.close();
                    }
                } //else not in the groups we are looking to parse, so ignore
            }

            count++;
        }

        //output rest
        for (map<string, fastqOutput>::iterator it = params->parsedFastqData.begin(); it != params->parsedFastqData.end(); it++) {
            if (params->m->getControl_pressed()) { break; }

            if (it->second.output.size() != 0) { //more seqs to output
                ofstream out; params->util.openOutputFileAppend(it->second.filename, out);
                for (int j = 0; j < it->second.output.size(); j++) { it->second.output[j].printFastq(out); }
                it->second.output.clear(); out.close();
                params->outputNames.push_back(it->second.filename); params->outputTypes["fastq"].push_back(it->second.filename);
            }else if (it->second.total == 0) { //no seqs for this group, remove file
                params->util.mothurRemove(it->second.filename);
            }else { //finished writing, just add to list of output files
                params->outputNames.push_back(it->second.filename); params->outputTypes["fastq"].push_back(it->second.filename);
            }
        }

        if (params->m->getControl_pressed()) { if (groupMap != nullptr) { delete groupMap; }else if (ct != nullptr) { delete ct; } return 0; }

        return count;

    }
    catch(exception& e) {
        params->m->errorOut(e, "SplitGroupCommand", "driverSplitFastq");
        exit(1);
    }
}
//**********************************************************************************************************************
void SplitGroupCommand::splitFastqOrFlow(string inputFile, string extension){
    try {
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<splitGroupsStruct*> data;

        string outputfileRoot = outputdir + util.getRootName(util.getSimpleName(inputFile));

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            splitGroupsStruct* dataBundle = new splitGroupsStruct(groupfile, countfile, namefile, Groups, lines[i+1].start, lines[i+1].end);
            dataBundle->setFiles(inputFile, outputfileRoot, extension);
            dataBundle->setFormat(format);
            data.push_back(dataBundle);

            if (extension == ".fastq") {
                workerThreads.push_back(new std::thread(driverSplitFastq, dataBundle));
            }else {
                workerThreads.push_back(new std::thread(driverSplitFlow, dataBundle));
            }
        }

        splitGroupsStruct* dataBundle = new splitGroupsStruct(groupfile, countfile, namefile, Groups, lines[0].start, lines[0].end);
        dataBundle->setFiles(inputFile, outputfileRoot, extension);
        dataBundle->setFormat(format);

        if (extension == ".fastq")  { driverSplitFastq(dataBundle); }
        else                        { driverSplitFlow(dataBundle);   }

        outputNames.insert(outputNames.end(), dataBundle->outputNames.begin(), dataBundle->outputNames.end());
        for (itTypes = dataBundle->outputTypes.begin(); itTypes != dataBundle->outputTypes.end(); itTypes++) {
            outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
        }

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();

            outputNames.insert(outputNames.end(), data[i]->outputNames.begin(), data[i]->outputNames.end());
            for (itTypes = data[i]->outputTypes.begin(); itTypes != data[i]->outputTypes.end(); itTypes++) {
                outputTypes[itTypes->first].insert(outputTypes[itTypes->first].end(), itTypes->second.begin(), itTypes->second.end());
            }

            delete data[i];
            delete workerThreads[i];
        }

        delete dataBundle;
    }
    catch(exception& e) {
        m->errorOut(e, "SplitGroupCommand", "splitFastqOrFlow");
        exit(1);
    }
}
//**********************************************************************************************************************



