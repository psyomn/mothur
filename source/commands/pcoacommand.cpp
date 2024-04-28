
/*
 *  pcacommand.cpp
 *  Mothur
 *
 *  Created by westcott on 1/4/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "pcoacommand.h"
#include "read/readphylipvector.h"

//**********************************************************************************************************************
vector<string> PCOACommand::setParameters(){
	try {
		CommandParameter pphylip("phylip", "InputTypes", "", "", "none", "none", "none","pcoa-loadings",false,true,true); parameters.push_back(pphylip);
		CommandParameter pmetric("metric", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pmetric);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

        abort = false; calledHelp = false;

        vector<string> tempOutNames;
        outputTypes["pcoa"] = tempOutNames;
        outputTypes["loadings"] = tempOutNames;

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string PCOACommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The pcoa command parameters are phylip and metric";
		helpString += "The phylip parameter allows you to enter your distance file.";
		helpString += "The metric parameter allows indicate you if would like the pearson correlation coefficient calculated. Default=True";
		helpString += "Example pcoa(phylip=yourDistanceFile).\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string PCOACommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "pcoa") {  pattern = "[filename],pcoa.axes"; }
        else if (type == "loadings") {  pattern = "[filename],pcoa.loadings"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "PCOACommand", "getOutputPattern");
        exit(1);
    }
}

//**********************************************************************************************************************
PCOACommand::PCOACommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }

		else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser. getParameters();

			ValidParameters validParameter;
			phylipfile = validParameter.validFile(parameters, "phylip");
			if (phylipfile == "not open") { abort = true; }
			else if (phylipfile == "not found") {
				//if there is a current phylip file, use it
				phylipfile = current->getPhylipFile();
				if (phylipfile != "") { m->mothurOut("Using " + phylipfile + " as input file for the phylip parameter.\n");  }
				else { 	m->mothurOut("You have no current phylip file and the phylip parameter is required.\n");  abort = true; }
			}else { current->setPhylipFile(phylipfile); }

			filename = phylipfile;

			if (outputdir == ""){	 outputdir += util.hasPath(phylipfile);  }

			string temp = validParameter.valid(parameters, "metric");	if (temp == "not found"){	temp = "T";				}
			metric = util.isTrue(temp);
		}

	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "PCOACommand");
		exit(1);
	}
}
//**********************************************************************************************************************
int PCOACommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

		cout.setf(ios::fixed, ios::floatfield);
		cout.setf(ios::showpoint);
		cerr.setf(ios::fixed, ios::floatfield);
		cerr.setf(ios::showpoint);

		vector<string> names;
		vector<vector<double> > D;

		fbase = outputdir + util.getRootName(util.getSimpleName(filename));

		ReadPhylipVector readFile(filename);
		names = readFile.read(D);

		if (m->getControl_pressed()) { return 0; }

		double offset = 0.0000;
		vector<double> d;
		vector<double> e;
		vector<vector<double> > G = D;
		//vector<vector<double> > copy_G;

		m->mothurOut("\nProcessing...\n");

		for(int count=0;count<2;count++){
			linearCalc.recenter(offset, D, G);		if (m->getControl_pressed()) { return 0; }
			linearCalc.tred2(G, d, e);				if (m->getControl_pressed()) { return 0; }
			linearCalc.qtli(d, e, G);				if (m->getControl_pressed()) { return 0; }
			offset = d[d.size()-1];
			if(offset > 0.0) break;
		}

		if (m->getControl_pressed()) { return 0; }

		output(fbase, names, G, d);

		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } return 0; }

		if (metric) {

			for (int i = 1; i < 4; i++) {

				vector< vector<double> > EuclidDists = linearCalc.calculateEuclidianDistance(G, i); //G is the pcoa file

				if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } return 0; }

				double corr = linearCalc.calcPearson(EuclidDists, D); //G is the pcoa file, D is the users distance matrix

				m->mothurOut("Rsq " + toString(i) + " axis: " + toString(corr * corr)); m->mothurOutEndLine();

				if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } return 0; }
			}
		}

		m->mothurOut("\nOutput File Names: \n");
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "execute");
		exit(1);
	}
}
/*********************************************************************************************************************************/

void PCOACommand::get_comment(istream& f, char begin, char end){
	try {
		char d=f.get();
		while(d != end){	d = f.get();	}
		d = f.peek();
	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "get_comment");
		exit(1);
	}
}
/*********************************************************************************************************************************/

void PCOACommand::output(string fnameRoot, vector<string> name_list, vector<vector<double> >& G, vector<double> d) {
	try {
		int rank = name_list.size();
		double dsum = 0.0000;
		for(int i=0;i<rank;i++){
			dsum += d[i];
			for(int j=0;j<rank;j++){
				if(d[j] >= 0)	{	G[i][j] *= pow(d[j],0.5);	}
				else			{	G[i][j] = 0.00000;			}
			}
		}

		ofstream pcaData;
        map<string, string> variables;
        variables["[filename]"] = fnameRoot;
        string pcoaDataFile = getOutputFileName("pcoa",variables);
        util.openOutputFile(pcoaDataFile, pcaData);
		pcaData.setf(ios::fixed, ios::floatfield);
		pcaData.setf(ios::showpoint);
		outputNames.push_back(pcoaDataFile);
		outputTypes["pcoa"].push_back(pcoaDataFile);

		ofstream pcaLoadings;
        string loadingsFile = getOutputFileName("loadings",variables);
        util.openOutputFile(loadingsFile, pcaLoadings);
		pcaLoadings.setf(ios::fixed, ios::floatfield);
		pcaLoadings.setf(ios::showpoint);
		outputNames.push_back(loadingsFile);
		outputTypes["loadings"].push_back(loadingsFile);

		pcaLoadings << "axis\tloading\n";
		for(int i=0;i<rank;i++){
			pcaLoadings << i+1 << '\t' << d[i] * 100.0 / dsum << endl;
		}

		pcaData << "group";
		for(int i=0;i<rank;i++){
			pcaData << '\t' << "axis" << i+1;
		}
		pcaData << endl;

		for(int i=0;i<rank;i++){
			pcaData << name_list[i];
			for(int j=0;j<rank;j++){
				pcaData  << '\t' << G[i][j];
			}
			pcaData << endl;
		}
	}
	catch(exception& e) {
		m->errorOut(e, "PCOACommand", "output");
		exit(1);
	}
}

/*********************************************************************************************************************************/

