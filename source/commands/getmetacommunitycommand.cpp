//
//  getmetacommunitycommand.cpp
//  Mothur
//
//  Created by SarahsWork on 4/9/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "getmetacommunitycommand.h"

#include "communitytype/communitytype.h"
#include "communitytype/kmeans.h"
#include "communitytype/qFinderDMM.h"
#include "communitytype/pam.h"
#include "calculators/canberra.h"
#include "calculators/gower.h"
#include "calculators/hamming.h"
#include "calculators/hellinger.h"
#include "calculators/manhattan.h"
#include "calculators/memchi2.h"
#include "calculators/memchord.h"
#include "calculators/memeuclidean.h"
#include "calculators/mempearson.h"
#include "calculators/odum.h"
#include "calculators/sharedace.h"
#include "calculators/sharedanderbergs.h"
#include "calculators/sharedbraycurtis.h"
#include "calculators/sharedchao1.h"
#include "calculators/sharedjabund.h"
#include "calculators/sharedjclass.h"
#include "calculators/sharedjest.h"
#include "calculators/sharedjsd.h"
#include "calculators/sharedkstest.h"
#include "calculators/sharedkulczynski.h"
#include "calculators/sharedkulczynskicody.h"
#include "calculators/sharedlennon.h"
#include "calculators/sharedmorisitahorn.h"
#include "calculators/sharednseqs.h"
#include "calculators/sharedochiai.h"
#include "calculators/sharedrjsd.h"
#include "calculators/sharedsobs.h"
#include "calculators/sharedsobscollectsummary.h"
#include "calculators/sharedsorabund.h"
#include "calculators/sharedsorclass.h"
#include "calculators/sharedsorest.h"
#include "calculators/sharedthetan.h"
#include "calculators/sharedthetayc.h"
#include "calculators/soergel.h"
#include "calculators/spearman.h"
#include "calculators/speciesprofile.h"
#include "calculators/structchi2.h"
#include "calculators/structchord.h"
#include "calculators/structeuclidean.h"
#include "calculators/structkulczynski.h"
#include "calculators/structpearson.h"
#include "calculators/whittaker.h"
#include "inputdata.h"
#include "subsample.h"
#include "validcalculator.h"

