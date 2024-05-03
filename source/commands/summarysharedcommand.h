#ifndef SUMMARYSHAREDCOMMAND_H
#define SUMMARYSHAREDCOMMAND_H
/*
 *  summarysharedcommand.h
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "command.hpp"

#include "datastructures/sharedrabundvectors.hpp"

class SummarySharedCommand : public Command {

public:
	SummarySharedCommand(string);
	~SummarySharedCommand() = default;
	
	vector<string> setParameters();
	string getCommandName()			{ return "summary.shared";			}
	string getCommandCategory()		{ return "OTU-Based Approaches";	}
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "http://www.mothur.org/wiki/Summary.shared"; }
	string getDescription()		{ return "generate a summary file containing calculator values for each line in the OTU data and for all possible comparisons between groups"; }

	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
	
private:
	vector<linePair> lines;
	
	bool abort, allLines, mult, all, createPhylip, subsample, withReplacement;
	set<string> labels; //holds labels to be used
	string label, calc, groups, sharedfile, output;
	vector<string>  Estimators, Groups, outputNames, sumCalculatorsNames;
	
	string format;
	int numGroups, processors, subsampleSize, iters, numCalcs;
	int process(SharedRAbundVectors*, string, string, vector<string>);
    int printSims(ostream&, vector< vector<double> >&, vector<string>);
    int runCalcs(SharedRAbundVectors*&, string, string, vector< vector<seqDist>  >&);

};

/**************************************************************************************************/
//custom data structure for threads to use.
//main process handling the calcs that can do more than 2 groups
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct summarySharedData {
    vector<SharedRAbundVector*> thisLookup;
    vector< vector<seqDist> > calcDists;
    vector<string>  Estimators;
	unsigned long long start;
	unsigned long long end;
	MothurOut* m;
	string sumFile, sumAllFile;
    int count;
    bool main, mult;
    bool subsample;
    Utils util;
    
	summarySharedData(){}
	summarySharedData(string sf, string sfa, MothurOut* mout, unsigned long long st, unsigned long long en, vector<string> est, SharedRAbundVectors*& lu, bool mai, bool mu, bool sub) {
		sumFile = sf;
        sumAllFile = sfa;
		m = mout;
		start = st;
		end = en;
        Estimators = est;
        thisLookup = lu->getSharedRAbundVectors();
        count=0;
        main = mai;
        mult = mu;
        subsample = sub;
	}
    ~summarySharedData() { for (int j = 0; j < thisLookup.size(); j++) { delete thisLookup[j]; } thisLookup.clear(); }
};
/**************************************************************************************************/

#endif
