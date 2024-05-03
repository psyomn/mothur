#ifndef CLUSTERSPLITCOMMAND_H
#define CLUSTERSPLITCOMMAND_H

/*
 *  clustersplitcommand.h
 *  Mothur
 *
 *  Created by westcott on 5/19/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */
 
#include "command.hpp"

#include "cluster.hpp"
#include "datastructures/listvector.hpp"
#include "datastructures/sparsedistancematrix.h"

class ClusterSplitCommand : public Command {
	
public:
	ClusterSplitCommand(string);
	~ClusterSplitCommand() = default;
	
	vector<string> setParameters();
	string getCommandName()			{ return "cluster.split";		}
	string getCommandCategory()		{ return "Clustering";			}
	
	string getHelpString();	
    string getOutputPattern(string);
    string getCommonQuestions();
	string getCitation() { return "Schloss PD, Westcott SL (2011). Assessing and improving methods used in OTU-based approaches for 16S rRNA gene sequence analysis. Appl Environ Microbiol 77:3219. \nhttp://www.mothur.org/wiki/Cluster.split"; }
	string getDescription()		{ return "splits your sequences by distance or taxonomy then clusters into OTUs"; }
	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	

private:
	vector<string> outputNames;
	string file, method, fileroot, tag, namefile, countfile, distfile, format, timing, taxFile, fastafile, inputDir, vsearchLocation, metricName, initialize, type;
	double cutoff, splitcutoff, stableMetric;
	int precision, length, processors, taxLevelCutoff, maxIters, numSingletons;
	bool  abort, classic, runCluster, deleteFiles, isList, cutoffNotSet, makeDist, runsensSpec, showabund; 
	
	void printData(ListVector*);
	vector<string> createProcesses(vector< map<string, string> >, set<string>&);
	int mergeLists(vector<string>, map<double, int>, ListVector*);
	map<double, int> completeListFile(vector<string>, string, set<string>&, ListVector*&);
	int createMergedDistanceFile(vector< map<string, string> >);
    string readFile(vector< map<string, string> >&);
    string printFile(string, vector< map<string, string> >&);
    int getLabels(string, set<string>& listLabels);
    int runSensSpec();
};

#endif

