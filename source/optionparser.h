#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

/*
 *  optionparser.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */



#include "mothur.h"
#include "mothurout.h"
#include "commands/command.hpp"
#include "utils.hpp"
#include "currentfile.h"

/***********************************************************************/

class OptionParser {
public:
	OptionParser(string, vector<string>);
    OptionParser(string, map<string, string>&);
	~OptionParser() = default;
	map<string, string> getParameters(); //adds inputdir to parameters if indicated
	
private:
	map<string, string> parameters;
	MothurOut* m;
    CurrentFile* current;
    Utils util;
    set<string> fileTypes;

    void fillFileTypes(set<string>&);
};

/***********************************************************************/

#endif
