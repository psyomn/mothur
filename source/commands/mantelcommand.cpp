/*
 *  mantelcommand.cpp
 *  mothur
 *
 *  Created by westcott on 2/9/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "mantelcommand.h"
#include "read/readphylipvector.h"


//**********************************************************************************************************************
vector<string> MantelCommand::setParameters(){	
	try {
		CommandParameter pphylip1("phylip1", "InputTypes", "", "", "none", "none", "none","mantel",false,true,true); parameters.push_back(pphylip1);
		CommandParameter pphylip2("phylip2", "InputTypes", "", "", "none", "none", "none","mantel",false,true,true); parameters.push_back(pphylip2);
		CommandParameter piters("iters", "Number", "", "1000", "", "", "","",false,false); parameters.push_back(piters);
		CommandParameter pmethod("method", "Multiple", "pearson-spearman-kendall", "pearson", "", "", "","",false,false); parameters.push_back(pmethod);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["mantel"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "MantelCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string MantelCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "Sokal, R. R., & Rohlf, F. J. (1995). Biometry, 3rd edn. New York: Freeman.\n";
		helpString += "The mantel command reads two distance matrices and calculates the mantel correlation coefficient.\n";
		helpString += "The mantel command parameters are phylip1, phylip2, iters and method.  The phylip1 and phylip2 parameters are required.  Matrices must be the same size and contain the same names.\n";
		helpString += "The method parameter allows you to select what method you would like to use. Options are pearson, spearman and kendall. Default=pearson.\n";
		helpString += "The iters parameter allows you to set number of randomization for the P value.  The default is 1000. \n";
		helpString += "The mantel command should be in the following format: mantel(phylip1=veg.dist, phylip2=env.dist).\n";
		helpString += "The mantel command outputs a .mantel file.\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "MantelCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string MantelCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "mantel") {  pattern = "[filename],mantel"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "MantelCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
MantelCommand::MantelCommand(string option) : Command()  {
	try {
		
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			phylipfile1 = validParameter.validFile(parameters, "phylip1");
			if (phylipfile1 == "not open") { phylipfile1 = ""; abort = true; }
			else if (phylipfile1 == "not found") { phylipfile1 = ""; m->mothurOut("phylip1 is a required parameter for the mantel command.\n");  abort = true;  }
			
			phylipfile2 = validParameter.validFile(parameters, "phylip2");
			if (phylipfile2 == "not open") { phylipfile2 = ""; abort = true; }
			else if (phylipfile2 == "not found") { phylipfile2 = ""; m->mothurOut("phylip2 is a required parameter for the mantel command.\n"); abort = true;  }
			
					if (outputdir == ""){    outputdir = util.hasPath(phylipfile1);	}
			
			method = validParameter.valid(parameters, "method");		if (method == "not found"){	method = "pearson";		}
			
			string temp = validParameter.valid(parameters, "iters");			if (temp == "not found") { temp = "1000"; }
			util.mothurConvert(temp, iters);
			
			if ((method != "pearson") && (method != "spearman") && (method != "kendall")) { m->mothurOut(method + " is not a valid method. Valid methods are pearson, spearman, and kendall.\n"); abort = true; }
		}
	}
	catch(exception& e) {
		m->errorOut(e, "MantelCommand", "MantelCommand");		
		exit(1);
	}
}
//**********************************************************************************************************************

int MantelCommand::execute(){
	try {
		
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
		
		/***************************************************/
		//    reading distance files					   //
		/***************************************************/
		
		//read phylip1
		ReadPhylipVector readMatrix(phylipfile1);
		vector< vector<double> > matrix1;
		vector<string> names1 = readMatrix.read(matrix1);
		
		if (m->getControl_pressed()) { return 0; }
		
		//read phylip2
		ReadPhylipVector readMatrix2(phylipfile2);
		vector< vector<double> > matrix2;
		vector<string> names2 = readMatrix2.read(matrix2);
		
		if (m->getControl_pressed()) { return 0; }
		
		//make sure matrix2 and matrix1 are in the same order
		if (names1 == names2) { //then everything is in same order and same size
		}else if (names1.size() != names2.size()) { //wrong size no need to order, abort
			m->mothurOut("[ERROR]: distance matrices are not the same size, aborting.\n"); 
			m->setControl_pressed(true);
		}else { //sizes are the same, but either the names are different or they are in different order
			m->mothurOut("[WARNING]: Names do not match between distance files. Comparing based on order in files.\n"); 
		}	
		
		if (m->getControl_pressed()) { return 0; }
		
		/***************************************************/
		//    calculating mantel and signifigance		   //
		/***************************************************/
		
		//calc mantel coefficient
		LinearAlgebra linear;
		double mantel = 0.0;
		if (method == "pearson")		{  mantel = linear.calcPearson(matrix1, matrix2);	}
		else if (method == "spearman")	{  mantel = linear.calcSpearman(matrix1, matrix2);	}
		else if (method == "kendall")	{  mantel = linear.calcKendall(matrix1, matrix2);	}
		
		
		//calc signifigance
		int count = 0;
		for (int i = 0; i < iters; i++) {
			
			if (m->getControl_pressed()) { return 0; }
			
			//randomize matrix2
			vector< vector<double> > matrix2Copy = matrix2;
			util.mothurRandomShuffle(matrix2Copy);
		
			//calc random mantel
			double randomMantel = 0.0;
			if (method == "pearson")		{  randomMantel = linear.calcPearson(matrix1, matrix2Copy);	}
			else if (method == "spearman")	{  randomMantel = linear.calcSpearman(matrix1, matrix2Copy);	}
			else if (method == "kendall")	{  randomMantel = linear.calcKendall(matrix1, matrix2Copy);	}
			
			if (randomMantel >= mantel) { count++; }
		}
		
		double pValue = count / (float) iters;
		
		if (m->getControl_pressed()) { return 0; }
		
        map<string, string> variables; 
		variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(phylipfile1));
		string outputFile = getOutputFileName("mantel",variables);
		outputNames.push_back(outputFile); outputTypes["mantel"].push_back(outputFile);
		ofstream out;
		
		util.openOutputFile(outputFile, out);
		
		out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
		cout.setf(ios::fixed, ios::floatfield); cout.setf(ios::showpoint);
		
		out << "Mantel\tpValue" << endl;
		out << mantel << '\t' << pValue << endl;
		
		out.close();
	
		m->mothurOut("\nmantel = " + toString(mantel) + "\tpValue = " + toString(pValue) + "\n");
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();		
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "MantelCommand", "execute");	
		exit(1);
	}
}

//**********************************************************************************************************************


