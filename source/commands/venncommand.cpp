/*
 *  venncommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 3/30/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "venncommand.h"

#include "inputdata.h"
#include "calculators/ace.h"
#include "calculators/sobs.h"
#include "calculators/chao1.h"
//#include "jackknife.h"
#include "calculators/sharedsobscollectsummary.h"
#include "calculators/sharedchao1.h"
#include "calculators/sharedace.h"
#include "calculators/nseqs.h"
#include "validcalculator.h"

//**********************************************************************************************************************
vector<string> VennCommand::setParameters(){	
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none","svg",false,false,true); parameters.push_back(plist);
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none","svg",false,false,true); parameters.push_back(pshared);	
		CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
		CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
		CommandParameter pcalc("calc", "String", "", "", "", "", "","",false,false); parameters.push_back(pcalc);
		CommandParameter pabund("abund", "Number", "", "10", "", "", "","",false,false); parameters.push_back(pabund);
		CommandParameter pnseqs("nseqs", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pnseqs);
        CommandParameter psharedotus("sharedotus", "Boolean", "", "t", "", "", "","",false,false); parameters.push_back(psharedotus);
		CommandParameter pfontsize("fontsize", "Number", "", "24", "", "", "","",false,false); parameters.push_back(pfontsize);
        CommandParameter ppermute("permute", "Multiple", "1-2-3-4", "4", "", "", "","",false,false); parameters.push_back(ppermute);		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        vector<string> tempOutNames;
        outputTypes["svg"] = tempOutNames;
        
        abort = false; calledHelp = false;   allLines = true;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string VennCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The venn command parameters are list, shared, groups, calc, abund, nseqs, permute, sharedotus, fontsize and label.   shared, relabund, list, rabund or sabund is required unless you have a valid current file.\n";
		helpString += "The groups parameter allows you to specify which of the groups in your groupfile you would like included in your venn diagram, you may only use a maximum of 4 groups.\n";
		helpString += "The group names are separated by dashes. The label allows you to select what distance levels you would like a venn diagram created for, and are also separated by dashes.\n";
		helpString += "The fontsize parameter allows you to adjust the font size of the picture created, default=24.\n";
		helpString += "The venn command should be in the following format: venn(groups=yourGroups, calc=yourCalcs, label=yourLabels, abund=yourAbund).\n";
		helpString += "Example venn(groups=A-B-C, calc=sharedsobs-sharedchao, abund=20).\n";
		helpString += "The default value for groups is all the groups in your groupfile up to 4, and all labels in your inputfile will be used.\n";
		helpString += "The default value for calc is sobs if you have only read a list file or if you have selected only one group, and sharedsobs if you have multiple groups.\n";
		helpString += "The default available estimators for calc are sobs, chao and ace if you have only read a list file, and sharedsobs, sharedchao and sharedace if you have read a shared file.\n";
		helpString += "The nseqs parameter will output the number of sequences represented by the otus in the picture, default=F.\n";
		helpString += "If you have more than 4 groups, you can use the permute parameter to set the number of groups you would like mothur to divide the samples into to draw the venn diagrams for all possible combos. Default=4.\n";
		helpString += "The only estimators available four 4 groups are sharedsobs and sharedchao.\n";
        helpString += "The sharedotus parameter can be used with the sharedsobs calculator to get the names of the OTUs in each section of the venn diagram. Default=t.\n";
		helpString += "The venn command outputs a .svg file for each calculator you specify at each distance you choose.\n";
		
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string VennCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "svg") {  pattern = "[filename],svg"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "VennCommand", "getOutputPattern");
        exit(1);
    }
}

//**********************************************************************************************************************
VennCommand::VennCommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			listfile = validParameter.validFile(parameters, "list");
			if (listfile == "not open") { listfile = ""; abort = true; }
			else if (listfile == "not found") { listfile = ""; }
			else {  format = "list"; inputfile = listfile; current->setListFile(listfile); }
			
			sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }	
			else if (sharedfile == "not found") { sharedfile = ""; }
			else {  format = "sharedfile"; inputfile = sharedfile; current->setSharedFile(sharedfile); }
			
			if ((sharedfile == "") && (listfile == "")) { 
				//is there are current file available for any of these?
				//give priority to shared, then list, then rabund, then sabund
				//if there is a current shared file, use it
				sharedfile = current->getSharedFile(); 
				if (sharedfile != "") { inputfile = sharedfile; format = "sharedfile"; m->mothurOut("Using " + sharedfile + " as input file for the shared parameter.\n");  }
				else { 
					listfile = current->getListFile(); 
					if (listfile != "") { inputfile = listfile; format = "list"; m->mothurOut("Using " + listfile + " as input file for the list parameter.\n");  }
					else { 
						m->mothurOut("No valid current files. You must provide a list or shared file.\n");  
						abort = true;
					}
				}
			}
			
			 
					if (outputdir == ""){    outputdir = util.hasPath(inputfile);		}

			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			label = validParameter.valid(parameters, "label");			
			if (label == "not found") { label = ""; }
			else { 
				if(label != "all") {  util.splitAtDash(label, labels);  allLines = false;  }
				else { allLines = true;  }
			}
			
			groups = validParameter.valid(parameters, "groups");			
			if (groups == "not found") { groups = ""; }
			else { 
				util.splitAtDash(groups, Groups);
                if (Groups.size() != 0) { if (Groups[0]== "all") { Groups.clear(); } }
			}
			
			calc = validParameter.valid(parameters, "calc");			
			if (calc == "not found") { 
				if(format == "list") { calc = "sobs"; }
				else { calc = "sharedsobs"; }
			}
			else { 
				 if (calc == "default")  {  
					if(format == "list") { calc = "sobs"; }
					else { calc = "sharedsobs"; }
				}
			}
			util.splitAtDash(calc, Estimators);
			if (util.inUsersGroups("citation", Estimators)) { 
				ValidCalculators validCalc; validCalc.printCitations(Estimators); 
				//remove citation from list of calcs
				for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") {  Estimators.erase(Estimators.begin()+i); break; } }
			}
			
			string temp;
			temp = validParameter.valid(parameters, "abund");		if (temp == "not found") { temp = "10"; }
			util.mothurConvert(temp, abund); 
			
			temp = validParameter.valid(parameters, "nseqs");		if (temp == "not found"){	temp = "f";				}
			nseqs = util.isTrue(temp); 

			temp = validParameter.valid(parameters, "permute");
            if (temp == "not found"){	temp = "4";				}
            else {
                if ((temp == "1") || (temp == "2") || (temp == "3") || (temp == "4")) {}
                else {
                    bool permTrue = util.isTrue(temp);
                    if (permTrue) { temp = "4"; }
                    else { }
                }
            }
			util.mothurConvert(temp, perm);
            if ((perm == 1) || (perm == 2) || (perm == 3) || (perm == 4)) { }
            else { m->mothurOut("[ERROR]: Not a valid permute value.  Valid values are 1, 2, 3, 4 and true.\n");  abort = true;  }
            
            temp = validParameter.valid(parameters, "sharedotus");		if (temp == "not found"){	temp = "t";				}
			sharedOtus = util.isTrue(temp); 
			
			temp = validParameter.valid(parameters, "fontsize");		if (temp == "not found") { temp = "24"; }
			util.mothurConvert(temp, fontsize);

		}
				
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "VennCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int VennCommand::execute(){
	try {
	
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
	
		ValidCalculators validCalculator;
					
		if (format == "list") {
			for (int i=0; i<Estimators.size(); i++) {
				if (validCalculator.isValidCalculator("vennsingle", Estimators[i]) ) { 
					if (Estimators[i] == "sobs") { 
						vennCalculators.push_back(new Sobs());
					}else if (Estimators[i] == "chao") { 
						vennCalculators.push_back(new Chao1());
					}else if (Estimators[i] == "ace") {
						if(abund < 5) { abund = 10; }
						vennCalculators.push_back(new Ace(abund));
					}
				}
			}
		}else {
			for (int i=0; i<Estimators.size(); i++) {
				if (validCalculator.isValidCalculator("vennshared", Estimators[i]) ) { 
					if (Estimators[i] == "sharedsobs") { 
						vennCalculators.push_back(new SharedSobsCS());
					}else if (Estimators[i] == "sharedchao") { 
						vennCalculators.push_back(new SharedChao1());
					}else if (Estimators[i] == "sharedace") { 
						vennCalculators.push_back(new SharedAce());
					}
				}
			}
		}
			
		//if the users entered no valid calculators don't execute command
		if (vennCalculators.size() == 0) { m->mothurOut("No valid calculators given, please correct.\n"); return 0;  }
		
		venn = new Venn(outputdir, nseqs, inputfile, fontsize, sharedOtus); 
		InputData input(inputfile, format, Groups);
		
		string lastLabel;
		
        SharedRAbundVectors* lookup = nullptr;
		if (format == "sharedfile") {
			lookup = input.getSharedRAbundVectors();
			lastLabel = lookup->getLabel();
            Groups = lookup->getNamesGroups();
			
			if ((lookup->size() > 4)) { combos = findCombinations(lookup->size()); }
		}else if (format == "list") {
			sabund = input.getSAbundVector();
			lastLabel = sabund->getLabel();
		}
		
		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;

		if (format != "list") {	
			
			//as long as you are not at the end of the file or done wih the lines you want
			while((lookup != nullptr) && ((allLines == 1) || (userLabels.size() != 0))) {
                vector<string> otuLabels = lookup->getOTUNames();
                
				if (m->getControl_pressed()) {
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
                    delete lookup;  delete venn;
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					return 0;
				}

				if(allLines == 1 || labels.count(lookup->getLabel()) == 1){
					m->mothurOut(lookup->getLabel()+"\n"); 
					processedLabels.insert(lookup->getLabel()); userLabels.erase(lookup->getLabel());
                    
                    vector<SharedRAbundVector*> data = lookup->getSharedRAbundVectors();
					if (lookup->size() > 4) {
						set< set<int> >::iterator it3;
						set<int>::iterator it2;
						for (it3 = combos.begin(); it3 != combos.end(); it3++) {  
			
							set<int> poss = *it3;
							vector<SharedRAbundVector*> subset;
							for (it2 = poss.begin(); it2 != poss.end(); it2++) {   subset.push_back(data[*it2]);   }
							
							vector<string> outfilenames = venn->getPic(subset, vennCalculators, otuLabels);
							for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]); }  }
						}		
					}else {
						vector<string> outfilenames = venn->getPic(data, vennCalculators, otuLabels);
						for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }
					}
                    for (int i = 0; i < data.size(); i++) {	delete data[i];  } data.clear();
				}
				
				if ((util.anyLabelsToProcess(lookup->getLabel(), userLabels, "") ) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = lookup->getLabel();
					
                    delete lookup;
					lookup = input.getSharedRAbundVectors(lastLabel);

					m->mothurOut(lookup->getLabel()+"\n"); 
					processedLabels.insert(lookup->getLabel()); userLabels.erase(lookup->getLabel());

                    vector<SharedRAbundVector*> data = lookup->getSharedRAbundVectors();
                    if (lookup->size() > 4) {
                        set< set<int> >::iterator it3;
                        set<int>::iterator it2;
                        for (it3 = combos.begin(); it3 != combos.end(); it3++) {
                            
                            set<int> poss = *it3;
                            vector<SharedRAbundVector*> subset;
                            for (it2 = poss.begin(); it2 != poss.end(); it2++) {   subset.push_back(data[*it2]);   }
                            
                            vector<string> outfilenames = venn->getPic(subset, vennCalculators, otuLabels);
                            for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]); }  }
                        }
                    }else {
                        vector<string> outfilenames = venn->getPic(data, vennCalculators, otuLabels);
                        for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }
                    }
                    for (int i = 0; i < data.size(); i++) {	delete data[i];  } data.clear();
					
					lookup->setLabels(saveLabel); //restore real lastlabel to save below
				}
				
				
				lastLabel = lookup->getLabel();
						
				//get next line to process
				delete lookup;
				lookup = input.getSharedRAbundVectors();
			}
			
			if (m->getControl_pressed()) {
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
					 delete venn;
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					return 0;
			}

			
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

					m->mothurOut(lookup->getLabel()+"\n"); 
					processedLabels.insert(lookup->getLabel()); userLabels.erase(lookup->getLabel());

                vector<SharedRAbundVector*> data = lookup->getSharedRAbundVectors();
                vector<string> otuLabels = lookup->getOTUNames();
                if (lookup->size() > 4) {
                    set< set<int> >::iterator it3;
                    set<int>::iterator it2;
                    for (it3 = combos.begin(); it3 != combos.end(); it3++) {
                        
                        set<int> poss = *it3;
                        vector<SharedRAbundVector*> subset;
                        for (it2 = poss.begin(); it2 != poss.end(); it2++) {   subset.push_back(data[*it2]);   }
                        
                        vector<string> outfilenames = venn->getPic(subset, vennCalculators, otuLabels);
                        for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]); }  }
                    }
                }else {
                    vector<string> outfilenames = venn->getPic(data, vennCalculators, otuLabels);
                    for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }
                }
                for (int i = 0; i < data.size(); i++) {	delete data[i];  } data.clear();
                delete lookup;
			}
			
			if (m->getControl_pressed()) {
                    delete venn;
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					return 0;
			}

			
		}else{
		
			while((sabund != nullptr) && ((allLines == 1) || (userLabels.size() != 0))) {
			
				if (m->getControl_pressed()) {
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
					delete sabund; delete venn;
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					return 0;
				}
		
				if(allLines == 1 || labels.count(sabund->getLabel()) == 1){			
	
					m->mothurOut(sabund->getLabel()); m->mothurOutEndLine();
					vector<string> outfilenames = venn->getPic(sabund, vennCalculators);
					for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }

					
					processedLabels.insert(sabund->getLabel());
					userLabels.erase(sabund->getLabel());
				}
				
				if ((util.anyLabelsToProcess(sabund->getLabel(), userLabels, "") ) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = sabund->getLabel();
				
					delete sabund;
					sabund = input.getSAbundVector(lastLabel);
					
					m->mothurOut(sabund->getLabel()); m->mothurOutEndLine();
					vector<string> outfilenames = venn->getPic(sabund, vennCalculators);
					for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }

					
					processedLabels.insert(sabund->getLabel());
					userLabels.erase(sabund->getLabel());
					
					//restore real lastlabel to save below
					sabund->setLabel(saveLabel);
				}		
				
				lastLabel = sabund->getLabel();		
				
				delete sabund;
				sabund = input.getSAbundVector();
			}
			
			if (m->getControl_pressed()) {
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					delete venn;  return 0;
			}
			
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
				if (sabund != nullptr) {	delete sabund;	}
				sabund = input.getSAbundVector(lastLabel);
					
				m->mothurOut(sabund->getLabel()); m->mothurOutEndLine();
				vector<string> outfilenames = venn->getPic(sabund, vennCalculators);
				for(int i = 0; i < outfilenames.size(); i++) { if (outfilenames[i] != "control" ) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]);  }  }

				delete sabund;
					
			}
			
			if (m->getControl_pressed()) {
					delete venn;
					for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
					for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }
					return 0;
			}
		}
		
		for (int i = 0; i < vennCalculators.size(); i++) {	delete vennCalculators[i];	}
		delete venn; 
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
//returns a vector of sets containing the group combinations
set< set<int> > VennCommand::findCombinations(int lookupSize){
	try {
		set< set<int> > combos;
		
		set<int> possibles;
		for (int i = 0; i < lookupSize; i++) {  possibles.insert(i);  }
		
		getCombos(possibles, combos);
        
		return combos;
		
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "findCombinations");
		exit(1);
	}
}
//**********************************************************************************************************************
//recusively finds combos of length perm
int VennCommand::getCombos(set<int> possibles, set< set<int> >& combos){
	try {
		
		if (possibles.size() == perm) { //done
			if (combos.count(possibles) == 0) { //no dups
				combos.insert(possibles);
			}
		}else { //we still have work to do
			set<int>::iterator it;
			set<int>::iterator it2;
			for (it = possibles.begin(); it != possibles.end(); it++) {  
				
				set<int> newPossibles;
				for (it2 = possibles.begin(); it2 != possibles.end(); it2++) {  //all possible combos of one length smaller
					if (*it != *it2) { 
						newPossibles.insert(*it2);
					}
				}
				getCombos(newPossibles, combos);
			}
		}
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "VennCommand", "getCombos");
		exit(1);
	}
}

//**********************************************************************************************************************
