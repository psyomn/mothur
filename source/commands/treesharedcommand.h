#ifndef TREEGROUPCOMMAND_H
#define TREEGROUPCOMMAND_H

/*
 *  treesharedcommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 4/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */
 
#include "command.hpp"
#include "datastructures/counttable.h"
#include "datastructures/sparsedistancematrix.h"

/* This command create a tree file for each similarity calculator at distance level, using various calculators to find the similiarity between groups. 
	The user can select the lines or labels they wish to use as well as the groups they would like included.
	They can also use as many or as few calculators as they wish. */
	
/**************************************************************************************************************/

class TreeSharedCommand : public Command {
	
public:
	TreeSharedCommand(string);
	TreeSharedCommand();
	~TreeSharedCommand();
	
	vector<string> setParameters();
	string getCommandName()			{ return "tree.shared";				}
	string getCommandCategory()		{ return "OTU-Based Approaches";	}
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "http://www.mothur.org/wiki/Tree.shared"; }
	string getDescription()		{ return "generate a tree file that describes the dissimilarity among groups"; }

	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:
	string lastLabel;
	string format, groupNames, filename, sharedfile, countfile, inputfile;
	int numGroups, subsampleSize, iters, processors;
	ofstream out;
	float precision, cutoff;
    vector<string> Treenames;

	bool abort, allLines, subsample, withReplacement;
	set<string> labels; //holds labels to be used
	string phylipfile, columnfile, namefile, calc, groups, label;
	vector<string>  Estimators, Groups, outputNames; //holds estimators to be used
	
    int createProcesses(SharedRAbundVectors*& thisLookup, CountTable&);
    void printSims(ostream&, vector< vector<double> >&, vector<string>);
    vector< vector<double> > makeSimsDist(SparseDistanceMatrix*, int);
};

/**************************************************************************************************************/
	
#endif


