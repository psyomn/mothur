/*
 *  homovacommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/8/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "homovacommand.h"
#include "datastructures/groupmap.h"
#include "datastructures/designmap.h"
#include "read/readphylipvector.h"


//**********************************************************************************************************************
vector<string> HomovaCommand::setParameters(){
	try {
		CommandParameter pdesign("design", "InputTypes", "", "", "none", "none", "none","homova",false,true,true); parameters.push_back(pdesign);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none","homova",false,true,true); parameters.push_back(pphylip);
        CommandParameter psets("sets", "String", "", "", "", "", "","",false,false); parameters.push_back(psets);
		CommandParameter piters("iters", "Number", "", "1000", "", "", "","",false,false); parameters.push_back(piters);
		CommandParameter palpha("alpha", "Number", "", "0.05", "", "", "","",false,false); parameters.push_back(palpha);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

        abort = false; calledHelp = false;

        vector<string> tempOutNames;
        outputTypes["homova"] = tempOutNames;

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string HomovaCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "Referenced: Stewart CN, Excoffier L (1996). Assessing population genetic structure and variability with RAPD data: Application to Vaccinium macrocarpon (American Cranberry). J Evol Biol 9: 153-71.\n";
		helpString += "The homova command outputs a .homova file. \n";
		helpString += "The homova command parameters are phylip, iters, sets and alpha.  The phylip and design parameters are required, unless valid current files exist.\n";
		helpString += "The design parameter allows you to assign your samples to groups when you are running homova. It is required. \n";
		helpString += "The design file looks like the group file.  It is a 2 column tab delimited file, where the first column is the sample name and the second column is the group the sample belongs to.\n";
        helpString += "The sets parameter allows you to specify which of the sets in your designfile you would like to analyze. The set names are separated by dashes. THe default is all sets in the designfile.\n";
		helpString += "The iters parameter allows you to set number of randomization for the P value.  The default is 1000. \n";
		helpString += "The homova command should be in the following format: homova(phylip=file.dist, design=file.design).\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string HomovaCommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "homova") {  pattern = "[filename],homova"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "HomovaCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************

HomovaCommand::HomovaCommand(string option) : Command() {
	try {
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }

		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();

			ValidParameters validParameter;


			phylipFileName = validParameter.validFile(parameters, "phylip");
			if (phylipFileName == "not open") { phylipFileName = ""; abort = true; }
			else if (phylipFileName == "not found") {
				//if there is a current phylip file, use it
				phylipFileName = current->getPhylipFile();
				if (phylipFileName != "") { m->mothurOut("Using " + phylipFileName + " as input file for the phylip parameter.\n");  }
				else { 	m->mothurOut("You have no current phylip file and the phylip parameter is required.\n");  abort = true; }

			}else { current->setPhylipFile(phylipFileName); }

			//check for required parameters
			designFileName = validParameter.validFile(parameters, "design");
			if (designFileName == "not open") { abort = true; }
			else if (designFileName == "not found") {
				//if there is a current design file, use it
				designFileName = current->getDesignFile();
				if (designFileName != "") { m->mothurOut("Using " + designFileName + " as input file for the design parameter.\n");  }
				else { 	m->mothurOut("You have no current design file and the design parameter is required.\n");  abort = true; }
			}else { current->setDesignFile(designFileName); }

			string temp = validParameter.valid(parameters, "iters");
			if (temp == "not found") { temp = "1000"; }
			util.mothurConvert(temp, iters);

			temp = validParameter.valid(parameters, "alpha");
			if (temp == "not found") { temp = "0.05"; }
			util.mothurConvert(temp, experimentwiseAlpha);

            string sets = validParameter.valid(parameters, "sets");
			if (sets == "not found") { sets = ""; }
			else {
				util.splitAtDash(sets, Sets);
			}
		}
		if (outputdir == "") { outputdir = util.hasPath(phylipFileName); }
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "HomovaCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int HomovaCommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

		//read design file
        DesignMap* designMap = new DesignMap(designFileName); if (m->getControl_pressed()) { delete designMap; return 0; }

		//read in distance matrix and square it
		ReadPhylipVector readMatrix(phylipFileName);
		vector<string> sampleNames = readMatrix.read(distanceMatrix);

        if (Sets.size() != 0) { //user selected sets, so we want to remove the samples not in those sets
            vector<string> dGroups = designMap->getCategory();

            for(int i=0;i<distanceMatrix.size();i++){

                if (m->getControl_pressed()) { delete designMap; return 0; }

                string group = designMap->get(sampleNames[i]);

                if (group == "not found") {
                    m->mothurOut("[ERROR]: " + sampleNames[i] + " is not in your design file, please correct.\n");  m->setControl_pressed(true);
                }else if (!util.inUsersGroups(group, Sets)){  //not in set we want remove it
                    //remove from all other rows
                    for(int j=0;j<distanceMatrix.size();j++){
                        distanceMatrix[j].erase(distanceMatrix[j].begin()+i);
                    }
                    distanceMatrix.erase(distanceMatrix.begin()+i);
                    sampleNames.erase(sampleNames.begin()+i);
                    i--;
                }
            }
        }

		for(int i=0;i<distanceMatrix.size();i++){
			for(int j=0;j<i;j++){ distanceMatrix[i][j] *= distanceMatrix[i][j];	 }
		}

		//link designMap to rows/columns in distance matrix
		map<string, vector<int> > origGroupSampleMap;
		for(int i=0;i<sampleNames.size();i++){
			string group = designMap->get(sampleNames[i]);

			if (group == "not found") {
				m->mothurOut("[ERROR]: " + sampleNames[i] + " is not in your design file, please correct.\n");  m->setControl_pressed(true);
			}else { origGroupSampleMap[group].push_back(i); }
		}
		int numGroups = origGroupSampleMap.size();

		if (m->getControl_pressed()) { delete designMap; return 0; }

		//create a new filename
		ofstream HOMOVAFile;
        map<string, string> variables;
		variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(phylipFileName));
		string HOMOVAFileName = getOutputFileName("homova", variables);
		util.openOutputFile(HOMOVAFileName, HOMOVAFile);
		outputNames.push_back(HOMOVAFileName); outputTypes["homova"].push_back(HOMOVAFileName);

		HOMOVAFile << "HOMOVA\tBValue\tP-value\tSSwithin/(Ni-1)_values" << endl;
		m->mothurOut("HOMOVA\tBValue\tP-value\tSSwithin/(Ni-1)_values\n");

		double fullHOMOVAPValue = runHOMOVA(HOMOVAFile, origGroupSampleMap, experimentwiseAlpha);

		if(fullHOMOVAPValue <= experimentwiseAlpha && numGroups > 2){

			int numCombos = numGroups * (numGroups-1) / 2;
			double pairwiseAlpha = experimentwiseAlpha / (double) numCombos;

			map<string, vector<int> >::iterator itA;
			map<string, vector<int> >::iterator itB;

			for(itA=origGroupSampleMap.begin();itA!=origGroupSampleMap.end();itA++){
				itB = itA;itB++;
				for(;itB!=origGroupSampleMap.end();itB++){
					map<string, vector<int> > pairwiseGroupSampleMap;
					pairwiseGroupSampleMap[itA->first] = itA->second;
					pairwiseGroupSampleMap[itB->first] = itB->second;

					runHOMOVA(HOMOVAFile, pairwiseGroupSampleMap, pairwiseAlpha);
				}
			}
			HOMOVAFile << endl;

			m->mothurOut("\nExperiment-wise error rate: " + toString(experimentwiseAlpha) + '\n');
			m->mothurOut("Pair-wise error rate (Bonferroni): " + toString(pairwiseAlpha) + '\n');
		}
		else{ m->mothurOut("Experiment-wise error rate: " + toString(experimentwiseAlpha) + '\n'); }

		m->mothurOut("If you have borderline P-values, you should try increasing the number of iterations\n");

		delete designMap;

		m->mothurOut("\nOutput File Names: \n");
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

double HomovaCommand::runHOMOVA(ofstream& HOMOVAFile, map<string, vector<int> > groupSampleMap, double alpha){
	try {
		map<string, vector<int> >::iterator it;
		int numGroups = groupSampleMap.size();

		vector<double> ssWithinOrigVector;
		double bValueOrig = calcBValue(groupSampleMap, ssWithinOrigVector);

		double counter = 0;
		for(int i=0;i<iters;i++){
			vector<double> ssWithinRandVector;
			map<string, vector<int> > randomizedGroup = getRandomizedGroups(groupSampleMap);
			double bValueRand = calcBValue(randomizedGroup, ssWithinRandVector);
			if(bValueRand >= bValueOrig){	counter++;	}
		}

		double pValue = (double) counter / (double) iters;
		string pString = "";
		if(pValue < 1/(double)iters){	pString = '<' + toString(1/(double)iters);	}
		else						{	pString = toString(pValue);					}


		//print homova table
		it = groupSampleMap.begin();
		HOMOVAFile << it->first;
		m->mothurOut(it->first);
		it++;
		for(;it!=groupSampleMap.end();it++){
			HOMOVAFile << '-' << it->first;
			m->mothurOut('-' + it->first);
		}

		HOMOVAFile << '\t' << bValueOrig << '\t' << pString;
		m->mothurOut('\t' + toString(bValueOrig) + '\t' + pString);

		if(pValue < alpha){
			HOMOVAFile << "*";
			m->mothurOut("*");
		}

		for(int i=0;i<numGroups;i++){
			HOMOVAFile << '\t' << ssWithinOrigVector[i];
			m->mothurOut('\t' + toString(ssWithinOrigVector[i]));
		}
		HOMOVAFile << endl;
		m->mothurOutEndLine();

		return pValue;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "runHOMOVA");
		exit(1);
	}
}

//**********************************************************************************************************************

double HomovaCommand::calcSigleSSWithin(vector<int> sampleIndices) {
	try {
		double ssWithin = 0.0;
		int numSamplesInGroup = sampleIndices.size();

		for(int i=0;i<numSamplesInGroup;i++){
			int row = sampleIndices[i];

			for(int j=0;j<numSamplesInGroup;j++){
				int col = sampleIndices[j];

				if(col < row){ ssWithin += distanceMatrix[row][col]; }
			}
		}

		ssWithin /= (double) numSamplesInGroup;
		return ssWithin;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "calcSigleSSWithin");
		exit(1);
	}
}

//**********************************************************************************************************************

double HomovaCommand::calcBValue(map<string, vector<int> > groupSampleMap, vector<double>& ssWithinVector) {
	try {
		double numGroups = (double)groupSampleMap.size();
		ssWithinVector.resize(numGroups, 0);

		double totalNumSamples = 0; double ssWithinFull = 0; double secondTermSum = 0; double inverseOneMinusSum = 0; int index = 0;

		for(map<string, vector<int> >::iterator it = groupSampleMap.begin();it!=groupSampleMap.end();it++){
			int numSamplesInGroup = it->second.size();
			totalNumSamples += numSamplesInGroup;

			ssWithinVector[index] = calcSigleSSWithin(it->second);
			ssWithinFull += ssWithinVector[index];

			secondTermSum += (numSamplesInGroup - 1) * log(ssWithinVector[index] / (double)(numSamplesInGroup - 1));
			inverseOneMinusSum += 1.0 / (double)(numSamplesInGroup - 1);

			ssWithinVector[index] /= (double)(numSamplesInGroup - 1); //this line is only for output purposes to scale SSw by the number of samples in the group
			index++;
		}

		double B = (totalNumSamples - numGroups) * log(ssWithinFull/(totalNumSamples-numGroups)) - secondTermSum;
		double denomintor = 1 + 1.0/(3.0 * (numGroups - 1.0)) * (inverseOneMinusSum - 1.0 / (double) (totalNumSamples - numGroups));
		B /= denomintor;

		return B;
	}
	catch(exception& e) {
		m->errorOut(e, "HomovaCommand", "calcBValue");
		exit(1);
	}
}
//**********************************************************************************************************************

map<string, vector<int> > HomovaCommand::getRandomizedGroups(map<string, vector<int> > origMapping){
	try{
		vector<int> sampleIndices;
		vector<int> samplesPerGroup;

		for(map<string, vector<int> >::iterator it=origMapping.begin();it!=origMapping.end();it++){
			vector<int> indices = it->second;
			samplesPerGroup.push_back(indices.size());
			sampleIndices.insert(sampleIndices.end(), indices.begin(), indices.end());
		}

		util.mothurRandomShuffle(sampleIndices);

		int index = 0;
		map<string, vector<int> > randomizedGroups = origMapping;
		for(map<string, vector<int> >::iterator it=randomizedGroups.begin();it!=randomizedGroups.end();it++){
			for(int i=0;i<it->second.size();i++){
				it->second[i] = sampleIndices[index++];
			}
		}

		return randomizedGroups;
	}
	catch (exception& e) {
		m->errorOut(e, "AmovaCommand", "randomizeGroups");
		exit(1);
	}
}

//**********************************************************************************************************************


