#ifndef SUBSAMPLECOMMAND_H
#define SUBSAMPLECOMMAND_H

/*
 *  subsamplecommand.h
 *  Mothur
 *
 *  Created by westcott on 10/27/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"

#include "datastructures/listvector.hpp"
#include "datastructures/rabundvector.hpp"
#include "datastructures/counttable.h"

class SubSampleCommand : public Command {

public:
	SubSampleCommand(string);
	~SubSampleCommand() = default;

	vector<string> setParameters();
	string getCommandName()			{ return "sub.sample";	}
	string getCommandCategory()		{ return "Sequence Processing";		}

	string getHelpString();
    string getOutputPattern(string);
	string getCitation() { return "http://www.mothur.org/wiki/Sub.sample"; }
	string getDescription()		{ return "get a sampling of sequences from a list, shared, rabund, sabund or fasta file"; }

	int execute();
	void help() { m->mothurOut(getHelpString()); }

private:
	bool abort, pickedGroups, allLines, persample, withReplacement;
	string listfile, groupfile, countfile, sharedfile, rabundfile, sabundfile, fastafile, namefile, taxonomyfile, treefile, constaxonomyfile;
	set<string> labels; //holds labels to be used
	string groups, label;
	vector<string> Groups, outputNames;
	int size;
	//vector<string> names;
	map<string, vector<string> > nameMap;
    CountTable ct;

	int getSubSampleShared();
	int getSubSampleList();
	void getSubSampleRabund();
	void getSubSampleSabund();
	int getSubSampleFasta();
    int getSubSampleTree();
	int processShared(SharedRAbundVectors*&);
	int processRabund(RAbundVector*&, ofstream&);
	int processSabund(SAbundVector*&, ofstream&);
	int processList(ListVector*&, set<string>&);
	vector<string> getNames();
	vector<string> readNames();
    int getTax(set<string>&);

};

#endif

