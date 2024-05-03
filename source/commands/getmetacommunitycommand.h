//
//  getmetacommunitycommand.h
//  Mothur
//
//  Created by SarahsWork on 4/9/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#ifndef Mothur_getmetacommunitycommand_h
#define Mothur_getmetacommunitycommand_h

#include "command.hpp"
//#include "sharedjackknife.h"
#include "calculators/calculator.h"

/**************************************************************************************************/

class GetMetaCommunityCommand : public Command {
public:
    GetMetaCommunityCommand(string);
    ~GetMetaCommunityCommand(){}

    vector<string> setParameters();
    string getCommandName()			{ return "get.communitytype";		}
    string getCommandCategory()		{ return "OTU-Based Approaches";         }

    string getOutputPattern(string);

	string getHelpString();
    string getCitation() { return "Holmes I, Harris K, Quince C (2012) Dirichlet Multinomial Mixtures: Generative Models for Microbial Metagenomics. PLoS ONE 7(2): e30126. doi:10.1371/journal.pone.0030126 http://www.mothur.org/wiki/get.communitytype"; }
    string getDescription()		{ return "Assigns samples to bins using a Dirichlet multinomial mixture model"; }

    int execute();
    void help() { m->mothurOut(getHelpString()); }

private:
    bool abort, allLines, subsample, withReplacement;
    vector<string> outputNames;
    string sharedfile, method, calc;
    int minpartitions, maxpartitions, optimizegap, iters, subsampleSize;
    vector<string> Groups, Estimators;
    set<string> labels;

    vector<vector<double> > generateDistanceMatrix(SharedRAbundVectors*& lookup);
    int driver(SharedRAbundVectors*& thisLookup, vector< vector<seqDist> >& calcDists, Calculator*);
    int processDriver(SharedRAbundVectors*&, vector<int>&, string, vector<string>, vector<string>, vector<string>, int);
    int createProcesses(SharedRAbundVectors*&);
    vector<double> generateDesignFile(int, map<string,string>);
    int generateSummaryFile(int, map<string,string>, vector<double>);

};

/**************************************************************************************************/
struct summaryData {

    string name;
    double refMean, difference;
    vector<double> partMean, partLCI, partUCI;

};
/**************************************************************************************************/

#endif