//**********************************************************************************************************************
vector<string> GetMetaCommunityCommand::setParameters(){
	try {
        CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none","outputType",false,true); parameters.push_back(pshared);
        CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
		CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
        CommandParameter pcalc("calc", "Multiple", "sharedsobs-sharedchao-sharedace-jabund-sorabund-jclass-sorclass-jest-sorest-thetayc-thetan-kstest-sharednseqs-ochiai-anderberg-kulczynski-kulczynskicody-lennon-morisitahorn-braycurtis-whittaker-odum-canberra-structeuclidean-structchord-hellinger-manhattan-structpearson-soergel-spearman-structkulczynski-speciesprofile-hamming-structchi2-gower-memchi2-memchord-memeuclidean-mempearson-jsd-rjsd", "rjsd", "", "", "","",false,false,true); parameters.push_back(pcalc);
        CommandParameter psubsample("subsample", "String", "", "", "", "", "","",false,false); parameters.push_back(psubsample);
        CommandParameter piters("iters", "Number", "", "1000", "", "", "","",false,false); parameters.push_back(piters);
        CommandParameter pminpartitions("minpartitions", "Number", "", "5", "", "", "","",false,false,true); parameters.push_back(pminpartitions);
        CommandParameter pmaxpartitions("maxpartitions", "Number", "", "100", "", "", "","",false,false,true); parameters.push_back(pmaxpartitions);
        CommandParameter poptimizegap("optimizegap", "Number", "", "3", "", "", "","",false,false,true); parameters.push_back(poptimizegap);
        CommandParameter pwithreplacement("withreplacement", "Boolean", "", "F", "", "", "","",false,false,true); parameters.push_back(pwithreplacement);
   		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
		CommandParameter pmethod("method", "Multiple", "dmm-kmeans-pam", "dmm", "", "", "","",false,false,true); parameters.push_back(pmethod);
        
        abort = false; calledHelp = false; allLines=true;
        
        vector<string> tempOutNames;
        outputTypes["fit"] = tempOutNames;
        outputTypes["relabund"] = tempOutNames;
        outputTypes["matrix"] = tempOutNames;
        outputTypes["design"] = tempOutNames;
        outputTypes["parameters"] = tempOutNames;
        outputTypes["summary"] = tempOutNames;
        
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NewCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetMetaCommunityCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The get.communitytype command parameters are shared, method, label, groups, minpartitions, maxpartitions, optimizegap, subsample, withreplacement. The shared file is required. \n";
        helpString += "The label parameter is used to analyze specific labels in your input. labels are separated by dashes.\n";
		helpString += "The groups parameter allows you to specify which of the groups in your shared file you would like analyzed.  Group names are separated by dashes.\n";
        helpString += "The method parameter allows you to select the method you would like to use.  Options are dmm, kmeans and pam. Default=dmm.\n";
        helpString += "The calc parameter allows you to select the calculator you would like to use to calculate the distance matrix used by the pam and kmeans method. By default the rjsd calculator is used.\n";
        helpString += "The iters parameter allows you to choose the number of times you would like to run the subsample while calculating the distance matrix for the pam and kmeans method.\n";
        helpString += "The subsample parameter allows you to enter the size pergroup of the sample or you can set subsample=T and mothur will use the size of your smallest group while calculating the distance matrix for the pam and kmeans methods.\n";
        helpString += "The withreplacement parameter allows you to indicate you want to subsample your data allowing for the same read to be included multiple times. Default=f. \n";
		helpString += "The minpartitions parameter is used to .... Default=5.\n";
        helpString += "The maxpartitions parameter is used to .... Default=10.\n";
        helpString += "The optimizegap parameter is used to .... Default=3.\n";
		helpString += "The get.communitytype command should be in the following format: get.communitytype(shared=yourSharedFile).\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetMetaCommunityCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fit")              {  pattern = "[filename],[distance],[method],mix.fit"; }
        else if (type == "relabund")    {  pattern = "[filename],[distance],[method],[tag],mix.relabund"; }
        else if (type == "design")      {  pattern = "[filename],[distance],[method],mix.design"; }
        else if (type == "matrix")      {  pattern = "[filename],[distance],[method],[tag],mix.posterior"; }
        else if (type == "parameters")  {  pattern = "[filename],[distance],[method],mix.parameters"; }
        else if (type == "summary")  {  pattern = "[filename],[distance],[method],mix.summary"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "GetMetaCommunityCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
GetMetaCommunityCommand::GetMetaCommunityCommand(string option) : Command()  {
	try {
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }
			else if (sharedfile == "not found") {
				//if there is a current shared file, use it
				sharedfile = current->getSharedFile();
				if (sharedfile != "") { m->mothurOut("Using " + sharedfile + " as input file for the shared parameter.\n");  }
				else { 	m->mothurOut("You have no current sharedfile and the shared parameter is required.\n");  abort = true; }
			}else { current->setSharedFile(sharedfile); }
            
            if (outputdir == ""){ outputdir = util.hasPath(sharedfile);  }
            
            string temp = validParameter.valid(parameters, "minpartitions");	if (temp == "not found"){	temp = "5";      }
			util.mothurConvert(temp, minpartitions);
            
            temp = validParameter.valid(parameters, "maxpartitions");        if (temp == "not found"){	temp = "10";	 }
			util.mothurConvert(temp, maxpartitions);
            
            temp = validParameter.valid(parameters, "optimizegap");          if (temp == "not found"){	temp = "3";	 }
			util.mothurConvert(temp, optimizegap);
            
            string groups = validParameter.valid(parameters, "groups");
			if (groups == "not found") { groups = ""; }
			else { util.splitAtDash(groups, Groups); if (Groups.size() != 0) { if (Groups[0]== "all") { Groups.clear(); } } }
            
            string label = validParameter.valid(parameters, "label");
			if (label == "not found") { label = ""; }
			else {
				if(label != "all") {  util.splitAtDash(label, labels);  allLines = false;  }
				else { allLines = true;  }
			}
            
            method = validParameter.valid(parameters, "method");
			if (method == "not found") { method = "dmm"; }
			
			if ((method == "dmm") || (method == "kmeans") || (method == "pam")) { }
			else { m->mothurOut("[ERROR]: " + method + " is not a valid method.  Valid algorithms are dmm, kmeans and pam.\n");  abort = true; }
            
            calc = validParameter.valid(parameters, "calc");
			if (calc == "not found") { calc = "rjsd";  }
			else {
                if (calc == "default")  {  calc = "rjsd";  }
			}
			util.splitAtDash(calc, Estimators);
			if (util.inUsersGroups("citation", Estimators)) {
				ValidCalculators validCalc; validCalc.printCitations(Estimators);
				//remove citation from list of calcs
				for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") {  Estimators.erase(Estimators.begin()+i); break; } }
			}
            if (Estimators.size() != 1) { abort = true; m->mothurOut("[ERROR]: only one calculator is allowed.\n"); }
            
            temp = validParameter.valid(parameters, "iters");			if (temp == "not found") { temp = "1000"; }
			util.mothurConvert(temp, iters);
            
            temp = validParameter.valid(parameters, "subsample");		if (temp == "not found") { temp = "F"; }
			if (util.isNumeric1(temp)) { util.mothurConvert(temp, subsampleSize); subsample = true; }
            else {
                if (util.isTrue(temp)) { subsample = true; subsampleSize = -1; }  //we will set it to smallest group later
                else { subsample = false; }
            }
            
            if (subsample == false) { iters = 0; }
            
            temp = validParameter.valid(parameters, "withreplacement");		if (temp == "not found"){	temp = "f";		}
            withReplacement = util.isTrue(temp);
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "GetMetaCommunityCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int GetMetaCommunityCommand::execute(){
	try {
		
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        InputData input(sharedfile, "sharedfile", Groups);
        set<string> processedLabels;
        set<string> userLabels = labels;
        string lastLabel = "";
        
        SharedRAbundVectors* lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        Groups = lookup->getNamesGroups();
        
        if (subsample) {
            if (subsampleSize == -1) { //user has not set size, set size = smallest samples size
                subsampleSize = lookup->getNumSeqsSmallestGroup();
            }else {
                lookup->removeGroups(subsampleSize);
                Groups = lookup->getNamesGroups();
            }
            
            if (lookup->size() < 2) { m->mothurOut("[ERROR]: You have not provided enough valid groups.  I cannot run the command.\n");  m->setControl_pressed(true);  return 0; }
        }
       
        //check minpartitions and maxpartitions to ensure
        if (lookup->size() < maxpartitions) {
            m->mothurOut("\n\n[NOTE]: This command is designed to be run with datasets containing > 50 samples.\n\n");
            m->mothurOut("[WARNING]: You have not provided enough valid groups, for maxpartitions=" + toString(maxpartitions) + ". Reducing maxpartitions to " + toString(lookup->size()) + ".\n");
            maxpartitions = lookup->size();
            
            if (minpartitions > lookup->size()) {
                minpartitions = lookup->size();
                m->mothurOut("[WARNING]: You have not provided enough valid groups, for minpartitions=" + toString(minpartitions) + ". Reducing minpartitions to " + toString(lookup->size()) + ".\n");
            }
            m->mothurOut("\n\n");
        }else if (lookup->size() < minpartitions) {
            m->mothurOut("[NOTE]: This command is designed to be run with datasets containing > 50 samples.\n");
            minpartitions = lookup->size();
            m->mothurOut("[WARNING]: You have not provided enough valid groups, for minpartitions=" + toString(minpartitions) + ". Reducing minpartitions to " + toString(lookup->size()) + ".\n");
            
            if (maxpartitions > lookup->size()) {
                maxpartitions = lookup->size();
                m->mothurOut("[WARNING]: You have not provided enough valid groups, for maxpartitions=" + toString(maxpartitions) + ". Reducing maxpartitions to " + toString(lookup->size()) + ".\n");
            }
            m->mothurOut("\n\n");
        }
        
        while (lookup != nullptr) {
            
            if (m->getControl_pressed()) { delete lookup; return 0; }
            
            createProcesses(lookup); delete lookup;
            
            lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        }
		
        //output files created by command
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
        return 0;
		
    }
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
int GetMetaCommunityCommand::createProcesses(SharedRAbundVectors*& thislookup){
	try {
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(sharedfile));
        variables["[distance]"] = thislookup->getLabel();
        variables["[method]"] = method;
		string outputFileName = getOutputFileName("fit", variables);
        outputNames.push_back(outputFileName); outputTypes["fit"].push_back(outputFileName);
                
		//force 1 processor
		vector< vector<int> > dividedPartitions;
        vector< vector<string> > rels, matrix;
        vector<string> doneFlags;
        dividedPartitions.resize(1);
        rels.resize(1);
        matrix.resize(1);
        int minPartition = 0;

        for (int i=1; i<=maxpartitions; i++) {
          int processToAssign = 1;
          dividedPartitions[(processToAssign-1)].push_back(i);
          variables["[tag]"] = toString(i);
          string relName = getOutputFileName("relabund", variables);
          string mName = getOutputFileName("matrix", variables);
          rels[(processToAssign-1)].push_back(relName);
          matrix[(processToAssign-1)].push_back(mName);
	}
        
        m->mothurOut("K\tNLE\t\tlogDet\tBIC\t\tAIC\t\tLaplace\n");
		minPartition = processDriver(thislookup, dividedPartitions[0], outputFileName, rels[0], matrix[0], doneFlags, 0);
        
        if (m->getControl_pressed()) { return 0; }
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: minPartition = " + toString(minPartition) + "\n"); }
        
        //run generate Summary function for smallest minPartition
        variables["[tag]"] = toString(minPartition);
        vector<double> piValues = generateDesignFile(minPartition, variables);
        if (method == "dmm") {  generateSummaryFile(minPartition, variables, piValues); } //pam doesn't make a relabund file
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "createProcesses");
		exit(1);
	}
}
//**********************************************************************************************************************
int GetMetaCommunityCommand::processDriver(SharedRAbundVectors*& thislookup, vector<int>& parts, string outputFileName, vector<string> relabunds, vector<string> matrix, vector<string> doneFlags, int processID){
	try {
        
        double minLaplace = 1e10;
        int minPartition = 1;
        vector<double> minSilhouettes; minSilhouettes.resize(thislookup->size(), 0);
        vector<string> namesOfGroups = thislookup->getNamesGroups();
        
		ofstream fitData, silData;
        if (method == "dmm") {
            util.openOutputFile(outputFileName, fitData);
            fitData.setf(ios::fixed, ios::floatfield);
            fitData.setf(ios::showpoint);
            fitData << "K\tNLE\tlogDet\tBIC\tAIC\tLaplace" << endl;
        }else if((method == "pam") || (method == "kmeans")) { //because ch is looking of maximal value
            minLaplace = 0;
            util.openOutputFile(outputFileName, silData);
            silData.setf(ios::fixed, ios::floatfield);
            silData.setf(ios::showpoint);
            silData << "K\tCH";
            
            for (int i = 0; i < namesOfGroups.size(); i++) { silData  << '\t' << namesOfGroups[i];  }
            silData << endl;
        } 
        
        cout.setf(ios::fixed, ios::floatfield);
        cout.setf(ios::showpoint);

        vector< vector<int> > sharedMatrix;
        vector<string> thisGroups = namesOfGroups;
        for (int i = 0; i < thisGroups.size(); i++) {
            RAbundVector rav = thislookup->getRAbundVector(thisGroups[i]);
            sharedMatrix.push_back(rav.get());
        }
        
        vector< vector<double> > dists; //do we want to output this matrix??
        if ((method == "pam") || (method == "kmeans")) {  dists = generateDistanceMatrix(thislookup);  }
        
        if (m->getDebug()) {
            m->mothurOut("[DEBUG]: dists = \n");
            for (int i = 0; i < dists.size(); i++) {
                if (m->getControl_pressed()) { break; }
                m->mothurOut("[DEBUG]: i = " + toString(i) + '\t');
                for (int j = 0; j < i; j++) { m->mothurOut(toString(dists[i][j]) +"\t"); }
                m->mothurOut("\n");
            }
        }
        
        for(int i=0;i<parts.size();i++){
            
            int numPartitions = parts[i];
            
            if (m->getDebug()) { m->mothurOut("[DEBUG]: running partition " + toString(numPartitions) + "\n"); }
            
            if (m->getControl_pressed()) { break; }
            
            //check to see if anyone else is done
            for (int j = 0; j < doneFlags.size(); j++) {
                if (!util.isBlank(doneFlags[j])) { //another process has finished
                    //are they done at a lower partition?
                    ifstream in; util.openInputFile(doneFlags[j], in);
                    int tempNum;
                    in >> tempNum; in.close();
                    if (tempNum < numPartitions) { break; } //quit, because someone else has finished
                }
            }
            
            CommunityTypeFinder* finder = nullptr;
            if (method == "dmm")            {   finder = new qFinderDMM(sharedMatrix, numPartitions);   }
            else if (method == "kmeans")    {   finder = new KMeans(sharedMatrix, numPartitions);       }
            else if (method == "pam")       {   finder = new Pam(sharedMatrix, dists, numPartitions);                 }
            else {
                if (i == 0) {  m->mothurOut(method + " is not a valid method option. I will run the command using dmm.\n"); }
                finder = new qFinderDMM(sharedMatrix, numPartitions);
            }
            
            string relabund = relabunds[i];
            string matrixName = matrix[i];
            outputNames.push_back(matrixName); outputTypes["matrix"].push_back(matrixName);
            
            finder->printZMatrix(matrixName, thisGroups);
            
            double chi; vector<double> silhouettes;
            if (method == "dmm") {
                double laplace = finder->getLaplace();
                if(laplace < minLaplace){
                    minPartition = numPartitions;
                    minLaplace = laplace;
                }
            }else {
                chi = finder->calcCHIndex(dists);
                silhouettes = finder->calcSilhouettes(dists);
                if (chi > minLaplace) { //save partition with maximum ch index score
                    minPartition = numPartitions;
                    minLaplace = chi;
                    minSilhouettes = silhouettes;
                }
            }
            
            if (method == "dmm") {
                finder->printFitData(cout, minLaplace);
                finder->printFitData(fitData);
                vector<string> currentLabels = thislookup->getOTUNames();
                finder->printRelAbund(relabund, currentLabels);
                outputNames.push_back(relabund); outputTypes["relabund"].push_back(relabund);
            }else if ((method == "pam") || (method == "kmeans")) { //print silouettes and ch values
                finder->printSilData(cout, chi, silhouettes);
                finder->printSilData(silData, chi, silhouettes);
                if (method == "kmeans") {
                    vector<string> currentLabels = thislookup->getOTUNames();
                    finder->printRelAbund(relabund, currentLabels);
                    outputNames.push_back(relabund); outputTypes["relabund"].push_back(relabund);
                }
            }
            delete finder;
            
            if(optimizegap != -1 && (numPartitions - minPartition) >= optimizegap && numPartitions >= minpartitions){
                string tempDoneFile = util.getRootName(util.getSimpleName(sharedfile)) + toString(processID) + ".done.temp";
                ofstream outDone;
                util.openOutputFile(tempDoneFile, outDone);
                outDone << minPartition << endl;
                outDone.close();
                break;
            }
        }
        if (method == "dmm") { fitData.close(); }
        
        if (m->getControl_pressed()) { return 0; }

        return minPartition;
    }
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "processDriver");
		exit(1);
	}
}
/**************************************************************************************************/

