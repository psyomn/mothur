#ifndef UNIFRACWEIGHTEDCOMMAND_H
#define UNIFRACWEIGHTEDCOMMAND_H

/*
 *  unifracweightedcommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "command.hpp"

#include "datastructures/counttable.h"
#include "read/readtree.h"

class UnifracWeightedCommand : public Command {

	public:
		UnifracWeightedCommand(string);
		~UnifracWeightedCommand() = default;

		vector<string> setParameters();
		string getCommandName()			{ return "unifrac.weighted";		}
		string getCommandCategory()		{ return "Hypothesis Testing";		}

        string getHelpString();
        string getOutputPattern(string);
		string getCitation() { return "Lozupone CA, Hamady M, Kelley ST, Knight R (2007). Quantitative and qualitative beta diversity measures lead to different insights into factors that structure microbial communities. Appl Environ Microbiol 73: 1576-85. \nhttp://www.mothur.org/wiki/Unifrac.weighted"; }
		string getDescription()		{ return "generic tests that describes whether two or more communities have the same structure"; }

		int execute();
		void help() { m->mothurOut(getHelpString()); }

	private:
        vector<string> Groups, outputNames; // AB. AC, BC...
		bool abort, phylip, random, includeRoot, subsample, consensus, withReplacement;
		string groups, itersString, outputForm, treefile, groupfile, namefile, countfile, sumFile;
		int processors, subsampleSize, subsampleIters, iters, numComp;

		void printWSummaryFile(int, vector<double>, vector<double>, vector<string>);
		void createPhylipFile(int, vector<double>);

        //random comparison functions
		int findIndex(float, int, vector< vector<double> >&);
		void calculateFreqsCumuls(set<double>&, vector< vector<double> > rScores, vector< map<double, double> >&, vector< map<double, double> >&);
		vector<double> createProcesses(Tree*, CountTable*, vector< vector<string> >, vector<vector<int> >&);
        int runRandomCalcs(Tree*, CountTable*, vector<double>, int, vector<double>&, vector<string>);

        vector<Tree*> buildTrees(vector< vector<double> >&, int, CountTable&);
        int getConsensusTrees(vector< vector<double> >&, int);
        int getAverageSTDMatrices(vector< vector<double> >&, int);
};
/**************************************************************************************************/

#endif
