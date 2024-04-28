/*
 *  clusterdoturcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/27/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clusterdoturcommand.h"
#include "clusterclassic.h"

//**********************************************************************************************************************
vector<string> ClusterDoturCommand::setParameters(){	
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none","list",false,true,true); parameters.push_back(pphylip);
		CommandParameter pname("name", "InputTypes", "", "", "namecount", "none", "none","rabund-sabund",false,false,true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "namecount", "none", "none","",false,false,true); parameters.push_back(pcount);
		CommandParameter pcutoff("cutoff", "Number", "", "10", "", "", "","",false,false,true); parameters.push_back(pcutoff);
		CommandParameter pprecision("precision", "Number", "", "100", "", "", "","",false,false); parameters.push_back(pprecision);
		CommandParameter pmethod("method", "Multiple", "furthest-nearest-average-weighted", "average", "", "", "","",false,false); parameters.push_back(pmethod);
		CommandParameter psim("sim", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(psim);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["list"] = tempOutNames;
        outputTypes["rabund"] = tempOutNames;
        outputTypes["sabund"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterDoturCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterDoturCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The cluster.classic command clusters using the algorithm from dotur. \n";
		helpString += "The cluster.classic command parameter options are phylip, name, count, method, cuttoff, sim, precision. Phylip is required, unless you have a valid current file.\n";
		helpString += "The cluster.classic command should be in the following format: \n";
		helpString += "cluster.classic(phylip=yourDistanceMatrix, method=yourMethod, cutoff=yourCutoff, precision=yourPrecision) \n";
		helpString += "The acceptable cluster methods are furthest, nearest, weighted and average.  If no method is provided then average is assumed.\n";	
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterDoturCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterDoturCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "list") {  pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; } 
        else if (type == "rabund") {  pattern = "[filename],[clustertag],rabund"; } 
        else if (type == "sabund") {  pattern = "[filename],[clustertag],sabund"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ClusterDoturCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
ClusterDoturCommand::ClusterDoturCommand(string option) : Command()  {
	try{
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			
			
			//check for required parameters
			phylipfile = validParameter.validFile(parameters, "phylip");
			if (phylipfile == "not open") { abort = true; }
			else if (phylipfile == "not found") { 
				phylipfile = current->getPhylipFile(); 
				if (phylipfile != "") {  m->mothurOut("Using " + phylipfile + " as input file for the phylip parameter.\n"); }
				else { 
					m->mothurOut("You need to provide a phylip file with the cluster.classic command.\n");abort = true;
				}	
			}else { current->setPhylipFile(phylipfile); }	

			//check for optional parameter and set defaults
			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { abort = true; namefile = ""; }	
			else if (namefile == "not found") { namefile = ""; }
			else { current->setNameFile(namefile); }
            
            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not open") { abort = true; countfile = ""; }	
			else if (countfile == "not found") { countfile = ""; }
			else { current->setCountFile(countfile); }
			
            if ((countfile != "") && (namefile != "")) { m->mothurOut("When executing a cluster.classic command you must enter ONLY ONE of the following: count or name.\n");  abort = true; }
            
			string temp;
			temp = validParameter.valid(parameters, "precision");
			if (temp == "not found") { temp = "100"; }
			//saves precision legnth for formatting below
			length = temp.length();
			util.mothurConvert(temp, precision); 
			
			temp = validParameter.valid(parameters, "cutoff");
			if (temp == "not found") { temp = "10"; }
			util.mothurConvert(temp, cutoff);
			
			temp = validParameter.valid(parameters, "sim");				if (temp == "not found") { temp = "F"; }
			sim = util.isTrue(temp); 
			
			method = validParameter.valid(parameters, "method");
			if (method == "not found") { method = "average"; }
			
			if ((method == "furthest") || (method == "nearest") || (method == "average") || (method == "weighted")) { 
				if (method == "furthest") { tag = "fn"; }
				else if (method == "nearest") { tag = "nn"; }
				else if (method == "average") { tag = "an"; }
				else if (method == "weighted") { tag = "wn"; }
			}else { m->mothurOut("Not a valid clustering method.  Valid clustering algorithms are furthest, nearest, average, weighted.\n");  abort = true; }
		}
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterDoturCommand", "ClusterCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int ClusterDoturCommand::execute(){
	try {
	
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
		
        
        ClusterClassic* cluster = new ClusterClassic(cutoff, method, sim);
        
        NameAssignment* nameMap = nullptr;
        CountTable* ct = nullptr;
        map<string, int> counts;
        if(namefile != "") {	
			nameMap = new NameAssignment(namefile);
			nameMap->readMap();
            cluster->readPhylipFile(phylipfile, nameMap);
            delete nameMap;
		}else if (countfile != "") {
            ct = new CountTable();
            ct->readTable(countfile, false, false);
            cluster->readPhylipFile(phylipfile, ct);
            counts = ct->getNameMap();
            delete ct;
        }else {
            cluster->readPhylipFile(phylipfile, nameMap);
        }
        tag = cluster->getTag();
        
		if (m->getControl_pressed()) { delete cluster; return 0; }
		
		list = cluster->getListVector();
		rabund = cluster->getRAbundVector();
								
		if (outputdir == "") { outputdir += util.hasPath(phylipfile); }
		fileroot = outputdir + util.getRootName(util.getSimpleName(phylipfile));
			
        map<string, string> variables; 
        variables["[filename]"] = fileroot;
        variables["[clustertag]"] = tag;
        string sabundFileName = getOutputFileName("sabund", variables);
        string rabundFileName = getOutputFileName("rabund", variables);
        //if (countfile != "") { variables["[tag2]"] = "unique_list"; }
        string listFileName = getOutputFileName("list", variables);
        
        if (countfile == "") {
            util.openOutputFile(sabundFileName,	sabundFile);
            util.openOutputFile(rabundFileName,	rabundFile);
            outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
            outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);
            
        }
		util.openOutputFile(listFileName,	listFile);
        outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);
		
		float previousDist = 0.00000;
		float rndPreviousDist = 0.00000;
		oldRAbund = *rabund;
		oldList = *list;
        bool printHeaders = true;
		
        int estart = time(nullptr); 
	
		while ((cluster->getSmallDist() <= cutoff) && (cluster->getNSeqs() > 1)){
			if (m->getControl_pressed()) { delete cluster; delete list; delete rabund; if(countfile == "") {rabundFile.close(); sabundFile.close();  util.mothurRemove((fileroot+ tag + ".rabund")); util.mothurRemove((fileroot+ tag + ".sabund")); }
                listFile.close(); util.mothurRemove((fileroot+ tag + ".list")); outputTypes.clear();  return 0;  }
		
			cluster->update(cutoff);
	
			float dist = cluster->getSmallDist();
			float rndDist = util.ceilDist(dist, precision);
            
			if(previousDist <= 0.0000 && dist != previousDist)  { printData("unique", counts, printHeaders);                                }
			else if(rndDist != rndPreviousDist)                 { printData(toString(rndPreviousDist), counts, printHeaders);    }
		
			previousDist = dist;
			rndPreviousDist = rndDist;
			oldRAbund = *rabund;
			oldList = *list;
		}
	
		if(previousDist <= 0.0000)          { printData("unique", counts, printHeaders);                            }
		else if(rndPreviousDist<cutoff)     { printData(toString(rndPreviousDist), counts, printHeaders); }
		
        if (countfile == "") {
            sabundFile.close();
            rabundFile.close();
        }
		listFile.close();
		
		delete cluster;  delete list; delete rabund;
		
		//set list file as new current listfile
		string currentName = "";
		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setListFile(currentName); } }
		
		//set rabund file as new current rabundfile
		itTypes = outputTypes.find("rabund");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setRabundFile(currentName); } }
		
		//set sabund file as new current sabundfile
		itTypes = outputTypes.find("sabund");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setSabundFile(currentName); } }
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		m->mothurOut("It took " + toString(time(nullptr) - estart) + " seconds to cluster\n"); 

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterDoturCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************

void ClusterDoturCommand::printData(string label, map<string, int>& counts, bool& ph){
	try {
        oldList.setPrintedLabels(ph); ph = false;
        
        oldRAbund.setLabel(label);
        if (countfile == "") {
            oldRAbund.print(rabundFile);
            oldRAbund.getSAbundVector().print(sabundFile);
        }

		oldRAbund.getSAbundVector().print(cout);
		
		oldList.setLabel(label);
        if(countfile != "") {
            oldList.print(listFile, counts);
        }else {
            oldList.print(listFile, true);
        }
        ph = false;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterDoturCommand", "printData");
		exit(1);
	}
}
//**********************************************************************************************************************