vector<double> GetMetaCommunityCommand::generateDesignFile(int numPartitions, map<string,string> variables){
    try {
        vector<double> piValues(numPartitions, 0);
        
        ifstream postFile;
        variables["[tag]"] = toString(numPartitions);
        string input = getOutputFileName("matrix", variables);
        util.openInputFile(input, postFile);//((fileRoot + toString(numPartitions) + "mix.posterior").c_str()); //matrix file
        variables.erase("[tag]");
		string outputFileName = getOutputFileName("design", variables);
        ofstream designFile;
        util.openOutputFile(outputFileName, designFile);
        outputNames.push_back(outputFileName); outputTypes["design"].push_back(outputFileName);
        
        
        vector<string> titles(numPartitions);
        
        for(int i=0;i<numPartitions;i++){   postFile >> titles[i];  }
        
        double posterior;
        string sampleName;
        int numSamples = 0;
        
        while(postFile){
            
            if (m->getControl_pressed()) { break; }
            
            double maxPosterior = 0.0000;
            int maxPartition = -1;
            
            postFile >> sampleName;
            
            for(int i=0;i<numPartitions;i++){
                
                postFile >> posterior;
                if(posterior > maxPosterior){
                    maxPosterior = posterior;
                    maxPartition = i;
                }
                piValues[i] += posterior;
                
            }
            
            designFile << sampleName << '\t' << titles[maxPartition] << endl;
            
            numSamples++;
            gobble(postFile);
        }
        for(int i=0;i<numPartitions;i++){
            piValues[i] /= (double)numSamples;
        }
        
        
        postFile.close();
        designFile.close();
        
        return piValues;
    }
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "generateDesignFile");
		exit(1);
	}
}

