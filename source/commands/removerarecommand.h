#ifndef REMOVERARECOMMAND_H
#define REMOVERARECOMMAND_H

/*
 *  removerarecommand.h
 *  mothur
 *
 *  Created by westcott on 1/21/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"
#include "datastructures/listvector.hpp"
#include "datastructures/sharedrabundvectors.hpp"


class RemoveRareCommand : public Command {
	
public:
	
	RemoveRareCommand(string);	
	~RemoveRareCommand(){}
	
	vector<string> setParameters();
	string getCommandName()			{ return "remove.rare";				}
	string getCommandCategory()		{ return "OTU-Based Approaches";	}
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "http://www.mothur.org/wiki/Remove.rare"; }
	string getDescription()		{ return "removes rare sequences from a sabund, rabund, shared or list and group file"; }

	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:
	string sabundfile, rabundfile, sharedfile, groupfile, countfile, listfile,  groups, label;
	int nseqs, allLines;
	bool abort, byGroup;
	vector<string> outputNames, Groups;
	set<string> labels;
	
	void processSabund();
	void processRabund();
	int processList();
	void processShared();
	void processLookup(SharedRAbundVectors*&);
	
};

#endif




