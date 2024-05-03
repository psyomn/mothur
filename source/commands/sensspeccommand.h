#ifndef SENSSPECCOMMAND_H
#define SENSSPECCOMMAND_H


/*
 *  sensspeccommand.h
 *  Mothur
 *
 *  Created by Pat Schloss on 7/6/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"

#include "datastructures/listvector.hpp"
#include "datastructures/optimatrix.h"

class SensSpecCommand : public Command {

public:
	SensSpecCommand(string);
	~SensSpecCommand(){}

	vector<string> setParameters();
	string getCommandName()			{ return "sens.spec";				}
	string getCommandCategory()		{ return "OTU-Based Approaches";	}

	string getHelpString();
    string getOutputPattern(string);
	string getCitation() { return "Schloss PD, Westcott SL (2011). Assessing and improving methods used in OTU-based approaches for 16S rRNA gene sequence analysis. Appl Environ Microbiol 77:3219.\nhttp://www.mothur.org/wiki/Sens.spec"; }
	string getDescription()		{ return "sens.spec"; }

	int execute();
	void help() { m->mothurOut(getHelpString()); }

private:
	vector< vector< int> > preProcessList(OptiMatrix& matrix, ListVector*);
	void processListFile();
	void setUpOutput();
	void outputStatistics(string, string, int);

	string listFile, distFile, sensSpecFileName, phylipfile, columnfile, namefile, countfile;
	string format;
	vector<string> outputNames;
	set<string> labels; //holds labels to be used

    double truePositives, falsePositives, trueNegatives, falseNegatives;
	bool abort, allLines, square;
	double cutoff;
	int precision;

	int process(ListVector*&, bool&, string&);
};

#endif
