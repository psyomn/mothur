/*
 *  heatmapsimcommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "heatmapsimcommand.h"

#include "datastructures/nameassignment.hpp"
#include "calculators/sharedjabund.h"
#include "calculators/sharedsorabund.h"
#include "calculators/sharedjclass.h"
#include "calculators/sharedsorclass.h"
#include "calculators/sharedjest.h"
#include "calculators/sharedsorest.h"
#include "calculators/sharedthetayc.h"
#include "calculators/sharedthetan.h"
#include "calculators/sharedmorisitahorn.h"
#include "calculators/sharedbraycurtis.h"
#include "inputdata.h"
#include "validcalculator.h"

//**********************************************************************************************************************
vector<string> HeatMapSimCommand::setParameters(){
	try {
		CommandParameter pshared("shared", "InputTypes", "", "", "PhylipColumnShared", "PhylipColumnShared", "none","svg",false,false,true); parameters.push_back(pshared);
		CommandParameter pphylip("phylip", "InputTypes", "", "", "PhylipColumnShared", "PhylipColumnShared", "none","svg",false,false); parameters.push_back(pphylip);
		CommandParameter pname("name", "InputTypes", "", "", "namecount", "none", "none","",false,false); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "namecount", "none", "none","",false,false); parameters.push_back(pcount);
		CommandParameter pcolumn("column", "InputTypes", "", "", "PhylipColumnShared", "PhylipColumnShared", "ColumnName","svg",false,false); parameters.push_back(pcolumn);
		CommandParameter pgroups("groups", "String", "", "", "", "", "","",false,false); parameters.push_back(pgroups);
		CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
		CommandParameter pcalc("calc", "Multiple", "jabund-sorabund-jclass-sorclass-jest-sorest-thetayc-thetan-morisitahorn-braycurtis", "jest-thetayc", "", "", "","",true,false); parameters.push_back(pcalc);
		CommandParameter pfontsize("fontsize", "Number", "", "24", "", "", "","",false,false); parameters.push_back(pfontsize);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

        abort = false; calledHelp = false; allLines = true; format = "";

        vector<string> tempOutNames;
        outputTypes["svg"] = tempOutNames;

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string HeatMapSimCommand::getHelpString(){
	try {
		string helpString = "";
		ValidCalculators validCalculator;
		helpString += "The heatmap.sim command parameters are shared, phylip, column, name, count, groups, calc, fontsize and label.  shared or phylip or column and name are required unless valid current files exist.\n";
		helpString += "There are two ways to use the heatmap.sim command. The first is with a shared file, and you may use the groups, label and calc parameter. \n";
		helpString += "The groups parameter allows you to specify which of the groups in your groupfile you would like included in your heatmap.\n";
		helpString += "The group names are separated by dashes. The label parameter allows you to select what distance levels you would like a heatmap created for, and is also separated by dashes.\n";
		helpString += "The fontsize parameter allows you to adjust the font size of the picture created, default=24.\n";
		helpString += "The heatmap.sim command should be in the following format: heatmap.sim(groups=yourGroups, calc=yourCalc, label=yourLabels).\n";
		helpString += "Example heatmap.sim(groups=A-B-C, calc=jabund).\n";
		helpString += "The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n";
		helpString +=  validCalculator.printCalc("heat");
		helpString += "The default value for calc is jclass-thetayc.\n";
		helpString += "The heatmap.sim command outputs a .svg file for each calculator you choose at each label you specify.\n";
		helpString += "The second way to use the heatmap.sim command is with a distance file representing the distance bewteen your groups. \n";
		helpString += "Using the command this way, the phylip or column parameter are required, and only one may be used.  If you use a column file the name filename is required. \n";
		helpString += "The heatmap.sim command should be in the following format: heatmap.sim(phylip=yourDistanceFile).\n";
		helpString += "Example heatmap.sim(phylip=amazonGroups.dist).\n";

		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string HeatMapSimCommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "svg") {  pattern = "[filename],svg"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "HeatMapCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************

HeatMapSimCommand::HeatMapSimCommand(string option) : Command()  {
	try {
		if(option == "help") {  help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }

		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();

			ValidParameters validParameter;
			phylipfile = validParameter.validFile(parameters, "phylip");
			if (phylipfile == "not open") { abort = true; }
			else if (phylipfile == "not found") { phylipfile = ""; }
			else {  format = "phylip"; 	inputfile = phylipfile; current->setPhylipFile(phylipfile); if (outputdir == "") { outputdir += util.hasPath(phylipfile); }  }

			columnfile = validParameter.validFile(parameters, "column");
			if (columnfile == "not open") { abort = true; }
			else if (columnfile == "not found") { columnfile = ""; }
			else {  format = "column";	inputfile = columnfile; current->setColumnFile(columnfile); if (outputdir == "") { outputdir += util.hasPath(columnfile); } }

			sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { abort = true; }
			else if (sharedfile == "not found") { sharedfile = ""; }
			else {  format = "shared";	inputfile = sharedfile; current->setSharedFile(sharedfile); if (outputdir == "") { outputdir += util.hasPath(sharedfile); } }

			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") { namefile = ""; }
			else { current->setNameFile(namefile); }

            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not open") { abort = true; countfile = ""; }
			else if (countfile == "not found") { countfile = ""; }
			else { current->setCountFile(countfile); }

            if ((countfile != "") && (namefile != "")) { m->mothurOut("You must enter ONLY ONE of the following: count or name.\n");  abort = true; }

			//error checking on files
			if ((sharedfile == "") && ((phylipfile == "") && (columnfile == "")))	{
				sharedfile = current->getSharedFile();
				if (sharedfile != "") { format = "shared"; inputfile = sharedfile; m->mothurOut("Using " + sharedfile + " as input file for the shared parameter.\n");  }
				else {
					//is there are current file available for either of these?
					//give priority to column, then phylip
					columnfile = current->getColumnFile();
					if (columnfile != "") {  format = "column"; inputfile = columnfile; m->mothurOut("Using " + columnfile + " as input file for the column parameter.\n");  }
					else {
						phylipfile = current->getPhylipFile();
						if (phylipfile != "") { format = "phylip";  inputfile = phylipfile; m->mothurOut("Using " + phylipfile + " as input file for the phylip parameter.\n");  }
						else {
							m->mothurOut("No valid current files. You must provide a shared or phylip or column file.\n");
							abort = true;
						}
					}
				}
			}
			else if ((phylipfile != "") && (columnfile != "")) { m->mothurOut("When running the heatmap.sim command with a distance file you may not use both the column and the phylip parameters.\n");  abort = true; }

			if (columnfile != "") {
				if (namefile == "") {
					namefile = current->getNameFile();
					if (namefile != "") {  m->mothurOut("Using " + namefile + " as input file for the name parameter.\n");  }
					else {
                        countfile = current->getCountFile();
						if (countfile != "") {  m->mothurOut("Using " + countfile + " as input file for the count parameter.\n");  }
                        else {
                            m->mothurOut("You need to provide a name or count file if you are going to use the column format.\n");
                            abort = true;
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
			if (calc == "not found") { calc = "jest-thetayc";  }
			else {
				if (calc == "default")  {  calc = "jest-thetayc";  }
			}
			util.splitAtDash(calc, Estimators);
			if (util.inUsersGroups("citation", Estimators)) {
				ValidCalculators validCalc; validCalc.printCitations(Estimators);
				//remove citation from list of calcs
				for (int i = 0; i < Estimators.size(); i++) { if (Estimators[i] == "citation") {  Estimators.erase(Estimators.begin()+i); break; } }
			}

			groups = validParameter.valid(parameters, "groups");
			if (groups == "not found") { groups = ""; }
			else {
				util.splitAtDash(groups, Groups);
                if (Groups.size() != 0) { if (Groups[0]== "all") { Groups.clear(); } }
			}

			string temp = validParameter.valid(parameters, "fontsize");				if (temp == "not found") { temp = "24"; }
			util.mothurConvert(temp, fontsize);

			if (!abort) {
				ValidCalculators validCalculator;

				int i;
				for (i=0; i<Estimators.size(); i++) {
					if (validCalculator.isValidCalculator("heat", Estimators[i]) ) {
						if (Estimators[i] == "jabund") {
							heatCalculators.push_back(new JAbund());
						}else if (Estimators[i] == "sorabund") {
							heatCalculators.push_back(new SorAbund());
						}else if (Estimators[i] == "jclass") {
							heatCalculators.push_back(new Jclass());
						}else if (Estimators[i] == "sorclass") {
							heatCalculators.push_back(new SorClass());
						}else if (Estimators[i] == "jest") {
							heatCalculators.push_back(new Jest());
						}else if (Estimators[i] == "sorest") {
							heatCalculators.push_back(new SorEst());
						}else if (Estimators[i] == "thetayc") {
							heatCalculators.push_back(new ThetaYC());
						}else if (Estimators[i] == "thetan") {
							heatCalculators.push_back(new ThetaN());
						}else if (Estimators[i] == "morisitahorn") {
							heatCalculators.push_back(new MorHorn());
						}else if (Estimators[i] == "braycurtis") {
							heatCalculators.push_back(new BrayCurtis());
						}
					}
				}

			}
		}



	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "HeatMapSimCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int HeatMapSimCommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

		heatmap = new HeatMapSim(outputdir, inputfile, fontsize);

		if (format == "shared") { runCommandShared();   }
		else                    {	runCommandDist();	}

		delete heatmap;

		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } outputTypes.clear(); return 0; }

		m->mothurOut("\nOutput File Names: \n");
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************
int HeatMapSimCommand::runCommandShared() {
	try {
		//if the users entered no valid calculators don't execute command
		if (heatCalculators.size() == 0) { m->mothurOut("No valid calculators.\n");  return 0; }

        InputData input(sharedfile, "sharedfile", Groups);
        set<string> processedLabels;
        set<string> userLabels = labels;
        string lastLabel = "";

		SharedRAbundVectors* lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        Groups = lookup->getNamesGroups();

		if (lookup->size() < 2) { m->mothurOut("[ERROR]: You have not provided enough valid groups.  I cannot run the command, quitting\n"); return 0;}

        while (lookup != nullptr) {

            if (m->getControl_pressed()) { delete lookup; break; }

            vector<string> outfilenames = heatmap->getPic(lookup, heatCalculators, lookup->getNamesGroups()); delete lookup;

            for(int i = 0; i < outfilenames.size(); i++) { outputNames.push_back(outfilenames[i]);  outputTypes["svg"].push_back(outfilenames[i]); }

            lookup = util.getNextShared(input, allLines, userLabels, processedLabels, lastLabel);
        }

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "runCommandShared");
		exit(1);
	}
}
//**********************************************************************************************************************
int HeatMapSimCommand::runCommandDist() {
	try {

		vector< vector<double> > matrix;
		vector<string> names;
		ifstream in;

		//read distance file and create distance vector and names vector
		if (format == "phylip") {
			//read phylip file
			util.openInputFile(phylipfile, in);

			string name;
			int numSeqs;
			in >> numSeqs >> name;

			//save name
			names.push_back(name);

			//resize the matrix and fill with zeros
			matrix.resize(numSeqs);
			for(int i = 0; i < numSeqs; i++) {
				matrix[i].resize(numSeqs, 0.0);
			}

			//determine if matrix is square or lower triangle
			//if it is square read the distances for the first sequence
			char d;
			bool square = false;
			while((d=in.get()) != EOF){

				//is d a number meaning its square
				if(isalnum(d)){
					square = true;
					in.putback(d);

					for(int i=0;i<numSeqs;i++){
						in >> matrix[0][i];
					}
					break;
				}

				//is d a line return meaning its lower triangle
				if(d == '\n'){
					square = false;
					break;
				}
			}

			//read rest of matrix
			if (square ) {
				for(int i=1;i<numSeqs;i++){
					in >> name;
					names.push_back(name);

					if (m->getControl_pressed()) { return 0; }

					for(int j=0;j<numSeqs;j++) { in >> matrix[i][j];  }
					gobble(in);
				}
			}else {
				double dist;
				for(int i=1;i<numSeqs;i++){
					in >> name;
					names.push_back(name);

					if (m->getControl_pressed()) { return 0; }

					for(int j=0;j<i;j++){
						in >> dist;
						matrix[i][j] = dist;  matrix[j][i] = dist;
					}
					gobble(in);
				}
			}
			in.close();
		}else {
			//read names file
			NameAssignment* nameMap;
            CountTable ct;
            if (namefile != "") {
                nameMap = new NameAssignment(namefile);
                nameMap->readMap();

                //put names in order in vector
                for (int i = 0; i < nameMap->size(); i++) {
                    names.push_back(nameMap->get(i));
                }
             }else if (countfile != "") {
                nameMap = nullptr;
                ct.readTable(countfile, true, false);
                names = ct.getNamesOfSeqs();
            }

            //resize matrix
            matrix.resize(names.size());
            for (int i = 0; i < names.size(); i++) {
                matrix[i].resize(names.size(), 0.0);
            }

			//read column file
			string first, second;
			double dist;
			util.openInputFile(columnfile, in);

			while (!in.eof()) {
				in >> first >> second >> dist; gobble(in);

				if (m->getControl_pressed()) { return 0; }

                if (namefile != "") {
                    map<string, int>::iterator itA = nameMap->find(first);
                    map<string, int>::iterator itB = nameMap->find(second);

                    if(itA == nameMap->end()){  m->mothurOut("AAError: Sequence '" + first + "' was not found in the names file, please correct\n"); exit(1);  }
                    if(itB == nameMap->end()){  m->mothurOut("ABError: Sequence '" + second + "' was not found in the names file, please correct\n"); exit(1);  }

                    //save distance
                    matrix[itA->second][itB->second] = dist;
                    matrix[itB->second][itA->second] = dist;
                }else if (countfile != "") {
                    int itA = ct.get(first);
                    int itB = ct.get(second);
                    matrix[itA][itB] = dist;
                    matrix[itB][itA] = dist;
                }
			}
			in.close();

			if (namefile != "") { delete nameMap; }
		}


		string outputFileName = heatmap->getPic(matrix, names);
		outputNames.push_back(outputFileName); //vector<vector<double>>, vector<string>
		outputTypes["svg"].push_back(outputFileName);

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "HeatMapSimCommand", "runCommandDist");
		exit(1);
	}
}
//**********************************************************************************************************************






