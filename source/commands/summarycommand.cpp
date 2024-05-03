/*
 *  summarycommand.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "summarycommand.h"

#include "calculators/ace.h"
#include "calculators/bergerparker.h"
#include "calculators/boneh.h"
#include "calculators/bootstrap.h"
#include "calculators/bstick.h"
#include "calculators/chao1.h"
#include "calculators/coverage.h"
#include "calculators/efron.h"
#include "calculators/geom.h"
#include "calculators/goodscoverage.h"
#include "calculators/heip.h"
#include "calculators/invsimpson.h"
#include "calculators/jackknife.h"
#include "calculators/logsd.h"
#include "calculators/npshannon.h"
#include "calculators/nseqs.h"
#include "calculators/qstat.h"
#include "calculators/shannon.h"
#include "calculators/shannoneven.h"
#include "calculators/shannonrange.h"
#include "calculators/shen.h"
#include "calculators/simpson.h"
#include "calculators/simpsoneven.h"
#include "calculators/smithwilson.h"
#include "calculators/sobs.h"
#include "calculators/solow.h"
#include "inputdata.h"
#include "subsample.h"
#include "validcalculator.h"

//**********************************************************************************************************************
vector<string> SummaryCommand::setParameters(){	
	try {
		CommandParameter plist("list", "InputTypes", "", "", "LRSS", "LRSS", "none","summary",false,false,true); parameters.push_back(plist);
		CommandParameter prabund("rabund", "InputTypes", "", "", "LRSS", "LRSS", "none","summary",false,false); parameters.push_back(prabund);
		CommandParameter psabund("sabund", "InputTypes", "", "", "LRSS", "LRSS", "none","summary",false,false); parameters.push_back(psabund);
		CommandParameter pshared("shared", "InputTypes", "", "", "LRSS", "LRSS", "none","summary",false,false,true); parameters.push_back(pshared);
        CommandParameter psubsample("subsample", "String", "", "", "", "", "","",false,false); parameters.push_back(psubsample);
        CommandParameter pwithreplacement("withreplacement", "Boolean", "", "F", "", "", "","",false,false,true); parameters.push_back(pwithreplacement);
        CommandParameter piters("iters", "Number", "", "1000", "", "", "","",false,false); parameters.push_back(piters);
		CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
		CommandParameter pcalc("calc", "Multiple", "sobs-chao-nseqs-coverage-ace-jack-shannon-shannoneven-npshannon-heip-smithwilson-simpson-simpsoneven-invsimpson-bootstrap-geometric-qstat-logseries-bergerparker-bstick-goodscoverage-efron-boneh-solow-shen", "sobs-chao-ace-jack-shannon-npshannon-simpson-shannonrange", "", "", "","",true,false,true); parameters.push_back(pcalc);
		CommandParameter palpha("alpha", "Multiple", "0-1-2", "1", "", "", "","",false,false,true); parameters.push_back(palpha);
        CommandParameter pabund("abund", "Number", "", "10", "", "", "","",false,false); parameters.push_back(pabund);
		CommandParameter psize("size", "Number", "", "0", "", "", "","",false,false); parameters.push_back(psize);
		CommandParameter pgroupmode("groupmode", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pgroupmode);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
		
        vector<string> tempOutNames;
        outputTypes["summary"] = tempOutNames;
        
        abort = false; calledHelp = false;  allLines = true;
        
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string SummaryCommand::getHelpString(){	
	try {
		string helpString = "";
		ValidCalculators validCalculator;
		helpString += "The summary.single command parameters are list, sabund, rabund, shared, subsample, iters, label, calc, abund and groupmode.  list, sabund, rabund or shared is required unless you have a valid current file.\n";
		helpString += "The summary.single command should be in the following format: \n";
		helpString += "summary.single(label=yourLabel, calc=yourEstimators).\n";
		helpString += "Example summary.single(label=unique-.01-.03, calc=sobs-chao-ace-jack-bootstrap-shannon-npshannon-simpson).\n";
		helpString += validCalculator.printCalc("summary");
        helpString += "The subsample parameter allows you to enter the size of the sample or you can set subsample=T and mothur will use the size of your smallest group in the case of a shared file. With a list, sabund or rabund file you must provide a subsample size.\n";
        helpString += "The withreplacement parameter allows you to indicate you want to subsample your data allowing for the same read to be included multiple times. Default=f. \n";
        helpString += "The iters parameter allows you to choose the number of times you would like to run the subsample. Default=1000.\n";
		helpString += "The default value calc is sobs-chao-ace-jack-shannon-npshannon-simpson\n";
		helpString += "If you are running summary.single with a shared file and would like your summary results collated in one file, set groupmode=t. (Default=true).\n";
        helpString += "The alpha parameter is used to set the alpha value for the shannonrange calculator.\n";
		helpString += "The label parameter is used to analyze specific labels in your input.\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string SummaryCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "summary") {  pattern = "[filename],summary-[filename],[tag],summary"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "SummaryCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
SummaryCommand::SummaryCommand(string option) : Command()  {
	try {
		if(option == "help") {  help();  abort = true; calledHelp = true; }
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
			
			sabundfile = validParameter.validFile(parameters, "sabund");
			if (sabundfile == "not open") { sabundfile = ""; abort = true; }	
			else if (sabundfile == "not found") { sabundfile = ""; }
			else {  format = "sabund"; inputfile = sabundfile; current->setSabundFile(sabundfile); }
			
			rabundfile = validParameter.validFile(parameters, "rabund");
			if (rabundfile == "not open") { rabundfile = ""; abort = true; }	
			else if (rabundfile == "not found") { rabundfile = ""; }
			else {  format = "rabund"; inputfile = rabundfile; current->setRabundFile(rabundfile); }
			
			sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }	
			else if (sharedfile == "not found") { sharedfile = ""; }
			else {  format = "sharedfile"; inputfile = sharedfile; current->setSharedFile(sharedfile); }
			
			if ((sharedfile == "") && (listfile == "") && (rabundfile == "") && (sabundfile == "")) { 
				//is there are current file available for any of these?
				//give priority to shared, then list, then rabund, then sabund
				//if there is a current shared file, use it
				sharedfile = current->getSharedFile(); 
				if (sharedfile != "") { inputfile = sharedfile; format = "sharedfile"; m->mothurOut("Using " + sharedfile + " as input file for the shared parameter.\n"); }
				else { 
					listfile = current->getListFile(); 
					if (listfile != "") { inputfile = listfile; format = "list"; m->mothurOut("Using " + listfile + " as input file for the list parameter.\n");  }
					else { 
						rabundfile = current->getRabundFile(); 
						if (rabundfile != "") { inputfile = rabundfile; format = "rabund"; m->mothurOut("Using " + rabundfile + " as input file for the rabund parameter.\n");  }
						else { 
							sabundfile = current->getSabundFile(); 
							if (sabundfile != "") { inputfile = sabundfile; format = "sabund"; m->mothurOut("Using " + sabundfile + " as input file for the sabund parameter.\n"); }
							else { 
								m->mothurOut("No valid current files. You must provide a list, sabund, rabund or shared file before you can use the collect.single command.\n");
								abort = true;
							}
						}
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
				
			calc = validParameter.valid(parameters, "calc");			
			if (calc == "not found") { calc = "sobs-chao-ace-jack-shannon-npshannon-simpson";  }
			else { 
				 if (calc == "default")  {  calc = "sobs-chao-ace-jack-shannon-npshannon-simpson";  }
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
			
			temp = validParameter.valid(parameters, "size");			if (temp == "not found") { temp = "0"; }
			util.mothurConvert(temp, size); 
			
			temp = validParameter.valid(parameters, "groupmode");		if (temp == "not found") { temp = "T"; }
			groupMode = util.isTrue(temp);
			
            temp = validParameter.valid(parameters, "iters");			if (temp == "not found") { temp = "1000"; }
			util.mothurConvert(temp, iters);
            
            temp = validParameter.valid(parameters, "subsample");		if (temp == "not found") { temp = "F"; }
			if (util.isNumeric1(temp)) { util.mothurConvert(temp, subsampleSize); subsample = true; }
            else {  
                if (util.isTrue(temp)) { subsample = true; subsampleSize = -1; }  //we will set it to smallest group later 
                else { subsample = false; subsampleSize = -1; }
            }
            
            temp = validParameter.valid(parameters, "withreplacement");		if (temp == "not found"){	temp = "f";		}
            withReplacement = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "alpha");		if (temp == "not found") { temp = "1"; }
			util.mothurConvert(temp, alpha);
            
            if ((alpha != 0) && (alpha != 1) && (alpha != 2)) { m->mothurOut("[ERROR]: Not a valid alpha value. Valid values are 0, 1 and 2.\n");  abort=true; }
            
            if (!subsample) { iters = 1; }
            else {
                //if you did not set a samplesize and are not using a sharedfile
                if ((subsampleSize == -1) && (format != "sharedfile"))  { m->mothurOut("[ERROR]: If you want to subsample with a list, rabund or sabund file, you must provide the sample size.  You can do this by setting subsample=yourSampleSize.\n");  abort=true; }
            }

		}
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "SummaryCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int SummaryCommand::execute(){
	try {
	
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
		
		if ((format != "sharedfile")) { inputFileNames.push_back(inputfile);  }
		else {  inputFileNames = parseSharedFile(sharedfile);  format = "rabund"; }
		
		if (m->getControl_pressed()) { return 0; }
		
		int numLines = 0; int numCols = 0;
		map<string, string> groupIndex;
        
		for (int p = 0; p < inputFileNames.size(); p++) {
			
			numLines = 0; numCols = 0;
			
            map<string, string> variables; 
            variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(inputFileNames[p]));
			string fileNameRoot = getOutputFileName("summary",variables);
            variables["[tag]"] = "ave-std";
            string fileNameAve = getOutputFileName("summary",variables);
            
			if (inputFileNames.size() > 1) { m->mothurOut("\nProcessing group " + groups[p] + "\n\n"); groupIndex[fileNameRoot] = groups[p]; }
			
            fillEstimators();
			
			//if the users entered no valid calculators don't execute command
			if (sumCalculators.size() == 0) {  for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } return 0; }
			
			ofstream outputFileHandle; ofstream outAve;
            if (subsample) {
                util.openOutputFile(fileNameAve, outAve);
                outputNames.push_back(fileNameAve); outputTypes["summary"].push_back(fileNameAve);
                outAve << "label\tmethod"; 
                outAve.setf(ios::fixed, ios::floatfield); outAve.setf(ios::showpoint);
                if (inputFileNames.size() > 1) {
                    groupIndex[fileNameAve] = groups[p];
                }
            }else {
                util.openOutputFile(fileNameRoot, outputFileHandle);
                outputFileHandle << "label";
                outputNames.push_back(fileNameRoot); outputTypes["summary"].push_back(fileNameRoot);
            }
		
            for(int i=0;i<sumCalculators.size();i++){
                if(sumCalculators[i]->getCols() == 1){
                    
                    if (subsample) { outAve << '\t' << sumCalculators[i]->getName();  }
                    else { outputFileHandle << '\t' << sumCalculators[i]->getName();  }
                    numCols++;
                }
                else{
                    if (subsample) { outAve << '\t' << sumCalculators[i]->getName() << "\t" << sumCalculators[i]->getName() << "_lci\t" << sumCalculators[i]->getName() << "_hci";  }
                    else { outputFileHandle << '\t' << sumCalculators[i]->getName() << "\t" << sumCalculators[i]->getName() << "_lci\t" << sumCalculators[i]->getName() << "_hci"; }
                    numCols += 3;
                }
            }
            
            if (subsample) {  outAve << endl; }
            else {  outputFileHandle << endl; }
            
			InputData input(inputFileNames[p], format, nullVector);
            set<string> processedLabels;
            set<string> userLabels = labels;
            string lastLabel = "";
            
            SAbundVector* sabund = util.getNextSAbund(input, allLines, userLabels, processedLabels, lastLabel);
            
			if (m->getControl_pressed()) {  if (!subsample) { outputFileHandle.close(); } else { outAve.close(); } for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } for(int i=0;i<sumCalculators.size();i++){  delete sumCalculators[i]; }  delete sabund;    return 0;  }
            
            while (sabund != nullptr) {
                       
                if (m->getControl_pressed()) { delete sabund; break; }
                       
                process(sabund, outputFileHandle, outAve); delete sabund;
                      
                sabund = util.getNextSAbund(input, allLines, userLabels, processedLabels, lastLabel);
                numLines++;
            }
			
            if (subsample) { outAve.close(); }
            else { outputFileHandle.close(); }
			
			if (m->getControl_pressed()) {  for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } for(int i=0;i<sumCalculators.size();i++){  delete sumCalculators[i]; }  return 0;  }
			 
			for(int i=0;i<sumCalculators.size();i++){  delete sumCalculators[i]; }
		}
		
        if ((format != "sharedfile")) {}
        else { for (int p = 0; p < inputFileNames.size(); p++) { util.mothurRemove(inputFileNames[p]); }}
        
		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }  return 0;  }
		
		//create summary file containing all the groups data for each label - this function just combines the info from the files already created.
		if ((sharedfile != "") && (groupMode)) {   vector<string> comboNames = createGroupSummaryFile(numLines, numCols, outputNames, groupIndex);  for (int i = 0; i < comboNames.size(); i++) { outputNames.push_back(comboNames[i]); } }
		
		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  }  return 0;  }
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
void SummaryCommand::fillEstimators() {
    try {
        sumCalculators.clear();
        
        ValidCalculators validCalculator;
        
        for (int i=0; i<Estimators.size(); i++) {
            if (validCalculator.isValidCalculator("summary", Estimators[i]) ) {
                if(Estimators[i] == "sobs"){
                    sumCalculators.push_back(new Sobs());
                }else if(Estimators[i] == "chao"){
                    sumCalculators.push_back(new Chao1());
                }else if(Estimators[i] == "coverage"){
                    sumCalculators.push_back(new Coverage());
                }else if(Estimators[i] == "geometric"){
                    sumCalculators.push_back(new Geom());
                }else if(Estimators[i] == "logseries"){
                    sumCalculators.push_back(new LogSD());
                }else if(Estimators[i] == "qstat"){
                    sumCalculators.push_back(new QStat());
                }else if(Estimators[i] == "bergerparker"){
                    sumCalculators.push_back(new BergerParker());
                }else if(Estimators[i] == "bstick"){
                    sumCalculators.push_back(new BStick());
                }else if(Estimators[i] == "ace"){
                    if(abund < 5)
                        abund = 10;
                    sumCalculators.push_back(new Ace(abund));
                }else if(Estimators[i] == "jack"){
                    sumCalculators.push_back(new Jackknife());
                }else if(Estimators[i] == "shannon"){
                    sumCalculators.push_back(new Shannon());
                }else if(Estimators[i] == "shannoneven"){
                    sumCalculators.push_back(new ShannonEven());
                }else if(Estimators[i] == "shannonrange"){
                    sumCalculators.push_back(new RangeShannon(alpha));
                }else if(Estimators[i] == "npshannon"){
                    sumCalculators.push_back(new NPShannon());
                }else if(Estimators[i] == "heip"){
                    sumCalculators.push_back(new Heip());
                }else if(Estimators[i] == "smithwilson"){
                    sumCalculators.push_back(new SmithWilson());
                }else if(Estimators[i] == "simpson"){
                    sumCalculators.push_back(new Simpson());
                }else if(Estimators[i] == "simpsoneven"){
                    sumCalculators.push_back(new SimpsonEven());
                }else if(Estimators[i] == "invsimpson"){
                    sumCalculators.push_back(new InvSimpson());
                }else if(Estimators[i] == "bootstrap"){
                    sumCalculators.push_back(new Bootstrap());
                }else if (Estimators[i] == "nseqs") {
                    sumCalculators.push_back(new NSeqs());
                }else if (Estimators[i] == "goodscoverage") {
                    sumCalculators.push_back(new GoodsCoverage());
                }else if (Estimators[i] == "efron") {
                    sumCalculators.push_back(new Efron(size));
                }else if (Estimators[i] == "boneh") {
                    sumCalculators.push_back(new Boneh(size));
                }else if (Estimators[i] == "solow") {
                    sumCalculators.push_back(new Solow(size));
                }else if (Estimators[i] == "shen") {
                    sumCalculators.push_back(new Shen(size, abund));
                }
            }
        }
    }
    catch(exception& e) {
        m->errorOut(e, "SummaryCommand", "fillEstimators");
        exit(1);
    }
}
//**********************************************************************************************************************
int SummaryCommand::process(SAbundVector*& sabund, ofstream& outputFileHandle, ofstream& outAve) {
    try {
        
        //calculator -> data -> values
        vector< vector< vector<double> > >  results; results.resize(sumCalculators.size());
        
        if (!subsample) {  outputFileHandle << sabund->getLabel(); }
        
        SubSample sample;
        for (int thisIter = 0; thisIter < iters; thisIter++) {
            
            SAbundVector* thisIterSabund = sabund;
            
            //we want the summary results for the whole dataset, then the subsampling
            if (subsample) { //subsample sabund and run it
                //copy sabund since getSample destroys it
                RAbundVector rabund = sabund->getRAbundVector();
                SAbundVector* newSabund = new SAbundVector();
                *newSabund = rabund.getSAbundVector();
                
                if (withReplacement)    {  sample.getSampleWithReplacement(newSabund, subsampleSize);   }
                else                    {  sample.getSample(newSabund, subsampleSize);                  }
                
                thisIterSabund = newSabund;
            }
            
            for(int i=0;i<sumCalculators.size();i++){
                vector<double> data = sumCalculators[i]->getValues(thisIterSabund);
               
                if (m->getControl_pressed()) {  return 0;  }
                
                if (!subsample) {
                    outputFileHandle << '\t';
                    sumCalculators[i]->print(outputFileHandle);
                }else {
                    //some of the calc have hci and lci need to make room for that
                    if (results[i].size() == 0) {  results[i].resize(data.size());  }
                    //save results for ave and std.
                    for (int j = 0; j < data.size(); j++) {
                        if (m->getControl_pressed()) {  return 0;  }
                        results[i][j].push_back(data[j]); 
                    }
                }
            }
            
            //cleanup memory
            if (subsample) { delete thisIterSabund; }
        }
        if (!subsample) { outputFileHandle << endl; }
     
        if (subsample) {
            outAve << sabund->getLabel() << '\t' << "ave";
            //find ave and std for this label and output
            //will need to modify the createGroupSummary to combine results and not mess with the .summary file.
            
            //calcs -> values
            vector< vector<double> >  calcAverages; calcAverages.resize(sumCalculators.size()); 
            for (int i = 0; i < calcAverages.size(); i++) {  calcAverages[i].resize(results[i].size(), 0);  }
            
            for (int thisIter = 0; thisIter < iters; thisIter++) { //sum all groups dists for each calculator
                for (int i = 0; i < calcAverages.size(); i++) {  //initialize sums to zero.
                    for (int j = 0; j < calcAverages[i].size(); j++) {
                        calcAverages[i][j] += results[i][j][thisIter];
                    }
                }
            }
            
            for (int i = 0; i < calcAverages.size(); i++) {  //finds average.
                for (int j = 0; j < calcAverages[i].size(); j++) {
                    calcAverages[i][j] /= (float) iters;
                    outAve  << '\t' << calcAverages[i][j];
                }
            }
            
            //find standard deviation
            vector< vector<double>  > stdDev; stdDev.resize(sumCalculators.size());
            for (int i = 0; i < stdDev.size(); i++) {  stdDev[i].resize(results[i].size(), 0);  }
            
            for (int thisIter = 0; thisIter < iters; thisIter++) { //compute the difference of each dist from the mean, and square the result of each
                for (int i = 0; i < stdDev.size(); i++) {  
                    for (int j = 0; j < stdDev[i].size(); j++) {
                        stdDev[i][j] += ((results[i][j][thisIter] - calcAverages[i][j]) * (results[i][j][thisIter] - calcAverages[i][j]));
                    }
                }
            }
            
            outAve << endl << sabund->getLabel() << '\t' << "std";
            for (int i = 0; i < stdDev.size(); i++) {  //finds average.
                for (int j = 0; j < stdDev[i].size(); j++) {
                    stdDev[i][j] /= (float) iters;
                    stdDev[i][j] = sqrt(stdDev[i][j]);
                    outAve  << '\t' << stdDev[i][j];
                }
            }
            outAve << endl;  
        }
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "SummaryCommand", "process");
        exit(1);
    }
}
//**********************************************************************************************************************
vector<string> SummaryCommand::parseSharedFile(string filename) {
	try {
        vector<string> filenames;
        
        map<string, string> files;
        map<string, string>::iterator it3;
        
        InputData input(filename, "sharedfile", groups);
        SharedRAbundVectors* lookup = input.getSharedRAbundVectors();

        /******************************************************/
        //user has not set size, set size = smallest samples size
        if (subsample) { 
            if (subsampleSize == -1) { subsampleSize = lookup->getNumSeqsSmallestGroup();   m->mothurOut("\nSetting subsample size to " + toString(subsampleSize) + ".\n"); }
            else { lookup->removeGroups(subsampleSize); }
            
            if (lookup->size() < 1) { m->mothurOut("You have not provided enough valid groups.  I cannot run the command.\n");  m->setControl_pressed(true);  return filenames; }
        }
		/******************************************************/
        
        groups = lookup->getNamesGroups();
        //clears file before we start to write to it below
        string sharedFileRoot = util.getRootName(filename);
        for (int i=0; i<groups.size(); i++) {
            ofstream temp;
            string group = groups[i];
            util.openOutputFile((sharedFileRoot + group + ".rabund"), temp);
            filenames.push_back((sharedFileRoot + group + ".rabund"));
            files[group] = (sharedFileRoot + group + ".rabund");
        }
        
        while(lookup != nullptr) {
            
            vector<SharedRAbundVector*> data = lookup->getSharedRAbundVectors();
            for (int i = 0; i < data.size(); i++) {
                ofstream temp;
                string group = data[i]->getGroup();
                util.openOutputFileAppend(files[group], temp);
                data[i]->getRAbundVector().print(temp);
                temp.close();
                delete data[i];
            }
            
            delete lookup;
            lookup = input.getSharedRAbundVectors();
        }
        
        return filenames;
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "parseSharedFile");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> SummaryCommand::createGroupSummaryFile(int numLines, int numCols, vector<string>& outputNames, map<string, string> groupIndex) {
	try {
				
		//open each groups summary file
        vector<string> newComboNames;
		
		map<string, map<string, vector<string> > > files;
        map<string, string> filesTypesLabels;
        map<string, int> filesTypesNumLines;
		for (int i=0; i<outputNames.size(); i++) {
			vector<string> thisFilesLines;
            
			ifstream temp;
			util.openInputFile(outputNames[i], temp);
			
			//read through first line - labels
            string labelsLine = util.getline(temp);
            vector<string> theseLabels = util.splitWhiteSpace(labelsLine);
            
            string newLabel = "";
            for (int j = 0; j < theseLabels.size(); j++) { 
                if (j == 1) {  newLabel += "group\t" + theseLabels[j];  }
                else if (j == 0) {  newLabel += theseLabels[j] + "\t";  }
                else{  newLabel += '\t' + theseLabels[j];	}
            }
			
			gobble(temp);
			
            int stop = numLines;
            if (theseLabels.size() != numCols+1) {  stop = numLines*2; }
			//for each label
			for (int k = 0; k < stop; k++) {
				
				string thisLine = "";
				string tempLabel;
					
				for (int j = 0; j < theseLabels.size(); j++) {  
					temp >> tempLabel; 
						
					//save for later
					if (j == 1) { thisLine += groupIndex[outputNames[i]] + "\t" + tempLabel;	}
                    else if (j == 0) {  thisLine += tempLabel + "\t";  }
					else{  thisLine += "\t" + tempLabel;	}
				}
					
				thisLine += "\n";
				
				thisFilesLines.push_back(thisLine);
					
				gobble(temp);
			}
            
            string extension = util.getExtension(outputNames[i]);
            if (theseLabels.size() != numCols+1) { extension = ".ave-std" + extension;  }
            string combineFileName = outputdir + util.getRootName(util.getSimpleName(sharedfile)) + "groups" + extension;
			util.mothurRemove(combineFileName); //remove old file
            filesTypesLabels[extension] = newLabel;
            filesTypesNumLines[extension] = stop;
            
            map<string, map<string, vector<string> > >::iterator itFiles = files.find(extension);
            if (itFiles != files.end()) { //add new files info to existing type
                files[extension][outputNames[i]] = thisFilesLines;
            }else {
                map<string, vector<string> > thisFile;
                thisFile[outputNames[i]] = thisFilesLines;
                files[extension] = thisFile;
            }
			
			temp.close();
			util.mothurRemove(outputNames[i]);
		}
		
        
        for (map<string, map<string, vector<string> > >::iterator itFiles = files.begin(); itFiles != files.end(); itFiles++) {
            
            if (m->getControl_pressed()) { break; }
            
            string extension = itFiles->first;
            map<string, vector<string> > thisType = itFiles->second;
            string combineFileName = outputdir + util.getRootName(util.getSimpleName(sharedfile)) + "groups" + extension;
            newComboNames.push_back(combineFileName);
            //open combined file
            ofstream out;
            util.openOutputFile(combineFileName, out);
            
            //output label line to new file
            out <<  filesTypesLabels[extension] << endl;
		
            //for each label
            for (int k = 0; k < filesTypesNumLines[extension]; k++) {
		
                //grab summary data for each group
                for (map<string, vector<string> >::iterator itType = thisType.begin(); itType != thisType.end(); itType++) {
                    out << (itType->second)[k];
                }
            }	
		
            outputNames.clear();
		
            out.close();
        }
		
		//return combine file name
		return newComboNames;
		
	}
	catch(exception& e) {
		m->errorOut(e, "SummaryCommand", "createGroupSummaryFile");
		exit(1);
	}
}
//**********************************************************************************************************************
