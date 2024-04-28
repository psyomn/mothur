//
//  sparcccommand.cpp
//  Mothur
//
//  Created by SarahsWork on 5/10/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "sparcccommand.h"


//**********************************************************************************************************************
vector<string> SparccCommand::setParameters(){
	try {
        CommandParameter pshared("shared", "InputTypes", "", "", "none", "none", "none","outputType",false,true); parameters.push_back(pshared);
        CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
		CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
		CommandParameter psamplings("samplings", "Number", "", "20", "", "", "","",false,false,false); parameters.push_back(psamplings);
        CommandParameter piterations("iterations", "Number", "", "10", "", "", "","",false,false,false); parameters.push_back(piterations);
        CommandParameter ppermutations("permutations", "Number", "", "1000", "", "", "","",false,false,false); parameters.push_back(ppermutations);
        CommandParameter pmethod("method", "Multiple", "relabund-dirichlet", "dirichlet", "", "", "","",false,false); parameters.push_back(pmethod);
        CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
        //every command must have inputdir and outputdir.  This allows mothur users to redirect input and output files.
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false; allLines = true;
        
        vector<string> tempOutNames;
        outputTypes["corr"] = tempOutNames; //filetypes should be things like: shared, fasta, accnos...
        outputTypes["pvalue"] = tempOutNames;
        outputTypes["sparccrelabund"] = tempOutNames;


		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string SparccCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The sparcc command allows you to ....\n";
		helpString += "The sparcc command parameters are: shared, groups, label, samplings, iterations, permutations, processors and method.\n";
		helpString += "The samplings parameter is used to .... Default=20.\n";
        helpString += "The iterations parameter is used to ....Default=10.\n";
        helpString += "The permutations parameter is used to ....Default=1000.\n";
        helpString += "The method parameter is used to ....Options are relabund and dirichlet. Default=dirichlet.\n";
        helpString += "The default value for groups is all the groups in your sharedfile.\n";
		helpString += "The label parameter is used to analyze specific labels in your shared file.\n";
		helpString += "The sparcc command should be in the following format: sparcc(shared=yourSharedFile)\n";
		helpString += "sparcc(shared=final.an.shared)\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string SparccCommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "corr") {  pattern = "[filename],[distance],sparcc_correlation"; }
        else if (type == "pvalue") {  pattern = "[filename],[distance],sparcc_pvalue"; }
        else if (type == "sparccrelabund") {  pattern = "[filename],[distance],sparcc_relabund"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "SparccCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
SparccCommand::SparccCommand(string option) : Command()  {
	try {
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

			normalizeMethod = validParameter.valid(parameters, "method");
			if (normalizeMethod == "not found") { normalizeMethod = "dirichlet"; }
			if ((normalizeMethod == "dirichlet") || (normalizeMethod == "relabund")) { }
			else { m->mothurOut(normalizeMethod + " is not a valid method.  Valid methods are dirichlet and relabund.\n");  abort = true; }


            string temp = validParameter.valid(parameters, "samplings");	if (temp == "not found"){	temp = "20";	}
			util.mothurConvert(temp, numSamplings);

            if(normalizeMethod == "relabund"){ numSamplings = 1; }

            temp = validParameter.valid(parameters, "iterations");	if (temp == "not found"){	temp = "10";	}
			util.mothurConvert(temp, maxIterations);

            temp = validParameter.valid(parameters, "permutations");	if (temp == "not found"){	temp = "1000";	}
			util.mothurConvert(temp, numPermutations);

            temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
			processors = current->setProcessors(temp);

            string groups = validParameter.valid(parameters, "groups");
			if (groups == "not found") { groups = ""; }
			else { util.splitAtDash(groups, Groups); if (Groups.size() != 0) { if (Groups[0]== "all") { Groups.clear(); } } }


            string label = validParameter.valid(parameters, "label");
			if (label == "not found") { label = ""; }
			else {
				if(label != "all") {  util.splitAtDash(label, labels);  allLines = false;  }
				else { allLines = true;  }
			}
		}

	}
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "SparccCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
int SparccCommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

        long start = time(nullptr);

        InputData input(sharedfile, "sharedfile", Groups);
        set<string> processedLabels;
        set<string> userLabels = labels;
        string lastLabel = "";
        
        SharedRAbundVectors* lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        Groups = lookup->getNamesGroups();
        
        while (lookup != nullptr) {
            
            if (m->getControl_pressed()) { delete lookup; break; }
            
            process(lookup); delete lookup;
            
            lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        }

        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }

        m->mothurOut("It took " + toString(time(nullptr) - start) + " seconds to process.\n\n");

        //output files created by command
		m->mothurOut("\nOutput File Names: \n");
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
        return 0;

    }
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
int SparccCommand::process(SharedRAbundVectors*& shared){
	try {
        cout.setf(ios::fixed, ios::floatfield);
        cout.setf(ios::showpoint);

        vector<vector<float> > sharedVector;
        vector<string> otuNames = shared->getOTUNames();
        vector<SharedRAbundVector*> data = shared->getSharedRAbundVectors();

        //fill sharedVector to pass to CalcSparcc
        for (int i = 0; i < data.size(); i++) {
            vector<int> abunds = data[i]->get();
            vector<float> temp;
            for (int j = 0; j < abunds.size(); j++) { temp.push_back((float) abunds[j]); }
            sharedVector.push_back(temp);
        }
        int numOTUs = (int)sharedVector[0].size();
        int numGroups = data.size();

        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(sharedfile));
        variables["[distance]"] = shared->getLabel();


        string relAbundFileName = getOutputFileName("sparccrelabund", variables);
        ofstream relAbundFile;
        util.openOutputFile(relAbundFileName, relAbundFile);
        outputNames.push_back(relAbundFileName);  outputTypes["sparccrelabund"].push_back(relAbundFileName);

        relAbundFile << "OTU\taveRelAbund\n";
        for(int i=0;i<numOTUs;i++){
            if (m->getControl_pressed()) { for (int i = 0; i < data.size(); i++) { delete data[i]; } data.clear(); relAbundFile.close(); return 0; }

            double relAbund = 0.0000;
            for(int j=0;j<numGroups;j++){
                relAbund += sharedVector[j][i]/(double)data[j]->getNumSeqs();
            }
            relAbundFile << otuNames[i] <<'\t' << relAbund / (double) numGroups << endl;
        }
        relAbundFile.close();

        CalcSparcc originalData(sharedVector, maxIterations, numSamplings, normalizeMethod);
        vector<vector<float> > origCorrMatrix = originalData.getRho();

        string correlationFileName = getOutputFileName("corr", variables);
        ofstream correlationFile;
        util.openOutputFile(correlationFileName, correlationFile);
        outputNames.push_back(correlationFileName);  outputTypes["corr"].push_back(correlationFileName);
        correlationFile.setf(ios::fixed, ios::floatfield);
        correlationFile.setf(ios::showpoint);

				correlationFile << "OTU_id";
        for(int i=0;i<numOTUs;i++){ correlationFile << '\t' << otuNames[i];    }   correlationFile << endl;
        for(int i=0;i<numOTUs;i++){
            correlationFile << otuNames[i];
            for(int j=0;j<numOTUs;j++){
                correlationFile << '\t' << origCorrMatrix[i][j];
            }
            correlationFile << endl;
        }


        if(numPermutations != 0){
            vector<vector<float> > pValues = createProcesses(sharedVector, origCorrMatrix);

            if (m->getControl_pressed()) { for (int i = 0; i < data.size(); i++) { delete data[i]; } data.clear(); return 0; }

            string pValueFileName = getOutputFileName("pvalue", variables);
            ofstream pValueFile;
            util.openOutputFile(pValueFileName, pValueFile);
            outputNames.push_back(pValueFileName);  outputTypes["pvalue"].push_back(pValueFileName);
            pValueFile.setf(ios::fixed, ios::floatfield);
            pValueFile.setf(ios::showpoint);

            for(int i=0;i<numOTUs;i++){ pValueFile << '\t' << otuNames[i];    }   pValueFile << endl;
            for(int i=0;i<numOTUs;i++){
                pValueFile << otuNames[i];
                for(int j=0;j<numOTUs;j++){
                    pValueFile << '\t' << pValues[i][j];
                }
                pValueFile << endl;
            }
        }

        for (int i = 0; i < data.size(); i++) { delete data[i]; } data.clear();
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "process");
		exit(1);
	}
}
/**************************************************************************************************/
struct sparccData {
   	MothurOut* m;
    vector< vector<float> > sharedVector;
    vector< vector<float> > origCorrMatrix;
    vector<vector<float> > pValues;
    int numSamplings, maxIterations, numPermutations, numOTUs;
    string normalizeMethod;
    Utils util;

