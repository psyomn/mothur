#ifndef MERGEGROUPSCOMMAND_H
#define MERGEGROUPSCOMMAND_H

/*
 *  mergegroupscommand.h
 *  mothur
 *
 *  Created by westcott on 1/24/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"
#include "inputdata.h"

#include "datastructures/designmap.h"

class MergeGroupsCommand : public Command {
    
#ifdef UNIT_TEST
    friend class TestMergeGroupsCommand;
#endif
	
public:
	MergeGroupsCommand(string);
	~MergeGroupsCommand() = default;
	
	vector<string> setParameters();
	string getCommandName()			{ return "merge.groups";	}
	string getCommandCategory()		{ return "General";			}
	
	string getHelpString();	
    string getOutputPattern(string);	
	string getCitation() { return "http://www.mothur.org/wiki/Merge.groups"; }
	string getDescription()		{ return "reads shared file and a design file and merges the groups in the shared file that are in the same grouping in the design file"; }

	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:
	DesignMap* designMap;
	
	bool abort, allLines, pickedGroups;
	set<string> labels; //holds labels to be used
	string groups, label,  inputDir, designfile, sharedfile, groupfile, countfile, method, fastafile;
	vector<string> Groups, outputNames;
		
	int process(SharedRAbundVectors*&, ofstream&, bool&);
	int processSharedFile(DesignMap*&);
	int processGroupFile(DesignMap*&);
    int processCountFile(DesignMap*&);
    int mergeAbund(vector<int>);
};

#endif