/**************************************************************************************************/

inline bool summaryFunction(summaryData i, summaryData j){ return i.difference > j.difference;   }

/**************************************************************************************************/
int GetMetaCommunityCommand::generateSummaryFile(int numPartitions, map<string,string> v, vector<double> piValues){
    try {
        vector<summaryData> summary;
        
        vector<double> pMean(numPartitions, 0);
        vector<double> pLCI(numPartitions, 0);
        vector<double> pUCI(numPartitions, 0);
        
        string name, header;
        double mean, lci, uci;
        
        ifstream referenceFile;
        map<string, string> variables;
        variables["[filename]"] = v["[filename]"];
        variables["[distance]"] = v["[distance]"];
        variables["[method]"] = method;
        variables["[tag]"] = "1";
        string reference = getOutputFileName("relabund", variables);
        util.openInputFile(reference, referenceFile); //((fileRoot + label + ".1mix.relabund").c_str());
        variables["[tag]"] = toString(numPartitions);
        string partFile = getOutputFileName("relabund", variables);
        ifstream partitionFile;
        util.openInputFile(partFile, partitionFile); //((fileRoot + toString(numPartitions) + "mix.relabund").c_str());
        
        header = util.getline(referenceFile);
        header = util.getline(partitionFile);
        stringstream head(header);
        string dummy, label;
        head >> dummy;
        vector<string> thetaValues(numPartitions, "");
        for(int i=0;i<numPartitions;i++){
            head >> label >> dummy >> dummy;
            thetaValues[i] = label.substr(label.find_last_of('_')+1);
        }
        
        
        vector<double> partitionDiff(numPartitions, 0.0000);
        
        while(referenceFile){
            
            if (m->getControl_pressed()) { break; }
            
            referenceFile >> name >> mean >> lci >> uci;
            
            summaryData tempData;
            tempData.name = name;
            tempData.refMean = mean;
            
            double difference = 0.0000;
            
            partitionFile >> name;
            for(int j=0;j<numPartitions;j++){
                partitionFile >> pMean[j] >> pLCI[j] >> pUCI[j];
                difference += abs(mean - pMean[j]);
                partitionDiff[j] += abs(mean - pMean[j]);;
            }
            
            tempData.partMean = pMean;
            tempData.partLCI = pLCI;
            tempData.partUCI = pUCI;
            tempData.difference = difference;
            summary.push_back(tempData);
            
            gobble(referenceFile);
            gobble(partitionFile);
        }
        referenceFile.close();
        partitionFile.close();
        
        if (m->getControl_pressed()) { return 0; }
        
        int numOTUs = (int)summary.size();
        
        sort(summary.begin(), summary.end(), summaryFunction);
        
        variables.erase("[tag]");
		string outputFileName = getOutputFileName("parameters", variables);
        outputNames.push_back(outputFileName); outputTypes["parameters"].push_back(outputFileName);
        
        ofstream parameterFile;
        util.openOutputFile(outputFileName, parameterFile); //((fileRoot + "mix.parameters").c_str());
        parameterFile.setf(ios::fixed, ios::floatfield);
        parameterFile.setf(ios::showpoint);
        
        double totalDifference =  0.0000;
        parameterFile << "Part\tDif2Ref_i\ttheta_i\tpi_i\n";
        for(int i=0;i<numPartitions;i++){
            if (m->getControl_pressed()) { break; }
            parameterFile << i+1 << '\t' << setprecision(2) << partitionDiff[i] << '\t' << thetaValues[i] << '\t' << piValues[i] << endl;
            totalDifference += partitionDiff[i];
        }
        parameterFile.close();
        
        if (m->getControl_pressed()) { return 0; }
        
        string summaryFileName = getOutputFileName("summary", variables);
        outputNames.push_back(summaryFileName); outputTypes["summary"].push_back(summaryFileName);
        
        ofstream summaryFile;
        util.openOutputFile(summaryFileName, summaryFile); //((fileRoot + "mix.summary").c_str());
        summaryFile.setf(ios::fixed, ios::floatfield);
        summaryFile.setf(ios::showpoint);
        
        
        summaryFile << "OTU\tP0.mean";
        for(int i=0;i<numPartitions;i++){
            summaryFile << "\tP" << i+1 << ".mean\tP" << i+1 << ".lci\tP" << i+1 << ".uci";
        }
        summaryFile << "\tDifference\tCumFraction" << endl;
        
        double cumDiff = 0.0000;
        
        for(int i=0;i<numOTUs;i++){
            if (m->getControl_pressed()) { break; }
            summaryFile << summary[i].name << setprecision(2) << '\t' << summary[i].refMean;
            for(int j=0;j<numPartitions;j++){
                summaryFile  << '\t' << summary[i].partMean[j] << '\t' << summary[i].partLCI[j] << '\t' << summary[i].partUCI[j];
            }
            
            cumDiff += summary[i].difference/totalDifference;
            summaryFile << '\t' << summary[i].difference << '\t' << cumDiff << endl;
        }
        summaryFile.close();
        
        return 0;
        
    }
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "generateSummaryFile");
		exit(1);
	}
    
}
//**********************************************************************************************************************
vector<vector<double> > GetMetaCommunityCommand::generateDistanceMatrix(SharedRAbundVectors*& thisLookup){
    try {
        vector<vector<double> > results;
        
        Calculator* matrixCalculator;
        ValidCalculators validCalculator;
        int i = 0;
        
        if (validCalculator.isValidCalculator("matrix", Estimators[i]) ) {
            if (Estimators[i] == "sharedsobs") {
                matrixCalculator = new SharedSobsCS();
            }else if (Estimators[i] == "sharedchao") {
                matrixCalculator = new SharedChao1();
            }else if (Estimators[i] == "sharedace") {
                matrixCalculator = new SharedAce();
            }else if (Estimators[i] == "jabund") {
                matrixCalculator = new JAbund();
            }else if (Estimators[i] == "sorabund") {
                matrixCalculator = new SorAbund();
            }else if (Estimators[i] == "jclass") {
                matrixCalculator = new Jclass();
            }else if (Estimators[i] == "sorclass") {
                matrixCalculator = new SorClass();
            }else if (Estimators[i] == "jest") {
                matrixCalculator = new Jest();
            }else if (Estimators[i] == "sorest") {
                matrixCalculator = new SorEst();
            }else if (Estimators[i] == "thetayc") {
                matrixCalculator = new ThetaYC();
            }else if (Estimators[i] == "thetan") {
                matrixCalculator = new ThetaN();
            }else if (Estimators[i] == "kstest") {
                matrixCalculator = new KSTest();
            }else if (Estimators[i] == "sharednseqs") {
                matrixCalculator = new SharedNSeqs();
            }else if (Estimators[i] == "ochiai") {
                matrixCalculator = new Ochiai();
            }else if (Estimators[i] == "anderberg") {
                matrixCalculator = new Anderberg();
            }else if (Estimators[i] == "kulczynski") {
                matrixCalculator = new Kulczynski();
            }else if (Estimators[i] == "kulczynskicody") {
                matrixCalculator = new KulczynskiCody();
            }else if (Estimators[i] == "lennon") {
                matrixCalculator = new Lennon();
            }else if (Estimators[i] == "morisitahorn") {
                matrixCalculator = new MorHorn();
            }else if (Estimators[i] == "braycurtis") {
                matrixCalculator = new BrayCurtis();
            }else if (Estimators[i] == "whittaker") {
                matrixCalculator = new Whittaker();
            }else if (Estimators[i] == "odum") {
                matrixCalculator = new Odum();
            }else if (Estimators[i] == "canberra") {
                matrixCalculator = new Canberra();
            }else if (Estimators[i] == "structeuclidean") {
                matrixCalculator = new StructEuclidean();
            }else if (Estimators[i] == "structchord") {
                matrixCalculator = new StructChord();
            }else if (Estimators[i] == "hellinger") {
                matrixCalculator = new Hellinger();
            }else if (Estimators[i] == "manhattan") {
                matrixCalculator = new Manhattan();
            }else if (Estimators[i] == "structpearson") {
                matrixCalculator = new StructPearson();
            }else if (Estimators[i] == "soergel") {
                matrixCalculator = new Soergel();
            }else if (Estimators[i] == "spearman") {
                matrixCalculator = new Spearman();
            }else if (Estimators[i] == "structkulczynski") {
                matrixCalculator = new StructKulczynski();
            }else if (Estimators[i] == "speciesprofile") {
                matrixCalculator = new SpeciesProfile();
            }else if (Estimators[i] == "hamming") {
                matrixCalculator = new Hamming();
            }else if (Estimators[i] == "structchi2") {
                matrixCalculator = new StructChi2();
            }else if (Estimators[i] == "gower") {
                matrixCalculator = new Gower();
            }else if (Estimators[i] == "memchi2") {
                matrixCalculator = new MemChi2();
            }else if (Estimators[i] == "memchord") {
                matrixCalculator = new MemChord();
            }else if (Estimators[i] == "memeuclidean") {
                matrixCalculator = new MemEuclidean();
            }else if (Estimators[i] == "mempearson") {
                matrixCalculator = new MemPearson();
            }else if (Estimators[i] == "jsd") {
                matrixCalculator = new JSD();
            }else if (Estimators[i] == "rjsd") {
                matrixCalculator = new RJSD();
            }else {
                m->mothurOut("[ERROR]: " + Estimators[i] + " is not a valid calculator, please correct.\n"); m->setControl_pressed(true); return results;
            }
        }
        
        //calc distances
        vector< vector< vector<seqDist> > > calcDistsTotals;  //each iter, then each groupCombos dists. this will be used to make .dist files
        vector< vector<seqDist> > calcDists; calcDists.resize(1);
        
        SubSample sample;
        for (int thisIter = 0; thisIter < iters+1; thisIter++) {
 
            SharedRAbundVectors* thisItersLookup = new SharedRAbundVectors(*thisLookup);
            vector<string> namesOfGroups = thisItersLookup->getNamesGroups();
            
            if (subsample && (thisIter != 0)) {
                if (withReplacement)    { sample.getSampleWithReplacement(thisItersLookup, subsampleSize);  }
                else                    { sample.getSample(thisItersLookup, subsampleSize);                 }
            }
            
            driver(thisItersLookup, calcDists, matrixCalculator);
            
            if (subsample && (thisIter != 0)) {
                if((thisIter) % 100 == 0){	m->mothurOutJustToScreen(toString(thisIter)+"\n"); 		}
                calcDistsTotals.push_back(calcDists);
                for (int i = 0; i < calcDists.size(); i++) {
                    for (int j = 0; j < calcDists[i].size(); j++) {
                        if (m->getDebug()) {  m->mothurOut("[DEBUG]: Results: iter = " + toString(thisIter) + ", " + namesOfGroups[calcDists[i][j].seq1] + " - " + namesOfGroups[calcDists[i][j].seq2] + " distance = " + toString(calcDists[i][j].dist) + ".\n");  }
                    }
                }
            }else { //print results for whole dataset
                for (int i = 0; i < calcDists.size(); i++) {
                    if (m->getControl_pressed()) { break; }
                    
                    //initialize matrix
                    results.resize(thisLookup->size());
                    for (int k = 0; k < thisLookup->size(); k++) {  results[k].resize(thisLookup->size(), 0.0); }
                    
                    for (int j = 0; j < calcDists[i].size(); j++) {
                        int row = calcDists[i][j].seq1;
                        int column = calcDists[i][j].seq2;
                        double dist = calcDists[i][j].dist;
                        
                        results[row][column] = dist;
                        results[column][row] = dist;
                    }
                }
            }
            for (int i = 0; i < calcDists.size(); i++) {  calcDists[i].clear(); }
            delete thisItersLookup;
		}
		
        if (iters != 0) {
            //we need to find the average distance and standard deviation for each groups distance
            vector< vector<seqDist>  > calcAverages = util.getAverages(calcDistsTotals, "average");
            
            //print results
            for (int i = 0; i < calcDists.size(); i++) {
                results.resize(thisLookup->size());
                for (int k = 0; k < thisLookup->size(); k++) {  results[k].resize(thisLookup->size(), 0.0); }
                
                for (int j = 0; j < calcAverages[i].size(); j++) {
                    int row = calcAverages[i][j].seq1;
                    int column = calcAverages[i][j].seq2;
                    float dist = calcAverages[i][j].dist;
                    
                    results[row][column] = dist;
                    results[column][row] = dist;
                }
            }
        }

        delete matrixCalculator;
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "GetMetaCommunityCommand", "generateDistanceMatrix");
        exit(1);
    }
}
/**************************************************************************************************/
int GetMetaCommunityCommand::driver(SharedRAbundVectors*& thisLookup, vector< vector<seqDist> >& calcDists, Calculator* matrixCalculator) {
	try {
        vector<SharedRAbundVector*> data = thisLookup->getSharedRAbundVectors();
		vector<SharedRAbundVector*> subset;
        
		for (int k = 0; k < data.size(); k++) { // pass cdd each set of groups to compare
			
			for (int l = 0; l < k; l++) {
				
				if (k != l) { //we dont need to similiarity of a groups to itself
					subset.clear(); //clear out old pair of sharedrabunds
					//add new pair of sharedrabunds
					subset.push_back(data[k]); subset.push_back(data[l]);
					
					
                    
                    //if this calc needs all groups to calculate the pair load all groups
                    if (matrixCalculator->getNeedsAll()) {
                        //load subset with rest of lookup for those calcs that need everyone to calc for a pair
                        for (int w = 0; w < data.size(); w++) {
                            if ((w != k) && (w != l)) { subset.push_back(data[w]); }
                        }
                    }
                    
                    vector<double> tempdata = matrixCalculator->getValues(subset); //saves the calculator outputs
                    
                    if (m->getControl_pressed()) { for (int i = 0; i < data.size(); i++) { delete data[i]; } data.clear(); return 1; }
                    
                    seqDist temp(l, k, tempdata[0]);
                    calcDists[0].push_back(temp);
                }
				
			}
		}
        
        for (int i = 0; i < data.size(); i++) { delete data[i]; } data.clear();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "GetMetaCommunityCommand", "driver");
		exit(1);
	}
}
//**********************************************************************************************************************

