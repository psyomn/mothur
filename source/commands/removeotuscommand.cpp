//
//  removeotulabels.cpp
//  Mothur
//
//  Created by Sarah Westcott on 5/21/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "removeotuscommand.h"

//**********************************************************************************************************************
vector<string> RemoveOtusCommand::setParameters(){	
	try {
        CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(paccnos);
        CommandParameter pconstaxonomy("constaxonomy", "InputTypes", "", "", "none", "FNGLT", "none","constaxonomy",false,false); parameters.push_back(pconstaxonomy);
		CommandParameter potucorr("otucorr", "InputTypes", "", "", "none", "FNGLT", "none","otucorr",false,false); parameters.push_back(potucorr);
        CommandParameter pcorraxes("corraxes", "InputTypes", "", "", "none", "FNGLT", "none","corraxes",false,false); parameters.push_back(pcorraxes);
        CommandParameter plist("list", "InputTypes", "", "", "none", "FNGLT", "none","list",false,false, true); parameters.push_back(plist);
        CommandParameter pshared("shared", "InputTypes", "", "", "none", "FNGLT", "none","shared",false,false, true); parameters.push_back(pshared);
        CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
        CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["constaxonomy"] = tempOutNames;
        outputTypes["otucorr"] = tempOutNames;
        outputTypes["corraxes"] = tempOutNames;
        outputTypes["shared"] = tempOutNames;
        outputTypes["list"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveOtusCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The remove.otus command can be used to remove specific otus with the output from classify.otu, otu.association, or corr.axes. It can also be used to select a set of otus from a shared or list file.\n";
		helpString += "The remove.otus parameters are: constaxonomy, otucorr, corraxes, shared, list, label and accnos.\n";
		helpString += "The constaxonomy parameter is input the results of the classify.otu command.\n";
        helpString += "The otucorr parameter is input the results of the otu.association command.\n";
        helpString += "The corraxes parameter is input the results of the corr.axes command.\n";
        helpString += "The label parameter is used to analyze specific labels in your input. \n";
		helpString += "The remove.otus commmand should be in the following format: \n";
		helpString += "remove.otus(accnos=yourListOfOTULabels, corraxes=yourCorrAxesFile)\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string RemoveOtusCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "constaxonomy")         {   pattern = "[filename],pick,[extension]";                }
        else if (type == "otucorr")         {   pattern = "[filename],pick,[extension]";                }
        else if (type == "corraxes")        {   pattern = "[filename],pick,[extension]";                }
        else if (type == "list")            {   pattern = "[filename],[distance],pick,[extension]";     }
        else if (type == "shared")          {   pattern = "[filename],[distance],pick,[extension]";     }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "RemoveOtusCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
RemoveOtusCommand::RemoveOtusCommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
            accnosfile = validParameter.validFile(parameters, "accnos");
			if (accnosfile == "not open") { abort = true; }
			else if (accnosfile == "not found") {  
				accnosfile = current->getAccnosFile(); 
				if (accnosfile != "") {  m->mothurOut("Using " + accnosfile + " as input file for the accnos parameter.\n");  }
				else { 
					m->mothurOut("You have no valid accnos file and accnos is required.\n");  
					abort = true;
				} 
			}else { current->setAccnosFile(accnosfile); }	
			
			constaxonomyfile = validParameter.validFile(parameters, "constaxonomy");
			if (constaxonomyfile == "not open") { constaxonomyfile = ""; abort = true; }
			else if (constaxonomyfile == "not found") {  constaxonomyfile = "";  }
            
            corraxesfile = validParameter.validFile(parameters, "corraxes");
			if (corraxesfile == "not open") { corraxesfile = ""; abort = true; }
			else if (corraxesfile == "not found") {  corraxesfile = "";  }
            
            otucorrfile = validParameter.validFile(parameters, "otucorr");
			if (otucorrfile == "not open") { otucorrfile = ""; abort = true; }
			else if (otucorrfile == "not found") {  otucorrfile = "";  }
            
            listfile = validParameter.validFile(parameters, "list");
			if (listfile == "not open") { listfile = ""; abort = true; }
			else if (listfile == "not found") {  listfile = "";  }
            else { current->setListFile(listfile); }
            
            sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }
			else if (sharedfile == "not found") {  sharedfile = "";  }
            else { current->setSharedFile(sharedfile); }
            
            
            if ((constaxonomyfile == "") && (corraxesfile == "") && (otucorrfile == "") && (sharedfile == "") && (listfile == ""))  { m->mothurOut("You must provide one of the following: constaxonomy, corraxes, otucorr, shared or list.\n");  abort = true; }
            
            if ((sharedfile != "") || (listfile != "")) {
                label = validParameter.valid(parameters, "label");			
                if (label == "not found") { label = ""; m->mothurOut("You did not provide a label, I will use the first label in your inputfile.\n");  label=""; }
            }
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "RemoveOtusCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int RemoveOtusCommand::execute(){
	try {
		
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        //get labels you want to keep
		otulabels = util.readAccnos(accnosfile);
        //simplfy labels
        unordered_set<string> newLabels;
        for (auto it = otulabels.begin(); it != otulabels.end(); it++) {  newLabels.insert(util.getSimpleLabel(*it)); }
        otulabels = newLabels;
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: numlabels = " + toString(otulabels.size()) + "\n"); }
		
		if (m->getControl_pressed()) { return 0; }
		
		//read through the correct file and output lines you want to keep
		if (constaxonomyfile != "")	{		readClassifyOtu();      }
		if (corraxesfile != "")		{		readCorrAxes();         }
		if (otucorrfile != "")		{		readOtuAssociation();	}
        if (listfile != "")         {		readList();             }
        if (sharedfile != "")		{		readShared();           }
        
        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); }  return 0; }
        
        //output files created by command
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
        
        string currentName = "";
        itTypes = outputTypes.find("list");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setListFile(currentName); }
        }
        
        itTypes = outputTypes.find("shared");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setSharedFile(currentName); }
        }
        
        //set constaxonomy file as new current constaxonomyfile
        itTypes = outputTypes.find("constaxonomy");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setConsTaxonomyFile(currentName); }
        }
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "GetOtusCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveOtusCommand::readClassifyOtu(){
	try {
		string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(constaxonomyfile);  }
		map<string, string> variables; 
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(constaxonomyfile));
        variables["[extension]"] = util.getExtension(constaxonomyfile);
		string outputFileName = getOutputFileName("constaxonomy", variables);
		
        ofstream out; util.openOutputFile(outputFileName, out);
		ifstream in; util.openInputFile(constaxonomyfile, in);
		
		bool wroteSomething = false;
		int removedCount = 0;
		
        //read headers
        string headers = util.getline(in); gobble(in);
        out << headers << endl;
        
        while (!in.eof()) {
            
            if (m->getControl_pressed()) { break; }
            
            string otu = ""; string tax = "unknown";
            int size = 0;
            
            in >> otu >> size; gobble(in);
            tax = util.getline(in); gobble(in);
            
            if (m->getDebug()) { m->mothurOut("[DEBUG]: " + otu + toString(size) + tax + "\n"); }
            
            if (otulabels.count(util.getSimpleLabel(otu)) == 0) {
				wroteSomething = true;
                out << otu << '\t' << size << '\t' << tax << endl;
            }else {  removedCount++;  }
        }
        in.close();
        out.close();
		
		if (wroteSomething == false) { m->mothurOut("Your file only contains labels from the .accnos file.\n");   }
		outputNames.push_back(outputFileName);  outputTypes["constaxonomy"].push_back(outputFileName);
		
		m->mothurOut("Removed " + toString(removedCount) + " otus from your constaxonomy file.\n"); 
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "readClassifyOtu");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveOtusCommand::readOtuAssociation(){
	try {
		string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(otucorrfile);  }
        map<string, string> variables; 
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(otucorrfile));
        variables["[extension]"] = util.getExtension(otucorrfile);
		string outputFileName = getOutputFileName("otucorr", variables);
		
        ofstream out; util.openOutputFile(outputFileName, out);
		ifstream in; util.openInputFile(otucorrfile, in);
		
		bool wroteSomething = false;
		int removedCount = 0;
		
        //read headers
        string headers = util.getline(in); gobble(in);
        out << headers << endl;
        
        while (!in.eof()) {
            
            if (m->getControl_pressed()) { break; }
            
            string otu1 = ""; 
            string otu2 = ""; 
            in >> otu1 >> otu2;
            string line = util.getline(in); gobble(in);
            
            if ((otulabels.count(util.getSimpleLabel(otu1)) == 0) && (otulabels.count(util.getSimpleLabel(otu2)) == 0)){
				wroteSomething = true;
                
                out << otu1 << '\t' << otu2 << '\t' << line << endl;
            }else { removedCount++; }
        }
        in.close();
        out.close();
		
		if (wroteSomething == false) { m->mothurOut("Your file only contains labels from the .accnos file.\n");   }
		outputNames.push_back(outputFileName);  outputTypes["otucorr"].push_back(outputFileName);
		
		m->mothurOut("Removed " + toString(removedCount) + " lines from your otu.corr file.\n"); 
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "readOtuAssociation");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveOtusCommand::readCorrAxes(){
	try {
		string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(corraxesfile);  }
        map<string, string> variables; 
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(corraxesfile));
        variables["[extension]"] = util.getExtension(corraxesfile);
		string outputFileName = getOutputFileName("corraxes", variables);
		
        ofstream out; util.openOutputFile(outputFileName, out);
		ifstream in; util.openInputFile(corraxesfile, in);
		
		bool wroteSomething = false;
		int removedCount = 0;
		
        //read headers
        string headers = util.getline(in); gobble(in);
        out << headers << endl;
        
        while (!in.eof()) {
            
            if (m->getControl_pressed()) { break; }
            
            string otu = ""; 
            in >> otu;
            string line = util.getline(in); gobble(in);
            
            if (otulabels.count(util.getSimpleLabel(otu)) == 0) {
				wroteSomething = true;
                
                out << otu << '\t' << line << endl;
            }else { removedCount++; }
        }
        in.close();
        out.close();
		
		if (wroteSomething == false) { m->mothurOut("Your file only contains labels from the .accnos file.\n");   }
		outputNames.push_back(outputFileName);  outputTypes["corraxes"].push_back(outputFileName);
		
		m->mothurOut("Removed " + toString(removedCount) + " lines from your corr.axes file.\n"); 
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "readCorrAxes");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveOtusCommand::readShared(){
	try {
        
        SharedRAbundVectors* lookup = getShared();
        
        if (m->getControl_pressed()) { delete lookup; return 0; }
        
        vector<string> newLabels;

        bool wroteSomething = false;
        int numRemoved = 0;
        vector<int> binsToRemove;
        for (int i = 0; i < lookup->getNumBins(); i++) {
            
            if (m->getControl_pressed()) { delete lookup; return 0; }
            
            //is this otu on the list
            if (otulabels.count(util.getSimpleLabel(lookup->getOTUNames()[i])) == 0) {
                wroteSomething = true;
            }else { numRemoved++; binsToRemove.push_back(i); }
        }
        
        lookup->removeOTUs(binsToRemove, true);

        string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(sharedfile);  }
        map<string, string> variables; 
		variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(sharedfile));
        variables["[extension]"] = util.getExtension(sharedfile);
        variables["[distance]"] = lookup->getLabel();
		string outputFileName = getOutputFileName("shared", variables);
        outputTypes["shared"].push_back(outputFileName);  outputNames.push_back(outputFileName);
        ofstream out; util.openOutputFile(outputFileName, out);  bool printHeaders = true;
        lookup->print(out, printHeaders);
        out.close();
        
        delete lookup;
        
        if (wroteSomething == false) { m->mothurOut("Your file contains only OTUs from the .accnos file.\n");  }
		m->mothurOut("Removed " + toString(numRemoved) + " OTUs from your shared file.\n");
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "readShared");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveOtusCommand::readList(){
	try {
        getListVector();
        
        if (m->getControl_pressed()) { delete list; return 0;}
        
        ListVector newList;
        newList.setLabel(list->getLabel());
        int removedCount = 0;
        bool wroteSomething = false;
        
        vector<string> binLabels = list->getLabels();
        vector<string> newLabels;
        for (int i = 0; i < list->getNumBins(); i++) {
            
            if (m->getControl_pressed()) { delete list; return 0;}
            
            if (otulabels.count(util.getSimpleLabel(binLabels[i])) == 0) {
                newList.push_back(list->get(i));
                newLabels.push_back(binLabels[i]);
            }else { removedCount++; }
        }
        
        string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(listfile);  }
        map<string, string> variables; 
		variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(listfile));
        variables["[extension]"] = util.getExtension(listfile);
        variables["[distance]"] = list->getLabel();
		string outputFileName = getOutputFileName("list", variables);
		ofstream out;
		util.openOutputFile(outputFileName, out);
        
		delete list;
        //print new listvector
        if (newList.getNumBins() != 0) {
            wroteSomething = true;
            newList.setLabels(newLabels);
            newList.print(out, false);
        }
		out.close();
		
		if (wroteSomething == false) { m->mothurOut("Your file contains only OTUs from the .accnos file.\n");   }
		outputNames.push_back(outputFileName); outputTypes["list"].push_back(outputFileName);
		
		m->mothurOut("Removed " + toString(removedCount) + " OTUs from your list file.\n"); 
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "RemoveOtusCommand", "readList");
        exit(1);
    }
}
//**********************************************************************************************************************
int RemoveOtusCommand::getListVector(){
	try {
		InputData input(listfile, "list", nullVector);
		list = input.getListVector();
		string lastLabel = list->getLabel();
		
		if (label == "") { label = lastLabel;  return 0; }
		
		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> labels; labels.insert(label);
		set<string> processedLabels;
		set<string> userLabels = labels;
		
		//as long as you are not at the end of the file or done wih the lines you want
		while((list != nullptr) && (userLabels.size() != 0)) {
			if (m->getControl_pressed()) {  return 0;  }
			
			if(labels.count(list->getLabel()) == 1){
				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
				break;
			}
			
			if ((util.anyLabelsToProcess(list->getLabel(), userLabels, "") ) && (processedLabels.count(lastLabel) != 1)) {
				string saveLabel = list->getLabel();
				
				delete list;
				list = input.getListVector(lastLabel);
				
				processedLabels.insert(list->getLabel());
				userLabels.erase(list->getLabel());
				
				//restore real lastlabel to save below
				list->setLabel(saveLabel);
				break;
			}
			
			lastLabel = list->getLabel();			
			
			//get next line to process
			//prevent memory leak
			delete list; 
			list = input.getListVector();
		}
		
		
		if (m->getControl_pressed()) {  return 0;  }
		
		//output error messages about any remaining user labels
		bool needToRun = false;
		for (set<string>::iterator it = userLabels.begin(); it != userLabels.end(); it++) {
			m->mothurOut("Your file does not include the label " + *it); 
            if (processedLabels.count(lastLabel) != 1)  { m->mothurOut(". I will use " + lastLabel + ".\n"); needToRun = true;  }
			else                                        { m->mothurOut(". Please refer to " + lastLabel + ".\n");               }
		}
		
		//run last label if you need to
		if (needToRun )  {
			delete list; 
			list = input.getListVector(lastLabel);
		}	
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "getListVector");	
		exit(1);
	}
}
//**********************************************************************************************************************
SharedRAbundVectors* RemoveOtusCommand::getShared(){
	try {
		InputData input(sharedfile, "sharedfile",nullVector);
		SharedRAbundVectors* lookup = input.getSharedRAbundVectors();
		string lastLabel = lookup->getLabel();
        
		if (label == "") { label = lastLabel;  return lookup; }
        
        //if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
        set<string> labels; labels.insert(label);
        set<string> processedLabels;
        set<string> userLabels = labels;
        
        //as long as you are not at the end of the file or done wih the lines you want
        while((lookup != nullptr) && (userLabels.size() != 0)) {
            if (m->getControl_pressed()) {   delete lookup; return nullptr;  }
            
            if(labels.count(lookup->getLabel()) == 1){
                processedLabels.insert(lookup->getLabel());
                userLabels.erase(lookup->getLabel());
                break;
            }
            
            if ((util.anyLabelsToProcess(lookup->getLabel(), userLabels, "") ) && (processedLabels.count(lastLabel) != 1)) {
                string saveLabel = lookup->getLabel();
                
                delete lookup;
                lookup = input.getSharedRAbundVectors(lastLabel);
                
                processedLabels.insert(lookup->getLabel());
                userLabels.erase(lookup->getLabel());
                
                //restore real lastlabel to save below
                lookup->setLabels(saveLabel);
                break;
            }
            
            lastLabel = lookup->getLabel();
            
            //get next line to process
            //prevent memory leak
            delete lookup;
            lookup = input.getSharedRAbundVectors();
        }
        
        
        if (m->getControl_pressed()) {  return 0;  }
        
        //output error messages about any remaining user labels
        set<string>::iterator it;
        bool needToRun = false;
        for (it = userLabels.begin(); it != userLabels.end(); it++) {
            m->mothurOut("Your file does not include the label " + *it);
            if (processedLabels.count(lastLabel) != 1) {
                m->mothurOut(". I will use " + lastLabel + ".\n"); 
                needToRun = true;
            }else {
                m->mothurOut(". Please refer to " + lastLabel + ".\n"); 
            }
        }
        
        //run last label if you need to
        if (needToRun )  {
            delete lookup;
            lookup = input.getSharedRAbundVectors(lastLabel);
        }	
		
		return lookup;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveOtusCommand", "getShared");	
		exit(1);
	}
}
//**********************************************************************************************************************