    sparccData(){}
    sparccData(vector< vector<float> > cs, vector< vector<float> > co, int ns, int mi, int np, string nm) {
        m = MothurOut::getInstance();
        sharedVector = cs;
        origCorrMatrix = co;
        numSamplings = ns;
        maxIterations = mi;
        numPermutations = np;
        normalizeMethod = nm;
        numOTUs = sharedVector[0].size();
        pValues.resize(numOTUs);
        for(int i=0;i<numOTUs;i++){ pValues[i].assign(numOTUs, 0);  }
    }
};
/**************************************************************************************************/
vector<vector<float> > shuffleSharedVector(vector<vector<float> >& sharedVector, MothurOut* m, Utils& util){
    try {
        int numGroups = (int)sharedVector.size();
        int numOTUs = (int)sharedVector[0].size();

        vector<vector<float> > shuffledVector = sharedVector;
        for(int i=0;i<numGroups;i++){
            for(int j=0;j<numOTUs;j++){
                shuffledVector[i][j] = sharedVector[util.getRandomIndex(numGroups-1)][j];
            }
        }

        return shuffledVector;
    }
    catch(exception& e) {
        m->errorOut(e, "SparccCommand", "shuffleSharedVector");
        exit(1);
    }
}
//**********************************************************************************************************************
void driverSparcc(sparccData* params){
    try {
        vector<vector<float> > sharedShuffled = params->sharedVector;

        for(int i=0;i<params->numPermutations;i++){
            if (params->m->getControl_pressed()) { break; }
            sharedShuffled = shuffleSharedVector(params->sharedVector, params->m, params->util);
            CalcSparcc permutedData(sharedShuffled, params->maxIterations, params->numSamplings, params->normalizeMethod);
            vector<vector<float> > permuteCorrMatrix = permutedData.getRho();

            for(int j=0;j<params->numOTUs;j++){
                for(int k=0;k<j;k++){
                     if (params->m->getControl_pressed()) { break; }
                    double randValue = permuteCorrMatrix[j][k];
                    double observedValue = params->origCorrMatrix[j][k];
                    if(observedValue >= 0 &&  randValue > observedValue)   { params->pValues[j][k]++; }//this method seems to deflate the
                    else if(observedValue < 0 && randValue < observedValue){ params->pValues[j][k]++; }//pvalues of small rho values
                }
            }

            float done = ceil(params->numPermutations * 0.05);
            if((i+1) % (int)(done) == 0){ params->m->mothurOutJustToScreen(toString(i+1)+"\n");   }
        }
    }
    catch(exception& e) {
        params->m->errorOut(e, "SparccCommand", "driverSparcc");
        exit(1);
    }
}
//**********************************************************************************************************************
vector<vector<float> > SparccCommand::createProcesses(vector<vector<float> >& sharedVector, vector<vector<float> >& origCorrMatrix){
	try {
        //divide work by number of permutations
        vector<int> lines;
        if (processors > numPermutations) { processors = numPermutations; }

        //figure out how many sequences you have to process
        int numItersPerProcessor = numPermutations / processors;
        for (int i = 0; i < processors; i++) {
            if(i == (processors - 1)){	numItersPerProcessor = numPermutations - i * numItersPerProcessor; 	}
            lines.push_back(numItersPerProcessor);
        }

        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<sparccData*> data;

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {

            sparccData* dataBundle = new sparccData(sharedVector, origCorrMatrix, numSamplings, maxIterations, lines[i+1], normalizeMethod);
            data.push_back(dataBundle);

            workerThreads.push_back(new std::thread(driverSparcc, dataBundle));
        }

        int numOTUs = sharedVector[0].size();
        sparccData* dataBundle = new sparccData(sharedVector, origCorrMatrix, numSamplings, maxIterations, lines[0], normalizeMethod);
        driverSparcc(dataBundle);
        vector<vector<float> > pValues = dataBundle->pValues;

        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();

            vector<vector<float> > thisProcessorsPValues = data[i]->pValues;

            for (int k = 0; k < numOTUs; k++) {
                for (int j = 0; j < k; j++) {
                    pValues[k][j] += thisProcessorsPValues[k][j];
                }
            }

            delete data[i];
            delete workerThreads[i];
        }
        delete dataBundle;


        for(int i=0;i<numOTUs;i++){
            pValues[i][i] = 1;
            for(int j=0;j<i;j++){
                pValues[i][j] /= (double)numPermutations;
                pValues[j][i] = pValues[i][j];
            }
        }

        return pValues;
    }
	catch(exception& e) {
		m->errorOut(e, "SparccCommand", "createProcesses");
		exit(1);
	}
}
//**********************************************************************************************************************
