#ifndef Mothur_removeotulabelscommand_h
#define Mothur_removeotulabelscommand_h


//
//  removeotuscommand.h
//  Mothur
//
//  Created by Sarah Westcott on 5/21/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "command.hpp"
#include "inputdata.h"
#include "datastructures/listvector.hpp"


/**************************************************************************************************/

class RemoveOtusCommand : public Command {
public:
    RemoveOtusCommand(string);
    ~RemoveOtusCommand(){}
    
    vector<string> setParameters();
    string getCommandName()			{ return "remove.otus";          }
    string getCommandCategory()		{ return "OTU-Based Approaches";	} 
    
	string getHelpString();	
    string getOutputPattern(string);	
    string getCitation() { return "http://www.mothur.org/wiki/Get.otus"; }
    string getDescription()		{ return "Can be used with output from classify.otu, otu.association, or corr.axes to remove specific otus."; }
    
    int execute(); 
    void help() { m->mothurOut(getHelpString()); }	
    
private:
    bool abort;
    string  accnosfile, constaxonomyfile, otucorrfile, corraxesfile, listfile, sharedfile, label;
    vector<string> outputNames;
    unordered_set<string> otulabels;
    ListVector* list;
    
    int readClassifyOtu();
    int readOtuAssociation();
    int readCorrAxes();
    int readList();
    int readShared();
    int getListVector();
    SharedRAbundVectors* getShared();
};

/**************************************************************************************************/








#endif
